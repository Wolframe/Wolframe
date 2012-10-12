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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file mainConnectionHandler.cpp

#include "mainConnectionHandler.hpp"
#include "cmdbind/execCommandHandler.hpp"
#include "handlerConfig.hpp"
#include "logger-v1.hpp"
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::proc;

enum State
{
	State0
};

struct STM :public cmdbind::LineCommandHandlerSTMTemplate<CommandHandler>
{
	STM()
	{
		(*this)
			[State0]
				.cmd< &CommandHandler::doHello >( "HELLO")
				.cmd< &CommandHandler::doCmdQUIT >( "QUIT")
		;
	}
};
static STM stm;

CommandHandler::CommandHandler()
	:cmdbind::LineCommandHandlerTemplate<CommandHandler>( &stm )
{
	m_commands.push_back( Command( "RUN", "test"));
}

int CommandHandler::endRun( cmdbind::CommandHandler* ch, std::ostream& out)
{
	cmdbind::ExecCommandHandler* chnd = dynamic_cast<cmdbind::ExecCommandHandler*>( ch);
	int argc;
	const char** argv;
	const char* lastcmd = chnd->getCommand( argc, argv);
	m_statusCode = ch->statusCode();
	int rt = stateidx();

	if (lastcmd)
	{
		try
		{
			rt = runCommand( lastcmd, argc, argv, out);
		}
		catch (const std::exception& e)
		{
			LOG_ERROR << "exception in command execution: " << e.what();
			m_statusCode = -1;
		}
	}
	delete ch;
	return rt;
}


int CommandHandler::doCmdQUIT( int argc, const char**, std::ostream& out)
{
	if (argc != 0)
	{
		out << "BAD arguments" << endl();
		return stateidx();
	}
	else
	{
		out << "BYE" << endl();
		return -1;
	}
}

int CommandHandler::doHello( int argc, const char**, std::ostream& out)
{
	if (argc != 0)
	{
		out << "BAD arguments" << endl();
		return stateidx();
	}
	try
	{
		CommandHandler* ch = (CommandHandler*)new cmdbind::ExecCommandHandler( cmds(), commands());
		ch->setProcProvider( m_provider);
		delegateProcessing<&CommandHandler::endRun>( ch);
	}
	catch (const std::exception& e)
	{
		LOG_ERROR << "exception in command execution: " << e.what();
	}
	return stateidx();
}

void Connection::networkInput( const void* dt, std::size_t nofBytes)
{
	m_cmdhandler.putInput( dt, nofBytes);
}

void Connection::signalOccured( NetworkSignal)
{
	LOG_TRACE << "Got signal";
	m_terminated = true;
}

const net::NetworkOperation Connection::nextOperation()
{
	void* inpp;
	std::size_t inppsize;
	const void* outpp;
	std::size_t outppsize;
	if (m_terminated)
	{
		return net::CloseConnection();
	}
	switch(m_cmdhandler.nextOperation())
	{
		case cmdbind::CommandHandler::READ:
			m_cmdhandler.getInputBlock( inpp, inppsize);
			return net::ReadData( inpp, inppsize);

		case cmdbind::CommandHandler::WRITE:
			m_cmdhandler.getOutput( outpp, outppsize);
			return net::SendData( outpp, outppsize);

		case cmdbind::CommandHandler::CLOSE:
			return net::CloseConnection();
	}
	return net::CloseConnection();
}

Connection::Connection( const net::LocalEndpoint& local)
	:m_cmdhandler()
	,m_terminated(false)
{
	m_cmdhandler.setInputBuffer( m_input.ptr(), m_input.size());
	m_cmdhandler.setOutputBuffer( m_output.ptr(), m_output.size());
	LOG_TRACE << "Created connection handler for " << local.toString();
}

Connection::~Connection()
{
	LOG_TRACE << "Connection handler destroyed";
}

void Connection::setPeer( const net::RemoteEndpoint& remote)
{
	LOG_TRACE << "Peer set to " << remote.toString();
}


