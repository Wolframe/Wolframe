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
#include "cmdbind/discardInputCommandHandlerEscDLF.hpp"
#include "cmdbind/doctypeFilterCommandHandler.hpp"
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
				.cmd< &CommandHandler::doRun >( "RUN")
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

int CommandHandler::endHello( cmdbind::CommandHandler* ch, std::ostream& out)
{
	cmdbind::ExecCommandHandler* chnd = dynamic_cast<cmdbind::ExecCommandHandler*>( ch);
	cmdbind::CommandHandlerR chr( ch);
	int argc;
	const char** argv;
	const char* lastcmd = chnd->getCommand( argc, argv);
	m_statusCode = ch->statusCode();
	int rt = stateidx();

	if (lastcmd) rt = runCommand( lastcmd, argc, argv, out);
	return rt;
}

int CommandHandler::doHello( int argc, const char**, std::ostream& out)
{
	if (argc != 0)
	{
		out << "BAD arguments" << endl();
		return stateidx();
	}
	CommandHandler* ch = (CommandHandler*)new cmdbind::ExecCommandHandler( cmds(), m_commands);
	ch->setProcProvider( m_provider);
	delegateProcessing<&CommandHandler::endHello>( ch);
	return stateidx();
}


int CommandHandler::endRun( cmdbind::CommandHandler* chnd, std::ostream& out)
{
	cmdbind::CommandHandlerR chr( chnd);
	int rt = stateidx();
	int stc = statusCode();
	if (!stc)
	{
		out << "OK RUN " << m_doctype << endl();
	}
	else
	{
		out << "ERR " << stc << endl();
		LOG_ERROR << "error in execution of RUN " << m_doctype << ". status code " << stc;
	}
	return rt;
}

static bool redirectConsumedInput( cmdbind::DoctypeFilterCommandHandler* fromh, cmdbind::CommandHandler* toh, std::ostream& out)
{
	void* buf;
	std::size_t bufsize;
	fromh->getInputBuffer( buf, bufsize);
	toh->setInputBuffer( buf, bufsize);
	toh->putInput( buf, bufsize);

	for (;;) switch (toh->nextOperation())
	{
		case cmdbind::CommandHandler::READ:
			return true;
		case cmdbind::CommandHandler::WRITE:
		{
			const void* toh_output;
			std::size_t toh_outputsize;
			toh->getOutput( toh_output, toh_outputsize);
			out << std::string( (const char*)toh_output, toh_outputsize);
			continue;
		}
		case cmdbind::CommandHandler::CLOSE:
			return false;
	}
}

int CommandHandler::endErrDocumentType( cmdbind::CommandHandler* ch, std::ostream& out)
{
	cmdbind::CommandHandlerR chr( ch);
	out << "ERR cannot process this document type" << endl();
	return stateidx();
}

int CommandHandler::endDoctypeDetection( cmdbind::CommandHandler* ch, std::ostream& out)
{
	cmdbind::DoctypeFilterCommandHandler* chnd = dynamic_cast<cmdbind::DoctypeFilterCommandHandler*>( ch);
	cmdbind::CommandHandlerR chr( ch);
	std::string doctype = chnd->doctypeid();

	int stc = ch->statusCode();
	if (stc)
	{
		std::ostringstream msg;
		msg << "unable to execute - failed to retrieve document type (error code" << stc << ")";
		cmdbind::CommandHandler* delegate_ch = (cmdbind::CommandHandler*)new cmdbind::DiscardInputCommandHandlerEscDLF( msg.str());
		delegateProcessing<&CommandHandler::endErrDocumentType>( delegate_ch);
		return stateidx();
	}
	if (doctype.empty())
	{
		cmdbind::CommandHandler* delegate_ch = (cmdbind::CommandHandler*)new cmdbind::DiscardInputCommandHandlerEscDLF( "unable to execute - no document type defined");
		delegateProcessing<&CommandHandler::endErrDocumentType>( delegate_ch);
		return stateidx();
	}
	else
	{
		cmdbind::CommandHandler* execch = m_provider->cmdhandler( doctype);
		if (!execch)
		{
			std::ostringstream msg;
			msg << "unable to execute - command handler for document type '" << doctype << "' is not defined";
			execch = (cmdbind::CommandHandler*)new cmdbind::DiscardInputCommandHandlerEscDLF( msg.str());
			if (redirectConsumedInput( chnd, execch, out))
			{
				delegateProcessing<&CommandHandler::endErrDocumentType>( ch);
			}
		}
		else
		{
			m_doctype = doctype;
			execch->passParameters( doctype, 0, 0);
			execch->setProcProvider( m_provider);
			if (redirectConsumedInput( chnd, execch, out))
			{
				delegateProcessing<&CommandHandler::endRun>( execch);
			}
		}
		return stateidx();
	}
}

int CommandHandler::doRun( int argc, const char** argv, std::ostream& out)
{
	m_doctype.clear();
	if (argc)
	{
		if (argc > 1)
		{
			out << "BAD to many arguments" << endl();
			return stateidx();
		}
		else
		{
			cmdbind::CommandHandler* ch = m_provider->cmdhandler( argv[0]);
			if (!ch)
			{
				std::ostringstream msg;
				msg << "unable to execute - command handler for document type '" << argv[0] << "' is not defined";
				ch = (cmdbind::CommandHandler*)new cmdbind::DiscardInputCommandHandlerEscDLF( msg.str());
				delegateProcessing<&CommandHandler::endErrDocumentType>( ch);
			}
			else
			{
				m_doctype = argv[0];
				ch->passParameters( m_doctype, 0, 0);
				ch->setProcProvider( m_provider);
				delegateProcessing<&CommandHandler::endRun>( ch);
			}
			return stateidx();
		}
	}
	else
	{
		CommandHandler* ch = (CommandHandler*)new cmdbind::DoctypeFilterCommandHandler();
		delegateProcessing<&CommandHandler::endDoctypeDetection>( ch);
		return stateidx();
	}
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


