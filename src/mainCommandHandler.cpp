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
///\file mainCommandHandler.cpp
#include "mainCommandHandler.hpp"
#include "interfaceCommandHandler.hpp"
#include "cmdbind/discardInputCommandHandlerEscDLF.hpp"
#include "cmdbind/authCommandHandler.hpp"
#include "processor/execContext.hpp"
#include "interfaceCommandHandler.hpp"
#include "handlerConfig.hpp"
#include "logger-v1.hpp"
#include <stdexcept>
#include <algorithm>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::proc;

struct MainSTM :public cmdbind::LineCommandHandlerSTMTemplate<MainCommandHandler>
{
	enum State
	{
		Unauthenticated,
		Authentication,
		Authenticated
	};

	MainSTM()
	{
		(*this)
			[Unauthenticated]
				.cmd< &MainCommandHandler::doAuth >( "AUTH")
				.cmd< &MainCommandHandler::doQuit >( "QUIT")
				.cmd< &MainCommandHandler::doCapabilities >( "CAPABILITIES")
			[Authentication]
				.cmd< &MainCommandHandler::doMech >( "MECH")
				.cmd< &MainCommandHandler::doQuit >( "QUIT")
				.cmd< &MainCommandHandler::doCapabilities >( "CAPABILITIES")
			[Authenticated]
				.cmd< &MainCommandHandler::doRequest >( "REQUEST")
				.cmd< &MainCommandHandler::doInterface >( "INTERFACE")
				.cmd< &MainCommandHandler::doAuth >( "AUTH")
				.cmd< &MainCommandHandler::doQuit >( "QUIT")
				.cmd< &MainCommandHandler::doCapabilities >( "CAPABILITIES")
		;
	}
};
static MainSTM mainstm;

MainCommandHandler::MainCommandHandler()
	:cmdbind::LineCommandHandlerTemplate<MainCommandHandler>( &mainstm ){}

int MainCommandHandler::doCapabilities( int argc, const char**, std::ostream& out)
{
	if (argc != 0)
	{
		out << "ERR unexpected arguments" << endl();
		return stateidx();
	}
	else
	{
		out << "OK" << boost::algorithm::join( cmds(), " ") << endl();
		return stateidx();
	}
}

int MainCommandHandler::doQuit( int argc, const char**, std::ostream& out)
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

int MainCommandHandler::doAuth( int argc, const char**, std::ostream& out)
{
	if (!m_authenticator.get())
	{
		m_authenticator.reset( execContext()->authenticator());
		if (!m_authenticator.get())
		{
			out << "ERR AUTH denied" << endl();
			return stateidx();
		}
	}
	if (argc != 0)
	{
		out << "ERR AUTH no arguments expected" << endl();
		return stateidx();
	}
	else
	{
		out << "MECHS " << "NONE" /*[+] boost::algorithm::join( m_authenticator->mechs(), " ")*/ << endl();
		return MainSTM::Authentication;
	}
}

int MainCommandHandler::endMech( cmdbind::CommandHandler* ch, std::ostream& out)
{
	//[+] cmdbind::AuthCommandHandler* chnd = dynamic_cast<cmdbind::AuthCommandHandler*>( ch);
	cmdbind::CommandHandlerR chr( ch);
	const char* error = ch->lastError();
	if (error)
	{
		out << "ERR authentication " << error << endl();
		return -1;
	}
	else
	{
		out << "OK authorization" << endl();
		//[+] m_execContext->setUser( chnd->user());
		return MainSTM::Authenticated;
	}
}

int MainCommandHandler::doMech( int argc, const char** /*argv*/, std::ostream& out)
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
	//[+] if (!m_authenticator->chooseMech( argv[0]))
	//[+] {
	//[+] 	out << "ERR denied" << endl();
	//[+] 	out << "MECHS " << boost::algorithm::join( m_authenticator->mechs(), " ") << endl();
	//[+] 	return MainSTM::Authentication;
	//[+] }
	cmdbind::AuthCommandHandler* authch = new cmdbind::AuthCommandHandler( m_authenticator);
	authch->setExecContext( execContext());
	delegateProcessing<&MainCommandHandler::endMech>( authch);
	return stateidx();
}

int MainCommandHandler::endInterface( cmdbind::CommandHandler* ch, std::ostream&)
{
	delete ch;
	int rt = stateidx();
	return rt;
}

int MainCommandHandler::doInterface( int argc, const char**, std::ostream& out)
{
	if (argc != 0)
	{
		out << "ERR INTERFACE unexpected arguments" << endl();
		return stateidx();
	}
	cmdbind::CommandHandler* delegate_ch = (cmdbind::CommandHandler*)new InterfaceCommandHandler();
	out << "OK INTERFACE enter commands" << endl();
	delegateProcessing<&MainCommandHandler::endInterface>( delegate_ch);
	return stateidx();
}

static bool IsCntrl( char ch) {return ch>0 && ch <=32;}

int MainCommandHandler::endRequest( cmdbind::CommandHandler* chnd, std::ostream& out)
{
	cmdbind::CommandHandlerR chr( chnd);
	int rt = stateidx();
	const char* error = chnd->lastError();
	if (error)
	{
		std::string errstr( error?error:"unspecified error");
		std::replace_if( errstr.begin(), errstr.end(), IsCntrl, ' ');
		out << "ERR REQUEST " << errstr << endl();
		LOG_ERROR << "error in execution of REQUEST " << m_command << ":" << errstr;
	}
	else
	{
		out << "OK REQUEST " << m_command << endl();
	}
	return rt;
}

