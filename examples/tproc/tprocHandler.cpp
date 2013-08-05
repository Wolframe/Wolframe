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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file tprocHandler.cpp

#include "tprocHandler.hpp"
#include "cmdbind/execCommandHandler.hpp"
#include "logger/logger-v1.hpp"
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::tproc;

enum State
{
	State1,
	State2,
	State3
};

struct STM :public cmdbind::LineCommandHandlerSTMTemplate<CommandHandler>
{
	STM()
	{
		(*this)
			[State1]
				.cmd< &CommandHandler::doHello >( "HELLO")
				.cmd< &CommandHandler::doCmd1A >( "CMD1A")
				.cmd< &CommandHandler::doCmd1B >( "CMD1B")
				.cmd< &CommandHandler::doCmd1C >( "CMD1B")
				.cmd< &CommandHandler::doCmdQUIT >( "QUIT")
			[State2]
				.cmd< &CommandHandler::doCmd2A >( "CMD2A")
				.cmd< &CommandHandler::doCmd2B >( "CMD2B")
			[State3]
				.cmd< &CommandHandler::doCmd3A >( "CMD3A")
				.cmd< &CommandHandler::doCmdQUIT >( "QUIT")
		;
	}
};
static STM stm;


int CommandHandler::doCmd1A( int argc, const char** argv, std::ostream& out)
{
	out << "OK CMD1A";
	if (argc == 0)
	{
		out << " ?" << endl();
		return (int)stateidx();
	}
	else
	{
		for (int ii=0; ii<argc; ii++)
		{
			out << " '" << argv[ii] << "'";
		}
		out << endl();
		return State2;
	}
}

int CommandHandler::doCmd1B( int argc, const char** argv, std::ostream& out)
{
	out << "OK CMD1B";
	if (argc == 0)
	{
		out << " ?" << endl();
		return (int)stateidx();
	}
	else
	{
		for (int ii=0; ii<argc; ii++)
		{
			out << " '" << argv[ii] << "'";
		}
		out << endl();
		return State2;
	}
}

int CommandHandler::doCmd1C( int argc, const char** argv, std::ostream& out)
{
	out << "OK CMD1C";
	if (argc == 0)
	{
		out << " ?" << endl();
		return (int)stateidx();
	}
	else
	{
		for (int ii=0; ii<argc; ii++)
		{
			out << " '" << argv[ii] << "'";
		}
		out << endl();
		return State2;
	}
}

int CommandHandler::doCmd2A( int argc, const char** argv, std::ostream& out)
{
	out << "OK CMD2A";
	if (argc == 0)
	{
		out << " ?" << endl();
		return (int)stateidx();
	}
	else
	{
		for (int ii=0; ii<argc; ii++)
		{
			out << " '" << argv[ii] << "'";
		}
		out << endl();
		return State3;
	}
}

int CommandHandler::doCmd2B( int argc, const char** argv, std::ostream& out)
{
	out << "OK CMD2B";
	if (argc == 0)
	{
		out << " ?" << endl();
		return (int)stateidx();
	}
	else
	{
		for (int ii=0; ii<argc; ii++)
		{
			out << " '" << argv[ii] << "'";
		}
		out << endl();
		return State3;
	}
}

int CommandHandler::endRun( cmdbind::CommandHandler* ch, std::ostream& out)
{
	cmdbind::ExecCommandHandler* chnd = dynamic_cast<cmdbind::ExecCommandHandler*>( ch);
	int argc;
	const char** argv;
	const char* lastcmd = chnd->getCommand( argc, argv);
	if (ch->lastError()) m_lastError = ch->lastError();
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
			m_lastError = "exception";
		}
	}
	delete ch;
	return rt;
}

int CommandHandler::doCmd3A( int argc, const char** argv, std::ostream& out)
{
	out << "OK CMD3A";
	if (argc == 0)
	{
		out << " ?" << endl();
		return (int)stateidx();
	}
	else
	{
		for (int ii=0; ii<argc; ii++)
		{
			out << " '" << argv[ii] << "'";
		}
		out << endl();
		try
		{
			CommandHandler* ch = (CommandHandler*)new cmdbind::ExecCommandHandler( cmds(), m_config->commands());
			ch->setProcProvider( m_provider);
			delegateProcessing<&CommandHandler::endRun>( ch);
		}
		catch (const std::exception& e)
		{
			LOG_ERROR << "exception in command execution: " << e.what();
		}
		return stateidx();
	}
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
		CommandHandler* ch = (CommandHandler*)new cmdbind::ExecCommandHandler( cmds(), m_config->commands());
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

Connection::Connection( const net::LocalEndpoint& local, const Configuration* config)
	:m_config(config)
	,m_cmdhandler( &stm, config)
	,m_input(config->input_bufsize())
	,m_output(config->output_bufsize())
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

net::ConnectionHandler* ServerHandler::ServerHandlerImpl::newConnection( const net::LocalEndpoint& local)
{
	return new tproc::Connection( local, m_config->m_appConfig);
}

ServerHandler::ServerHandler( const HandlerConfiguration* cfg,
			      const module::ModulesDirectory* /*modules*/)
	: m_impl( new ServerHandlerImpl( cfg)) {}

ServerHandler::~ServerHandler()
{
	delete m_impl;
}

net::ConnectionHandler* ServerHandler::newConnection( const net::LocalEndpoint& local)
{
	return m_impl->newConnection( local);
}


