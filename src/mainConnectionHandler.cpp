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
#include "cmdbind/discardInputCommandHandlerEscDLF.hpp"
#include "cmdbind/doctypeFilterCommandHandler.hpp"
#include "cmdbind/authCommandHandler.hpp"
#include "handlerConfig.hpp"
#include "logger-v1.hpp"
#include <stdexcept>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::proc;

enum State
{
	Unauthorized,
	Authorization,
	Authorized
};

struct STM :public cmdbind::LineCommandHandlerSTMTemplate<CommandHandler>
{
	STM()
	{
		(*this)
			[Unauthorized]
				.cmd< &CommandHandler::doAuth >( "AUTH")
				.cmd< &CommandHandler::doQuit >( "QUIT")
				.cmd< &CommandHandler::doCapabilities >( "CAPABILITIES")
			[Authorization]
				.cmd< &CommandHandler::doMech >( "MECH")
				.cmd< &CommandHandler::doQuit >( "QUIT")
				.cmd< &CommandHandler::doCapabilities >( "CAPABILITIES")
			[Authorized]
				.cmd< &CommandHandler::doRequest >( "REQUEST")
				.cmd< &CommandHandler::doInterface >( "INTERFACE")
				.cmd< &CommandHandler::doAuth >( "AUTH")
				.cmd< &CommandHandler::doQuit >( "QUIT")
				.cmd< &CommandHandler::doCapabilities >( "CAPABILITIES")
		;
	}
};
static STM stm;

CommandHandler::CommandHandler()
	:cmdbind::LineCommandHandlerTemplate<CommandHandler>( &stm ){}

static unsigned long getNumber( const char* aa)
{
	unsigned long result = 0;
	for (int ii=0; aa[ii] <= '9' && aa[0] >= '0'; ++ii)
	{
		unsigned long xx = result * 10 + aa[0] - '0';
		if (xx < result) throw std::runtime_error( "number out of range");
		result = xx;
	}
	return result;

}

int CommandHandler::doCapabilities( int argc, const char**, std::ostream& out)
{
	if (argc != 0)
	{
		out << "ERR unexpected arguments" << endl();
		return stateidx();
	}
	else
	{
		out << "OK " << boost::algorithm::join( cmds(), " ") << endl();
		return stateidx();
	}
}

int CommandHandler::doQuit( int argc, const char**, std::ostream& out)
{
	if (argc != 0)
	{
		out << "ERR unexpected arguments" << endl();
		return stateidx();
	}
	else
	{
		out << "BYE" << endl();
		return -1;
	}
}

int CommandHandler::doAuth( int argc, const char**, std::ostream& out)
{
	if (argc != 0)
	{
		out << "ERR AUTH no arguments expected" << endl();
		return stateidx();
	}
	else
	{
		out << "MECHS " << boost::algorithm::join( m_authMechanisms.list(), " ") << "NONE" << endl();
		return Authorization;
	}
}

int CommandHandler::endMech( cmdbind::CommandHandler* ch, std::ostream& out)
{
	cmdbind::AuthCommandHandler* chnd = dynamic_cast<cmdbind::AuthCommandHandler*>( ch);
	cmdbind::CommandHandlerR chr( ch);
	int authstatus = ch->statusCode();

	if (authstatus)
	{
		out << "ERR authorization " << authstatus << endl();
		return -1;
	}
	else
	{
		out << "OK authorization" << endl();
		m_authtickets.push_back( chnd->ticket());
		return Authorized;
	}
}

int CommandHandler::doMech( int argc, const char** argv, std::ostream& out)
{
	if (argc == 0)
	{
		out << "ERR argument (mechanism identifier) expected for MECH" << endl();
		return stateidx();
	}
	if (argc >= 2)
	{
		out << "ERR to many arguments for MECH" << endl();
		return stateidx();
	}
	if (boost::iequals( std::string(argv[0]), "NONE"))
	{
		out << "OK authorization" << endl();
		m_authtickets.push_back( "none");
		return Authorized;
	}
	cmdbind::AuthCommandHandler* authch = m_authMechanisms.get( argv[0]);
	if (authch)
	{
		out << "ERR no handler defined for authorization mechanism " << argv[0] << "'" << endl();
	}
	authch->setProcProvider( m_provider);
	delegateProcessing<&CommandHandler::endMech>( authch);
	return stateidx();
}

int CommandHandler::doInterface( int argc, const char** argv, std::ostream& out)
{
	int min_version = 0;
	int version = 0;
	if (argc == 1)
	{

		min_version = getNumber( argv[0]);
	}
	typedef std::map<std::string,std::string> UIForms;
	UIForms uiforms = m_provider->uiforms( m_authtickets.back(), min_version, version);
	UIForms::const_iterator fi = uiforms.begin(), fe = uiforms.end();
	out << "INTERFACE " << version << endl();
	for (; fi != fe; ++fi)
	{
		out << "UIFORM " << fi->first << endl();
		out << protocol::escapeStringDLF( fi->second);
		out << endl() << "." << endl();
	}
	out << "OK" << endl();
	return stateidx();
}

int CommandHandler::endRequest( cmdbind::CommandHandler* chnd, std::ostream& out)
{
	cmdbind::CommandHandlerR chr( chnd);
	int rt = stateidx();
	int stc = statusCode();
	if (!stc)
	{
		out << "OK REQUEST " << m_doctype << endl();
	}
	else
	{
		out << "ERR REQUEST " << stc << endl();
		LOG_ERROR << "error in execution of REQUEST " << m_doctype << ". status code " << stc;
	}
	return rt;
}

static bool redirectConsumedInput( cmdbind::DoctypeFilterCommandHandler* fromh, cmdbind::CommandHandler* toh, std::ostream& out)
{
	void* buf;
	std::size_t bufsize;
	const void* toh_output;
	std::size_t toh_outputsize;
	int cmdStatus = 0;
	fromh->getInputBuffer( buf, bufsize);
	toh->setInputBuffer( buf, bufsize);
	toh->putInput( buf, bufsize);

	for (;;) switch (toh->nextOperation())
	{
		case cmdbind::CommandHandler::READ:
			return true;
		case cmdbind::CommandHandler::WRITE:
			toh->getOutput( toh_output, toh_outputsize);
			out << std::string( (const char*)toh_output, toh_outputsize);
			continue;
		case cmdbind::CommandHandler::CLOSE:
			cmdStatus = toh->statusCode();
			if (cmdStatus) out << "ERR " << cmdStatus << CommandHandler::endl();
			return false;
	}
}

int CommandHandler::endErrDocumentType( cmdbind::CommandHandler* ch, std::ostream& out)
{
	cmdbind::CommandHandlerR chr( ch);
	out << "ERR document type" << LineCommandHandler::endl();
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
				delegateProcessing<&CommandHandler::endErrDocumentType>( execch);
			}
		}
		else
		{
			m_doctype = doctype;
			execch->passParameters( doctype, 0, 0);
			execch->setProcProvider( m_provider);
			if (redirectConsumedInput( chnd, execch, out))
			{
				out << "ANSWER" << endl();
				delegateProcessing<&CommandHandler::endRequest>( execch);
			}
		}
		return stateidx();
	}
}

int CommandHandler::doRequest( int argc, const char** argv, std::ostream& out)
{
	m_doctype.clear();
	if (argc)
	{
		if (argc > 1)
		{
			out << "ERR to many arguments" << endl();
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
				out << "ANSWER" << endl();
				delegateProcessing<&CommandHandler::endRequest>( ch);
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


