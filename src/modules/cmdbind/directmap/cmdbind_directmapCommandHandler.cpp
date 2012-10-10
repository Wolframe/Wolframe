/************************************************************************
Copyright (C) 2011, 2012 Project Wolframe.
All rights reserved.

This file is part of Project Wolframe.

Commercial Usage
Licensees holding valid Project Wolframe Commercial licenses may
use this file in accordance with the Project Wolframe
Commercial License Agreement provided with the Software or,
alternatively, in accordance with the terms contained
in a written agreement between the licensee and Project Wolframe.

GNU General Public License Usage
Alternatively, you can redistribute this file and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Wolframe is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
#include "cmdbind/directmapCommandHandler.hpp"
#include "langbind/directmapConfig_struct.hpp"
#include "serialize/struct/filtermapBase.hpp"
#include "utils/doctype.hpp"
#include "langbind/appObjects.hpp"
#include "filter/typingfilter.hpp"
#include "logger-v1.hpp"
#include <stdexcept>
#include <cstddef>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace cmdbind;
using namespace langbind;

void DirectmapContext::load( const DirectmapConfigStruct& cfg_, const module::ModulesDirectory*)
{
	std::vector<DirectmapCommandConfigStruct>::const_iterator ii = cfg_.command.begin(), ee = cfg_.command.end();
	std::size_t idx = m_cfg.command.size();
	for (; ii != ee; ++ii,++idx)
	{
		std::string key( boost::algorithm::to_lower_copy( ii->name));
		if (m_map.find( key) != m_map.end())
		{
			throw std::runtime_error( std::string( "duplicate definition of directmap command '") + ii->name + "'");
		}
		m_map[ key] = idx;
		m_cfg.command.push_back( *ii);
	}
}

std::list<std::string> DirectmapContext::commands() const
{
	std::list<std::string> rt;
	std::vector<DirectmapCommandConfigStruct>::const_iterator ii = m_cfg.command.begin(), ee = m_cfg.command.end();
	for (; ii != ee; ++ii)
	{
		rt.push_back( ii->name);
	}
	return rt;
}

void DirectmapCommandHandler::initcall()
{
	m_cmd = m_ctx->command( m_name);
	if (!m_cmd->inputform.empty())
	{
		const ddl::Form* df = m_provider->form( m_cmd->inputform);
		if (!df)
		{
			throw std::runtime_error( std::string( "input form is not defined '") + m_cmd->inputform + "'");
		}
		m_inputform = *df;
		m_inputform_defined = true;
	}
	if (!m_cmd->outputform.empty())
	{
		const ddl::Form* df = m_provider->form( m_cmd->outputform);
		if (!df)
		{
			throw std::runtime_error( std::string( "output form is not defined '") + m_cmd->outputform + "'");
		}
		m_outputform = *df;
		m_outputform_defined = true;
	}
	m_function = m_provider->transactionFunction( m_cmd->call);
	if (!m_function)
	{
		throw std::runtime_error( std::string( "transaction function not defined '") + m_cmd->call + "'");
	}
	types::CountedReference<langbind::Filter> filter( m_provider->filter( m_cmd->filter, ""));
	if (!filter.get())
	{
		throw std::runtime_error( std::string( "filter not defined '") + m_cmd->filter + "'");
	}
	if (!filter->inputfilter().get())
	{
		throw std::runtime_error( std::string( "input filter not defined '") + m_cmd->filter + "'");
	}
	if (m_inputfilter.get())
	{
		setFilterAs( filter->inputfilter());
		m_inputfilter->setValue( "empty", "false");
	}
	if (!filter->outputfilter().get())
	{
		throw std::runtime_error( std::string( "output filter not defined '") + m_cmd->filter + "'");
	}
	if (m_outputfilter.get())
	{
		setFilterAs( filter->outputfilter());
		if (m_outputform.doctype())
		{
			m_outputfilter->setDocType( m_outputform.doctype());
		}
	}
	m_input.reset( new langbind::TypingInputFilter( m_inputfilter));
	m_output.reset( new langbind::TypingOutputFilter( m_outputfilter));
}

IOFilterCommandHandler::CallResult DirectmapCommandHandler::call( const char*& err)
{
	try
	{
		for (;;) switch (m_state)
		{
			case 0:
				m_state = 1;
				initcall();
				/* no break here ! */
			case 1:
				if (m_inputform_defined)
				{
					m_inputform_parser.reset( new serialize::DDLStructParser( &m_inputform));
					m_inputform_parser->init( m_input, serialize::Context::ValidateAttributes);
					m_state = 2;
					continue;
				}
				else
				{
					std::string doctype;
					if (m_inputfilter->getDocType( doctype) && !doctype.empty())
					{
						// if no input form is defined we check for the input document type and set the form on our own:
						std::string doctypeid( utils::getIdFromDoctype( doctype));
						const ddl::Form* df = m_provider->form( doctypeid);
						if (!df) throw std::runtime_error( std::string( "input form '") + doctypeid + "' is not defined (document type '" + doctypeid + "')");

						m_inputform = *df;
						m_inputform_parser.reset( new serialize::DDLStructParser( &m_inputform));
						m_inputform_parser->init( m_input, serialize::Context::ValidateAttributes);
						m_state = 2;
						continue;
					}
					else
					{
						// check if the input is standalone and continue without mapping input to a form (state 3):
						switch (m_inputfilter->state())
						{
							case InputFilter::Open: m_state = 3; continue;
							case InputFilter::EndOfMessage: return IOFilterCommandHandler::Yield;
							case InputFilter::Error: throw std::runtime_error( std::string( "error in input: ") + m_inputfilter->getError());
						}
						return IOFilterCommandHandler::Error;
					}
				}
			case 2:
				if (!m_inputform_parser->call()) return IOFilterCommandHandler::Yield;
				m_input.reset( new serialize::DDLStructSerializer( &m_inputform));
				m_state = 3;
				/* no break here ! */
			case 3:
				m_functionclosure.reset( new TransactionFunctionClosure( m_provider, m_function));
				m_functionclosure->init( m_input);
				m_state = 4;
				/* no break here ! */
			case 4:
				if (!m_functionclosure->call()) return IOFilterCommandHandler::Yield;
				if (m_outputform_defined)
				{
					serialize::DDLStructParser formparser( &m_outputform);
					formparser.init( m_functionclosure->result(), serialize::Context::ValidateAttributes);
					if (!formparser.call())
					{
						throw std::logic_error( "output form serialization not complete");
					}
					m_outputform_serialize.reset( new serialize::DDLStructSerializer( &m_outputform));
					m_outputprinter.init( m_outputform_serialize, m_output);
				}
				else
				{
					m_outputprinter.init( m_functionclosure->result(), m_output);
				}
				m_state = 5;
				/* no break here ! */
			case 5:
			{
				if (!m_outputprinter.call()) return IOFilterCommandHandler::Yield;
				m_state = 6;
				return IOFilterCommandHandler::Ok;
			}
			default:
				return IOFilterCommandHandler::Ok;
		}
	}
	catch (const std::runtime_error& e)
	{
		m_errormsg = e.what();
		err = m_errormsg.c_str();
		return IOFilterCommandHandler::Error;
	}
}


