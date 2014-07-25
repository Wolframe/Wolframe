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
///\file StandardProtocolHandler.cpp
#include "standardProtocolHandler.hpp"
#include "interfaceProtocolHandler.hpp"
#include "authProtocolHandler.hpp"
#include "passwordChangeProtocolHandler.hpp"
#include "escDlfProtocolHandler.hpp"
#include "processor/execContext.hpp"
#include "interfaceProtocolHandler.hpp"
#include "handlerConfig.hpp"
#include "logger-v1.hpp"
#include <stdexcept>
#include <algorithm>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::cmdbind;

struct MainSTM :public cmdbind::LineProtocolHandlerSTMTemplate<StandardProtocolHandler>
{
	enum State
	{
		Unauthenticated,
		Authentication,
		Authenticated,
		Authenticated_passwd
	};

	MainSTM()
	{
		(*this)
			[Unauthenticated]
				.cmd< &StandardProtocolHandler::doAuth >( "AUTH")
				.cmd< &StandardProtocolHandler::doQuit >( "QUIT")
				.cmd< &StandardProtocolHandler::doCapabilities >( "CAPABILITIES")
			[Authentication]
				.cmd< &StandardProtocolHandler::doMech >( "MECH")
				.cmd< &StandardProtocolHandler::doQuit >( "QUIT")
				.cmd< &StandardProtocolHandler::doCapabilities >( "CAPABILITIES")
			[Authenticated]
				.cmd< &StandardProtocolHandler::doRequest >( "REQUEST")
				.cmd< &StandardProtocolHandler::doInterface >( "INTERFACE")
				.cmd< &StandardProtocolHandler::doAuth >( "AUTH")
				.cmd< &StandardProtocolHandler::doQuit >( "QUIT")
				.cmd< &StandardProtocolHandler::doCapabilities >( "CAPABILITIES")
			[Authenticated_passwd]
				.cmd< &StandardProtocolHandler::doRequest >( "REQUEST")
				.cmd< &StandardProtocolHandler::doInterface >( "INTERFACE")
				.cmd< &StandardProtocolHandler::doAuth >( "AUTH")
				.cmd< &StandardProtocolHandler::doPasswordChange >( "PASSWD")
				.cmd< &StandardProtocolHandler::doQuit >( "QUIT")
				.cmd< &StandardProtocolHandler::doCapabilities >( "CAPABILITIES")
		;
	}
};
static MainSTM mainstm;

StandardProtocolHandler::StandardProtocolHandler()
	:cmdbind::LineProtocolHandlerTemplate<StandardProtocolHandler>(&mainstm){}

void StandardProtocolHandler::setPeer( const net::RemoteEndpointR& remote)
{
	m_remoteEndpoint = remote;
}

void StandardProtocolHandler::setLocalEndPoint( const net::LocalEndpointR& local)
{
	m_localEndpoint = local;
}

int StandardProtocolHandler::doCapabilities( int argc, const char**, std::ostream& out)
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

int StandardProtocolHandler::doQuit( int argc, const char**, std::ostream& out)
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

int StandardProtocolHandler::doAuth( int argc, const char**, std::ostream& out)
{
	if (!m_localEndpoint.get())
	{
		throw std::logic_error( "no local endpoint set");
	}
	if (!m_remoteEndpoint.get())
	{
		throw std::logic_error( "no remote endpoint set");
	}
	execContext()->setConnectionData( m_remoteEndpoint, m_localEndpoint);

	if (!m_authenticator.get())
	{
		if (!m_remoteEndpoint.get())
		{
			throw std::logic_error("no remote endpoint set, cannot authenticate");
		}
		m_authenticator.reset( execContext()->authenticator( *m_remoteEndpoint ));

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
		std::string mechlist = boost::algorithm::join( m_authenticator->mechs(), " ");
		if (mechlist.empty())
		{
			out << "MECHS NONE" << endl();
		}
		else
		{
			out << "MECHS NONE " << mechlist << endl();
		}
		return MainSTM::Authentication;
	}
}

int StandardProtocolHandler::endMech( cmdbind::ProtocolHandler* ch, std::ostream& out)
{
	cmdbind::ProtocolHandlerR chr( ch);
	const char* error = ch->lastError();
	if (error)
	{
		out << "ERR authentication failed: " << error << endl();
		return MainSTM::Unauthenticated;
	}
	else
	{
		AAAA::User* usr = m_authenticator->user();
		if (usr)
		{
			out << "OK authenticated" << endl();
			execContext()->setUser( usr);

			if (execContext()->hasCapability( "PasswordChange")
			&& execContext()->checkAuthorization( proc::ExecContext::PASSWD))
			{
				return MainSTM::Authenticated_passwd;
			}
			else
			{
				return MainSTM::Authenticated;
			}
		}
		else
		{
			out << "ERR authentication refused" << endl();
			return MainSTM::Unauthenticated;
		}
	}
}

int StandardProtocolHandler::doMech( int argc, const char** argv, std::ostream& out)
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
	if (0==std::strcmp(argv[0],"NONE"))
	{
		out << "OK no authentication" << endl();
		return MainSTM::Authenticated;
	}
	else
	{
		if (!m_authenticator->setMech( argv[0] ))
		{
			out << "ERR denied" << endl();
			std::string mechlist = boost::algorithm::join( m_authenticator->mechs(), " ");
			if (mechlist.empty())
			{
				out << "MECHS NONE" << endl();
			}
			else
			{
				out << "MECHS NONE " << mechlist << endl();
			}
			return MainSTM::Authentication;
		}
		else
		{
			out << "OK go authentication with mech " << argv[0] << endl();
			cmdbind::AuthProtocolHandler* authch = new cmdbind::AuthProtocolHandler( m_authenticator);
			authch->setExecContext( execContext());
			delegateProcessing<&StandardProtocolHandler::endMech>( authch);
		}
	}
	return stateidx();
}

