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
///\file testAuthentication.cpp
///\brief Implementation of a fake authentication mechanism to test the protocol

#include "testAuthentication.hpp"
#include "utils/fileUtils.hpp"
#include "utils/stringUtils.hpp"
#include "config/structSerialize.hpp"
#include "serialize/struct/structDescription.hpp"
#include "logger-v1.hpp"
#include <boost/algorithm/string.hpp>
#include <cstring>

using namespace _Wolframe;
using namespace _Wolframe::AAAA;

const serialize::StructDescriptionBase* TestAuthenticationConfig::Structure::getStructDescription()
{
	struct TestAuthenticationConfigDescription :public serialize::StructDescription<TestAuthenticationConfig::Structure>
	{
		TestAuthenticationConfigDescription()
		{
			(*this)
			( "identifier", &TestAuthenticationConfig::Structure::m_identifier)	.mandatory()
			( "filename", &TestAuthenticationConfig::Structure::m_filename)		.mandatory()
			;
		}
	};
	static const TestAuthenticationConfigDescription rt;
	return &rt;
}

void TestAuthenticationConfig::setCanonicalPathes( const std::string& refPath)
{
	structure.m_filename = utils::getCanonicalPath( structure.m_filename, refPath);
}

bool TestAuthenticationConfig::parse( const config::ConfigurationNode& pt, const std::string&, const module::ModulesDirectory*)
{
	try
	{
		config::parseConfigStructure( structure, pt);
		m_config_pos = pt.position();
		return true;
	}
	catch (const std::runtime_error& e)
	{
		LOG_FATAL << logPrefix() << e.what();
		return false;
	}
}

bool TestAuthenticationConfig::check() const
{
	try
	{
		utils::readBinaryFileContent( structure.m_filename);
	}
	catch (const std::runtime_error& e)
	{
		LOG_ERROR << "check configuration failed (" << m_config_pos.logtext() << "): " << e.what();
	}
	return true;
}

void TestAuthenticationConfig::print( std::ostream& os, size_t indent) const
{
	std::string indentstr = std::string(indent,' ') + std::string(indent,' ');
	os << indentstr << "Test authentication " << structure.m_identifier << " filename '" << structure.m_filename << "'" << std::endl;
}


TestAuthenticationUnit::TestAuthenticationUnit( const TestAuthenticationConfig& cfg)
	:AuthenticationUnit(cfg.structure.m_identifier)
{
	std::vector<std::string> entries;
	utils::splitString( entries, utils::readBinaryFileContent( cfg.structure.m_filename), "\r\n");
	std::vector<std::string>::const_iterator ei = entries.begin(), ee = entries.end();
	for (; ei != ee; ++ei)
	{
		const char* cc = std::strchr( ei->c_str(),' ');
		if (!cc) throw std::runtime_error( "illegal entry in test login entries file");
		m_usrpwdmap[ std::string( ei->c_str(), cc-ei->c_str())] = std::string( cc+1);
	}
}

const char** TestAuthenticationUnit::mechs() const
{
	static const char* ar[2] = {"TEST",0};
	return ar;
}

AuthenticatorSlice* TestAuthenticationUnit::slice( const std::string& mech, const net::RemoteEndpoint&)
{
	if (boost::algorithm::iequals( mech, "TEST"))
	{
		TestAuthenticatorSlice* rt = new TestAuthenticatorSlice( identifier(), &m_usrpwdmap);
		LOG_DEBUG << "[test authentication] unit creates new slice in state " << TestAuthenticatorSlice::stateName(rt->state());
		return rt;
	}
	else
	{
		LOG_ERROR << "Mech not available '" << mech << "'";
		return 0;
	}
}

TestAuthenticatorSlice::TestAuthenticatorSlice( const std::string& identifier_, const std::map<std::string,std::string>* usrpwdmap_)
	:m_state(AskUsername),m_status(AuthenticatorSlice::MESSAGE_AVAILABLE),m_identifier(identifier_),m_user(0),m_usrpwdmap(usrpwdmap_),
	m_inputReusable(false)
{}

