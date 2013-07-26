/************************************************************************
Copyright (C) 2011 - 2013 Project Wolframe.
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
#include "directmapCommandHandler.hpp"
#include "serialize/struct/filtermapBase.hpp"
#include "serialize/tostringUtils.hpp"
#include "types/doctype.hpp"
#include "types/variant.hpp"
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

void DirectmapContext::load( const std::vector<std::string>& prgfiles_, const module::ModulesDirectory*)
{
	std::vector<std::string>::const_iterator ci = prgfiles_.begin(), ce = prgfiles_.end();
	for (; ci != ce; ++ci) m_program.loadProgram( *ci);
}

void DirectmapCommandHandler::initcall()
{
	m_cmd = m_ctx->command( m_name);
	if (!m_cmd->inputform.empty())
	{
		const types::FormDescription* df = m_provider->formDescription( m_cmd->inputform);
		if (!df)
		{
			throw std::runtime_error( std::string( "input form is not defined '") + m_cmd->inputform + "'");
		}
		m_inputform.reset( new types::Form( df));
	}
	if (!m_cmd->outputform.empty())
	{
		const types::FormDescription* df = m_provider->formDescription( m_cmd->outputform);
		if (!df)
		{
			throw std::runtime_error( std::string( "output form is not defined '") + m_cmd->outputform + "'");
		}
		m_outputform.reset( new types::Form( df));
	}
	std::string filtername;
	if (m_cmd->filter.empty())
	{
		throw std::runtime_error( "empty filter definition in command");
	}
	else
	{
		filtername = m_cmd->filter;
	}
	m_function = m_provider->formFunction( m_cmd->call);
	if (!m_function)
	{
		throw std::runtime_error( std::string( "function not defined '") + m_cmd->call + "'");
	}
	types::CountedReference<langbind::Filter> filter( m_provider->filter( filtername, ""));
	if (!filter.get())
	{
		throw std::runtime_error( std::string( "filter not defined '") + filtername + "'");
	}
	if (!filter->inputfilter().get())
	{
		throw std::runtime_error( std::string( "input filter not defined '") + filtername + "'");
	}
	if (m_inputfilter.get())
	{
		setFilterAs( filter->inputfilter());
		m_inputfilter->setValue( "empty", "false");
	}
	else
	{
		m_inputfilter = filter->inputfilter();
	}
	if (!filter->outputfilter().get())
	{
		throw std::runtime_error( std::string( "output filter not defined '") + filtername + "'");
	}
	if (m_outputfilter.get())
	{
		setFilterAs( filter->outputfilter());
	}
	else
	{
		m_outputfilter = filter->outputfilter();
	}
	if (m_outputform.get())
	{
		const char* xmlroot = m_outputform->description()->xmlRoot();
		if (xmlroot)
		{
			std::string xmlDoctype = m_provider->xmlDoctypeString( m_outputform->description()->name(), m_outputform->description()->ddlname(), xmlroot);
			m_outputfilter->setDocType( xmlDoctype);
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
				if (m_inputform.get())
				{
					m_inputform_parser.reset( new serialize::DDLStructParser( m_inputform.get()));
					serialize::Context::Flags flags = (serialize::Context::Flags)((int)serialize::Context::ValidateAttributes|(int)serialize::Context::ValidateInitialization);
					m_inputform_parser->init( m_input, flags);
					m_state = 2;
					continue;
				}
				else
				{
					std::string doctype;
					if (m_inputfilter->getDocType( doctype) && !doctype.empty())
					{
						// if no input form is defined we check for the input document type and set the form on our own:
						std::string doctypeid( types::getIdFromDoctype( doctype));
						const types::FormDescription* df = m_provider->formDescription( doctypeid);
						if (df)
						{
							m_inputform.reset( new types::Form( df));
							m_inputform_parser.reset( new serialize::DDLStructParser( m_inputform.get()));
							m_inputform_parser->init( m_input, serialize::Context::ValidateAttributes);
							m_state = 2;
						}
						else
						{
							LOG_WARNING << "input form '" << doctypeid << "' is not defined (document type '" << doctypeid << "'). treating document as standalone (document processed with root element ignored)";
							m_state = 11;
						}
						continue;
					}
					else
					{
						switch (m_inputfilter->state())
						{
							case InputFilter::Open:
							{
								m_state = 11;
								LOG_WARNING << "input form: standalone document type and no input form defined. document processed with root element ignored";
								break;
							}
							case InputFilter::EndOfMessage: return IOFilterCommandHandler::Yield;
							case InputFilter::Error: throw std::runtime_error( std::string( "error in input: ") + m_inputfilter->getError());
						}
					}
				}
			case 11:
			{
				// ... treat document as standalone: swallow root element
				langbind::InputFilter::ElementType typ;
				types::VariantConst element;
				if (!m_input->getNext( typ, element))
				{
					switch (m_inputfilter->state())
					{
						case InputFilter::Open: throw std::runtime_error( "unexpected end of standalone document. no root element defined");
						case InputFilter::EndOfMessage: return IOFilterCommandHandler::Yield;
						case InputFilter::Error: throw std::runtime_error( std::string( "error in input: ") + m_inputfilter->getError());
					}
				}
				m_state = 3;
				break;
			}
			case 2:
			{
				if (!m_inputform_parser->call()) return IOFilterCommandHandler::Yield;
				const char* xmlroot = m_inputform->description()->xmlRoot();
				if (xmlroot)
				{
					m_input.reset( new serialize::DDLStructSerializer( m_inputform->select( xmlroot)));
				}
				else
				{
					m_input.reset( new serialize::DDLStructSerializer( m_inputform.get()));
				}
				m_state = 3;
				/* no break here ! */
			}
			case 3:
				m_functionclosure.reset( m_function->createClosure());
				m_functionclosure->init( m_provider, m_input);
				m_state = 4;
				/* no break here ! */
			case 4:
				if (!m_functionclosure->call()) return IOFilterCommandHandler::Yield;
				if (m_outputform.get())
				{
					const char* xmlroot = m_outputform->description()->xmlRoot();
					types::VariantStruct* substructure = (xmlroot)?m_outputform->select(xmlroot):m_outputform.get();

					serialize::DDLStructParser formparser( substructure);
					formparser.init( m_functionclosure->result(), serialize::Context::None);
					if (!formparser.call())
					{
						throw std::logic_error( "output form serialization not complete");
					}
					m_outputform_serialize.reset( new serialize::DDLStructSerializer( m_outputform.get()));
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