int StandardProtocolHandler::endPasswordChange( cmdbind::ProtocolHandler* ch, std::ostream& out)
{
	cmdbind::ProtocolHandlerR chr( ch);
	const char* error = ch->lastError();
	if (error)
	{
		out << "ERR password change failed: " << error << endl();
	}
	else
	{
		if (m_passwordChanger->status() == AAAA::PasswordChanger::PASSWORD_EXCHANGED)
		{
			out << "OK password changed" << endl();
		}
		else
		{
			out << "ERR password change failed (unknown error)" << endl();
		}
	}
	m_passwordChanger.reset();
	return stateidx();
}

int StandardProtocolHandler::doPasswordChange( int argc, const char**, std::ostream& out)
{
	if (argc >= 1)
	{
		out << "ERR to many arguments for PASSWD" << endl();
		return stateidx();
	}
	if (!m_remoteEndpoint.get())
	{
		throw std::logic_error("no remote endpoint set, cannot change password");
	}
	m_passwordChanger.reset( execContext()->passwordChanger( *m_remoteEndpoint ));

	if (!m_passwordChanger.get())
	{
		out << "ERR PASSWD denied" << endl();
		return stateidx();
	}
	out << "OK start password change" << endl();

	cmdbind::PasswordChangeProtocolHandler* pwdch = new cmdbind::PasswordChangeProtocolHandler( m_passwordChanger);
	pwdch->setExecContext( execContext());
	delegateProcessing<&StandardProtocolHandler::endPasswordChange>( pwdch);
	return stateidx();
}

int StandardProtocolHandler::endInterface( cmdbind::ProtocolHandler* ch, std::ostream&)
{
	delete ch;
	int rt = stateidx();
	return rt;
}

int StandardProtocolHandler::doInterface( int argc, const char**, std::ostream& out)
{
	if (argc != 0)
	{
		out << "ERR INTERFACE unexpected arguments" << endl();
		return stateidx();
	}
	cmdbind::ProtocolHandler* delegate_ch = (cmdbind::ProtocolHandler*)new InterfaceProtocolHandler();
	out << "OK INTERFACE enter commands" << endl();
	delegateProcessing<&StandardProtocolHandler::endInterface>( delegate_ch);
	return stateidx();
}

static bool IsCntrl( char ch) {return ch>0 && ch <=32;}

int StandardProtocolHandler::endRequest( cmdbind::ProtocolHandler* chnd, std::ostream& out)
{
	cmdbind::ProtocolHandlerR chr( chnd);
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

bool StandardProtocolHandler::redirectConsumedInput( cmdbind::DoctypeFilterProtocolHandler* fromh, cmdbind::ProtocolHandler* toh, std::ostream& out)
{
	void* buf;
	std::size_t bufsize;
	fromh->getInputBuffer( buf, bufsize);
	return Parent::redirectInput( buf, bufsize, toh, out);
}

int StandardProtocolHandler::endErrDocumentType( cmdbind::ProtocolHandler* ch, std::ostream& out)
{
	cmdbind::ProtocolHandlerR chr( ch);
	const char* err = ch->lastError();
	out << "ERR " << (err?err:"document type")<< LineProtocolHandler::endl();
	return stateidx();
}

int StandardProtocolHandler::endDoctypeDetection( cmdbind::ProtocolHandler* ch, std::ostream& out)
{
	cmdbind::DoctypeFilterProtocolHandler* chnd = dynamic_cast<cmdbind::DoctypeFilterProtocolHandler*>( ch);
	cmdbind::ProtocolHandlerR chr( ch);

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
		cmdbind::ProtocolHandler* delegate_ch = (cmdbind::ProtocolHandler*)new cmdbind::EscDlfProtocolHandler();
		delegate_ch->setLastError( msg.str());
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
			delegateProcessing<&StandardProtocolHandler::endErrDocumentType>( delegate_ch);
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

	cmdbind::CommandHandlerR exec( execContext()->provider()->cmdhandler( m_command, info->docformat()));	
	if (!exec.get())
	{
		std::ostringstream msg;
		if (m_command.empty())
		{
			msg << "got no document type and no command defined (empty request ?)";
		}
		else
		{
			msg << "no command handler for '" << m_command << "'";
		}
		cmdbind::ProtocolHandler* execch = (cmdbind::ProtocolHandler*)new cmdbind::EscDlfProtocolHandler();
		execch->setExecContext( execContext());
		execch->setLastError( msg.str());
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
			delegateProcessing<&StandardProtocolHandler::endErrDocumentType>( execch);
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
		exec->setDoctypeInfo( info);

		EscDlfProtocolHandler* execch = new EscDlfProtocolHandler( cmdbind::CommandHandlerR( exec));
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
			delegateProcessing<&StandardProtocolHandler::endRequest>( execch);
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

int StandardProtocolHandler::doRequest( int argc, const char** argv, std::ostream& out)
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
	ProtocolHandler* ch = (ProtocolHandler*)new cmdbind::DoctypeFilterProtocolHandler();
	ch->setExecContext( execContext());

	delegateProcessing<&StandardProtocolHandler::endDoctypeDetection>( ch);
	return stateidx();
}

