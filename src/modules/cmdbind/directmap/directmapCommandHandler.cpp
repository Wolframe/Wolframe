/************************************************************************
Copyright (C) 2011 - 2014 Project Wolframe.
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
#include "serialize/struct/structParser.hpp"
#include "serialize/struct/structSerializer.hpp"
#include "serialize/tostringUtils.hpp"
#include "types/docmetadata.hpp"
#include "types/variant.hpp"
#include "filter/redirectFilterClosure.hpp"
#include "filter/typingfilter.hpp"
#include "logger-v1.hpp"
#include <stdexcept>
#include <cstddef>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace cmdbind;
using namespace langbind;

void DirectmapCommandHandler::initcall()
{
	if (!execContext()) throw std::logic_error( "execution context is not defined");

	// Check if we are allowed to execute the command:
	if (!m_cmd->authfunction.empty())
	{
		if (!execContext()->checkAuthorization( m_cmd->authfunction, m_cmd->authresource))
		{
			throw std::runtime_error( std::string( "not authorized to execute command '") + m_name + "'");
		}
	}
	// Initialize input form for validation if defined:
	if (m_cmd->inputform)
	{
		m_inputform.reset( new types::Form( m_cmd->inputform));
	}
	// Initialize output form for validation if defined:
	if (m_cmd->outputform)
	{
		m_outputform.reset( new types::Form( m_cmd->outputform));
	}

	// Initialize filters:
	if (m_inputfilter.get())
	{
		m_inputfilter->setValue( "empty", "false");
	}
	m_outputfilter->inheritMetaData( m_inputfilter->getMetaDataRef());
	m_outputfilter->setMetaData( m_cmd->outputmetadata);

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
					/* no break here ! */;
				}
				else
				{
					m_state = 3;
					continue;
				}
			case 2:
			{
				if (!m_inputform_parser->call()) return IOFilterCommandHandler::Yield;
				m_input.reset( new serialize::DDLStructSerializer( m_inputform.get()));
				m_state = 3;
				/* no break here ! */
			}
			case 3:
				m_functionclosure.reset( m_cmd->function->createClosure());
				m_functionclosure->init( execContext(), m_input);
				m_state = 4;
				/* no break here ! */
			case 4:
			{
				if (!m_functionclosure->call()) return IOFilterCommandHandler::Yield;
				langbind::TypedInputFilterR res = m_functionclosure->result();
				if (!m_cmd->has_result)
				{
					langbind::InputFilter::ElementType typ;
					types::VariantConst element;

					if (res.get() && res->getNext( typ, element))
					{
						if (typ != langbind::FilterBase::CloseTag)
						{
							LOG_WARNING << "Function called is returning a result but no RETURN declared in command. The function result is ignored";
						}
					}
					m_state = 6;
					continue;
				}
				else if (m_outputform.get())
				{
					// Validate the output with form -> Fill the output form:
					serialize::DDLStructParser formparser( m_outputform.get());
					formparser.init( m_functionclosure->result(), serialize::Context::ValidateInitialization);
					if (!formparser.call())
					{
						throw std::runtime_error( "internal: output form serialization is not complete");
					}
					// Pass serializer of the data in the output form ro printer:
					langbind::TypedInputFilterR outputform_serialize( new serialize::DDLStructSerializer( m_outputform.get()));
					m_outputprinter.init( outputform_serialize, m_output);
					m_state = 5;
				}
				else 
				{
					// SKIP output -> Pass function result iterator directly to printer:
					m_outputprinter.init( m_functionclosure->result(), m_output);
					m_state = 5;
				}
				/* no break here ! */
			}
			case 5:
			{
				if (!m_outputprinter.call()) return IOFilterCommandHandler::Yield;
				m_state = 6;
				continue;
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
	catch (const std::logic_error& e)
	{
		LOG_FATAL << "logic error processing request: " << e.what();
		throw e;
	}
}