bool MainCommandHandler::redirectConsumedInput( cmdbind::DoctypeFilterCommandHandler* fromh, cmdbind::CommandHandler* toh, std::ostream& out)
{
	void* buf;
	std::size_t bufsize;
	fromh->getInputBuffer( buf, bufsize);
	return Parent::redirectInput( buf, bufsize, toh, out);
}

int MainCommandHandler::endErrDocumentType( cmdbind::CommandHandler* ch, std::ostream& out)
{
	cmdbind::CommandHandlerR chr( ch);
	const char* err = ch->lastError();
	out << "ERR " << (err?err:"document type")<< LineCommandHandler::endl();
	return stateidx();
}

int MainCommandHandler::endDoctypeDetection( cmdbind::CommandHandler* ch, std::ostream& out)
{
	cmdbind::DoctypeFilterCommandHandler* chnd = dynamic_cast<cmdbind::DoctypeFilterCommandHandler*>( ch);
	cmdbind::CommandHandlerR chr( ch);

	types::DoctypeInfoR info = chnd->info();
	
	const char* error = ch->lastError();
	if (!info.get())
	{
		error = "unknown document format (format detection failed)";
	}
	if (error)
	{
		std::ostringstream msg;
		msg << "failed to retrieve document type (" << error << ")";
		cmdbind::CommandHandler* delegate_ch = (cmdbind::CommandHandler*)new cmdbind::DiscardInputCommandHandlerEscDLF( msg.str());
		if (m_commandtag.empty())
		{
			out << "ANSWER" << endl();
		}
		else
		{
			out << "ANSWER " << '&' << m_commandtag << endl();
		}
		if (redirectConsumedInput( chnd, delegate_ch, out))
		{
			delegateProcessing<&MainCommandHandler::endErrDocumentType>( delegate_ch);
		}
		else
		{
			out << "ERR doctype detection " << error << endl();
			delete delegate_ch;
		}
		return stateidx();
	}
	LOG_DEBUG << "Got document type '" << info->doctype() << "' format '" << info->docformat() << "' command prefix '" << m_command << "'";
	m_command.append(info->doctype());

	cmdbind::CommandHandler* execch = execContext()->provider()->cmdhandler( m_command, info->docformat());
	if (!execch)
	{
		std::ostringstream msg;
		if (m_command.empty())
		{
			msg << "ERR got no document type and no command defined (empty request ?)";
		}
		else
		{
			msg << "no command handler for '" << m_command << "'";
		}
		execch = (cmdbind::CommandHandler*)new cmdbind::DiscardInputCommandHandlerEscDLF( msg.str());
		execch->setExecContext( execContext());
		if (m_commandtag.empty())
		{
			out << "ANSWER" << endl();
		}
		else
		{
			out << "ANSWER " << '&' << m_commandtag << endl();
		}
		if (redirectConsumedInput( chnd, execch, out))
		{
			delegateProcessing<&MainCommandHandler::endErrDocumentType>( execch);
		}
		else
		{
			out << "ERR " << msg.str() << endl();
			delete execch;
		}
		return stateidx();
	}
	else
	{
		const char* docformatptr = info->docformat().c_str();

		execch->setExecContext( execContext());
		execch->passParameters( m_command, 1, &docformatptr);
		if (m_commandtag.empty())
		{
			out << "ANSWER" << endl();
		}
		else
		{
			out << "ANSWER " << '&' << m_commandtag << endl();
		}
		if (redirectConsumedInput( chnd, execch, out))
		{
			delegateProcessing<&MainCommandHandler::endRequest>( execch);
		}
		else
		{
			if (execch->lastError())
			{
				std::string errstr( execch->lastError());
				std::replace_if( errstr.begin(), errstr.end(), IsCntrl, ' ');
				out << "ERR REQUEST " << m_command << " " << errstr << endl();
			}
			else
			{
				out << "OK REQUEST " << m_command << endl();
			}
			delete execch;
		}
		return stateidx();
	}
}

int MainCommandHandler::doRequest( int argc, const char** argv, std::ostream& out)
{
	m_command.clear();
	m_commandtag.clear();
	if (argc)
	{
		bool has_commandtag = false;
		bool has_command = false;
		for (int ii=0; ii<argc; ++ii)
		{
			if (argv[ii][0] == '&')
			{
				if (has_commandtag)
				{
					out << "ERR more than one command tag" << endl();
					return stateidx();
				}
				has_commandtag = true;
				m_commandtag.append( argv[ii]+1);
			}
			else
			{
				if (has_command)
				{
					out << "ERR to many arguments" << endl();
					return stateidx();
				}
				has_command = true;
				m_command.append( argv[ii]);
			}
		}
	}
	CommandHandler* ch = (CommandHandler*)new cmdbind::DoctypeFilterCommandHandler();
	ch->setExecContext( execContext());

	delegateProcessing<&MainCommandHandler::endDoctypeDetection>( ch);
	return stateidx();
}

