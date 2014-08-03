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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file tprocHandler.cpp

#include "tprocHandler.hpp"
#include "execProtocolHandler.hpp"
#include "logger-v1.hpp"
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::tproc;

enum State
{
	State1,
	State2,
	State3
};

struct STM :public cmdbind::LineProtocolHandlerSTMTemplate<ProtocolHandler>
{
	STM()
	{
		(*this)
			[State1]
				.cmd< &ProtocolHandler::doHello >( "HELLO")
				.cmd< &ProtocolHandler::doCmd1A >( "CMD1A")
				.cmd< &ProtocolHandler::doCmd1B >( "CMD1B")
				.cmd< &ProtocolHandler::doCmd1C >( "CMD1B")
				.cmd< &ProtocolHandler::doCmdQUIT >( "QUIT")
			[State2]
				.cmd< &ProtocolHandler::doCmd2A >( "CMD2A")
				.cmd< &ProtocolHandler::doCmd2B >( "CMD2B")
			[State3]
				.cmd< &ProtocolHandler::doCmd3A >( "CMD3A")
				.cmd< &ProtocolHandler::doCmdQUIT >( "QUIT")
		;
	}
};
static STM stm;


int ProtocolHandler::doCmd1A( int argc, const char** argv, std::ostream& out)
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

int ProtocolHandler::doCmd1B( int argc, const char** argv, std::ostream& out)
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

int ProtocolHandler::doCmd1C( int argc, const char** argv, std::ostream& out)
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

int ProtocolHandler::doCmd2A( int argc, const char** argv, std::ostream& out)
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

int ProtocolHandler::doCmd2B( int argc, const char** argv, std::ostream& out)
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

int ProtocolHandler::endRun( cmdbind::ProtocolHandler* ch, std::ostream& out)
{
	cmdbind::ExecProtocolHandler* chnd = dynamic_cast<cmdbind::ExecProtocolHandler*>( ch);
	int argc;
	const char** argv;
	const char* lastcmd = chnd->getCommand( argc, argv);
	if (ch->lastError()) setLastError( ch->lastError());
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
			setLastError( std::string("exception in command execution: ") + e.what());
		}
	}
	delete ch;
	return rt;
}

int ProtocolHandler::doCmd3A( int argc, const char** argv, std::ostream& out)
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
			ProtocolHandler* ch = (ProtocolHandler*)new cmdbind::ExecProtocolHandler( cmds(), m_config->commands());
			ch->setExecContext( execContext());
			delegateProcessing<&ProtocolHandler::endRun>( ch);
		}
		catch (const std::exception& e)
		{
			LOG_ERROR << "exception in command execution: " << e.what();
		}
		return stateidx();
	}
}

int ProtocolHandler::doCmdQUIT( int argc, const char**, std::ostream& out)
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

int ProtocolHandler::doHello( int argc, const char**, std::ostream& out)
{
	if (argc != 0)
	{
		out << "BAD arguments" << endl();
		return stateidx();
	}
	try
	{
		ProtocolHandler* ch = (ProtocolHandler*)new cmdbind::ExecProtocolHandler( cmds(), m_config->commands());
		ch->setExecContext( execContext());
		delegateProcessing<&ProtocolHandler::endRun>( ch);
	}
	catch (const std::exception& e)
	{
		LOG_ERROR << "exception in command execution: " << e.what();
	}
	return stateidx();
}

void Connection::networkInput( const void* dt, std::size_t nofBytes)
{
	m_protocolhandler.putInput( dt, nofBytes);
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
	switch(m_protocolhandler.nextOperation())
	{
		case cmdbind::ProtocolHandler::READ:
			m_protocolhandler.getInputBlock( inpp, inppsize);
			return net::ReadData( inpp, inppsize);

		case cmdbind::ProtocolHandler::WRITE:
			m_protocolhandler.getOutput( outpp, outppsize);
			return net::SendData( outpp, outppsize);

		case cmdbind::ProtocolHandler::CLOSE:
			return net::CloseConnection();
	}
	return net::CloseConnection();
}

Connection::Connection( const net::LocalEndpointR& local, const Configuration* config)
	:m_protocolhandler( &stm, config)
	,m_input(config->input_bufsize())
	,m_output(config->output_bufsize())
	,m_terminated(false)
{
	m_protocolhandler.setInputBuffer( m_input.ptr(), m_input.size());
	m_protocolhandler.setOutputBuffer( m_output.ptr(), m_output.size());
	LOG_TRACE << "Created connection handler for " << local->toString();
}

Connection::~Connection()
{
	LOG_TRACE << "Connection handler destroyed";
}

void Connection::setPeer( const net::RemoteEndpointR& remote)
{
	LOG_TRACE << "Peer set to " << remote->toString();
}

net::ConnectionHandler* ServerHandler::ServerHandlerImpl::newConnection( const net::LocalEndpointR& local)
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

net::ConnectionHandler* ServerHandler::newConnection( const net::LocalEndpointR& local)
{
	return m_impl->newConnection( local);
}


