/************************************************************************
Copyright (C) 2011 Project Wolframe.
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
#include "dispatcher.hpp"
#include "methodtable.hpp"
#include "logger.hpp"

using namespace _Wolframe::mtproc;
using namespace _Wolframe::protocol;

void CommandDispatcher::resetCommand()
{
	m_methodIdx = 0;
	m_command = unknown;
	m_lineBuffer.clear();
	m_argBuffer.clear();
	m_argc = 0;
	m_argv = 0;

	if (m_instance)
	{
		m_state = Init;
		m_context.init( m_instance->m_data);
	}
	else
	{
		m_state = Null;
		m_context.init( 0);
	}
}

void CommandDispatcher::init( const char** protocolCmds, Instance* instance)
{
	m_instance = instance;
	m_parser.init();
	resetCommand();

	if (protocolCmds)
	{
		for( unsigned int ii=0; protocolCmds[ ii]; ii++)
		{
			m_parser.add( protocolCmds[ ii]);
		}
	}
	if (instance && instance->m_mt)
	{
		for( unsigned int ii=0; instance->m_mt[ii].call && instance->m_mt[ii].name; ii++)
		{
			m_parser.add( instance->m_mt[ii].name);
		}
	}
}

void CommandDispatcher::protocolInput( InputBlock::iterator& start, InputBlock::iterator& end, bool eoD)
{
	if (m_context.contentIterator)
	{
		m_context.contentIterator->protocolInput( start.ptr(), end-start, eoD);
	}
	else
	{
		LOG_ERROR << "illegal state: got input without recipient (content iterator disapeared)";
		throw (IllegalState());
	}
}

bool CommandDispatcher::getOutput( void** output, unsigned int* outputsize)
{
	if (!m_context.output) return false;
	if (!m_context.output->pos()) return false;
	*output = m_context.output->ptr();
	*outputsize = m_context.output->pos();
	return true;
}

void CommandDispatcher::setOutputBuffer( void* buf, unsigned int bufsize)
{
	if (m_context.output)
	{
		m_context.output->init( buf, bufsize);
	}
	else
	{
		LOG_ERROR << "illegal state: declare output buffer without format output object existing";
		throw (IllegalState());
	}
}

CommandDispatcher::CommandDispatcher( Instance* instance)
	:m_argBuffer( &m_lineBuffer)
{
	init( instance);
}

CommandDispatcher::Command CommandDispatcher::getCommand( InputBlock::iterator& itr, InputBlock::iterator& eoM)
{
	LOG_DATA << "Dispatcher State: " << stateName(m_state);

	switch (m_state)
	{
		case Running:
		case Null:
		case Init:
		{
			resetCommand();
			m_state = Parsing;
			//no break here !
		}
		case Parsing:
		{
			int ci = m_parser.getCommand( itr, eoM, m_lineBuffer);
			if (ci >= unknown && ci < method)
			{
				if (ci == unknown) return unknown;
				m_command = (Command)ci;
				m_methodIdx = 0;
			}
			else
			{
				m_command = method;
				m_methodIdx = (unsigned int)ci - (unsigned int)method;
			}
			m_state = Selected;
			//no break here !
		}
		case Selected:
		{
			if (m_command == method)
			{
				if (!ProtocolParser::getLine( itr, eoM, m_argBuffer)) return unknown;
				m_state = ArgumentsParsed;
			}
			else
			{
				if (ProtocolParser::skipSpaces( itr, eoM))
				{
					if (!ProtocolParser::isEOLN( itr))
					{
						LOG_ERROR << "two many arguments for command";
						resetCommand();
						return unknown;
					}
					else if (ProtocolParser::consumeEOLN( itr, eoM))
					{
						CommandDispatcher::Command rt = m_command;
						resetCommand();
						return rt;
					}
					else
					{
						return unknown;
					}
				}
				else
				{
					return unknown;
				}
			}
			//no break here !
		}
		case ArgumentsParsed:
		{
			return m_command;
		}
	}
	LOG_ERROR << "illegal state (end of getCommand)";
	throw (IllegalState());
}

bool CommandDispatcher::commandHasIO() const
{
	return (m_command == method && m_instance && m_instance->m_mt[ m_methodIdx].hasIO);
}

CommandDispatcher::IOState CommandDispatcher::call( int& returnCode)
{
	LOG_DATA << "Dispatcher Call";

	switch (m_state)
	{
		case Null:
		case Init:
		case Parsing:
		case Selected:
		{
			LOG_ERROR << "illegal call in this state (not running)";
			throw (IllegalState());
		}

		case ArgumentsParsed:
		{
			LOG_DEBUG << "call of '" << m_instance->m_mt[ m_methodIdx].name << "'";
			m_argc = m_argBuffer.argc()+1;
			m_argv = m_argBuffer.argv( m_instance->m_mt[ m_methodIdx].name);
			m_state = Running;
			//no break here !
		}

		case Running:
			for (;;)
			{
				returnCode = m_instance->m_mt[ m_methodIdx].call( &m_context, m_argc, m_argv);
				if (returnCode != 0)
				{
					LOG_ERROR << "error " << returnCode << " calling '" << m_instance->m_mt[ m_methodIdx].name << "'";
					return Error;
				}
				Generator::State istate = (m_context.contentIterator)?m_context.contentIterator->state():protocol::Generator::Open;

				switch (istate)
				{
					case protocol::Generator::Open:
						if (m_context.output && (m_context.output->pos() > 0 || m_context.output->size() == 0))
						{
							if (commandHasIO())
							{
								return WriteOutput;
							}
							LOG_ERROR << "error printed in method '" << m_instance->m_mt[ m_methodIdx].name << "' declared to have no output";
							return Error;
						}
						else
						{
							LOG_DATA << "end of method call";
							return Close;
						}

					case protocol::Generator::EndOfMessage:
						if (commandHasIO())
						{
							return ReadInput;
						}
						m_context.contentIterator->protocolInput( 0, 0, true);
						continue;

					case protocol::Generator::Error:
						returnCode = m_context.contentIterator->getError();
						LOG_ERROR << "error " << returnCode << ") in generator calling '" << m_instance->m_mt[ m_methodIdx].name << "'";
						return Error;
				}
			}//for(;;)
	}
	return Close;
}

const char* CommandDispatcher::getCapabilities()
{
	m_lineBuffer.clear();
	unsigned int ii;
	if (m_instance && m_instance->m_mt)
	{
		for (ii=0; m_instance->m_mt[ii].call && m_instance->m_mt[ii].name; ii++)
		{
			if (ii>0)
			{
				m_lineBuffer.push_back( ',');
				m_lineBuffer.push_back( ' ');
			}
			m_lineBuffer.append( m_instance->m_mt[ii].name);
			if (m_instance->m_mt[ii].hasIO) m_lineBuffer.append( "[]");
		}
	}
	return m_lineBuffer.c_str();
}