TestAuthenticatorSlice::~TestAuthenticatorSlice()
{
	if (m_user) delete m_user;
}

bool TestAuthenticatorSlice::checkCredentials( const types::SecureString& username, const types::SecureString& password)
{
	std::map<std::string,std::string>::const_iterator ui = m_usrpwdmap->find( username);
	if (ui == m_usrpwdmap->end())
	{
		LOG_ERROR << "authentication with mech 'Test' failed: user not found";
		m_status = AuthenticatorSlice::USER_NOT_FOUND;
		return false;
	}
	else if (password != ui->second)
	{
		LOG_ERROR << "authentication with mech 'Test' failed: invalid credentials";
		m_status = AuthenticatorSlice::INVALID_CREDENTIALS;
		return false;
	}
	else
	{
		m_status = AuthenticatorSlice::AUTHENTICATED;
		return true;
	}
}

void TestAuthenticatorSlice::setState( State st)
{
	if (st != m_state)
	{
		m_state = st;
		LOG_DATA << "[test authentication] new state '" << stateName(st) << "' external " << status();
	}
}

void TestAuthenticatorSlice::messageIn( const std::string& msg)
{
	LOG_DATA << "[test authentication] " << stateName(m_state) << " message in (" << msg << ")";
	switch (m_state)
	{
		case AskUsername:
		case AskPassword:
		case Done:
			LOG_ERROR << "test authentication protocol error: Got message in state '" << stateName(m_state) << "'";
			m_status = AuthenticatorSlice::SYSTEM_FAILURE;
			setState( Done);
			break;

		case WaitUsername:
			m_username = msg;
			setState( AskPassword);
			break;
		case WaitPassword:
			if (checkCredentials( m_username, msg))
			{
				if (m_user) delete m_user;
				m_user = 0;
				m_user = new User( "Test", m_username, m_username);
				m_username.clear();
			}
			setState( Done);
			break;
	}
}

std::string TestAuthenticatorSlice::message( const char* cmd, const std::string& content)
{
	std::string msgstr = cmd;
	if (content.size())
	{
		msgstr.push_back( ' ');
		msgstr.append( content);
	}
	LOG_DATA << "[test authentication] message out (" << msgstr << ")";
	return msgstr;
}

std::string TestAuthenticatorSlice::messageOut()
{
	switch (m_state)
	{
		case Done:
		case WaitPassword:
		case WaitUsername:
			LOG_ERROR << "test authentication protocol error: Asked for message in state '" << stateName(m_state) << "'";
			m_status = AuthenticatorSlice::SYSTEM_FAILURE;
			setState( Done);
			return message( "ERR", "protocol");

		case AskUsername:
			setState( WaitUsername);
			return message( "USERNAME");

		case AskPassword:
			setState( WaitPassword);
			return message( "PASSWORD");
	}
	setState( Done);
	return message( "ERR", "internal illegal state");
}

AuthenticatorSlice::Status TestAuthenticatorSlice::status() const
{
	AuthenticatorSlice::Status rt = m_status;
	switch (m_state)
	{
		case AskUsername:
			rt = AuthenticatorSlice::MESSAGE_AVAILABLE;
			break;
		case AskPassword:
			rt = AuthenticatorSlice::MESSAGE_AVAILABLE;
			break;
		case WaitUsername:
			rt = AuthenticatorSlice::AWAITING_MESSAGE;
			break;
		case WaitPassword:
			rt = AuthenticatorSlice::AWAITING_MESSAGE;
			break;
		case Done:
			rt = m_status;
	}
	return rt;
}

User* TestAuthenticatorSlice::user()
{
	switch (m_state)
	{
		case AskUsername:
		case AskPassword:
		case WaitUsername:
		case WaitPassword:
			break;
		case Done:
		{
			User* rt = m_user;
			m_user = 0;
			return rt;
		}
	}
	return 0;
}


