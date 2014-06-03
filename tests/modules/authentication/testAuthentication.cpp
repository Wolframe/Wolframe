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
			( "pattern", &TestAuthenticationConfig::Structure::m_pattern)		.optional()
			;
		}
	};
	static const TestAuthenticationConfigDescription rt;
	return &rt;
}

bool TestAuthenticationConfig::parse( const config::ConfigurationNode& pt, const std::string&, const module::ModulesDirectory*)
{
	try
	{
		config::parseConfigStructure( structure, pt);
		m_config_pos = pt.data().position;
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
	return true;
}

void TestAuthenticationConfig::print( std::ostream& os, size_t indent) const
{
	std::string indentstr = std::string(indent,' ') + std::string(indent,' ');
	os << indentstr << "Test authentication " << structure.m_identifier << " pattern '" << structure.m_pattern << "'" << std::endl;
}


TestAuthenticationUnit::TestAuthenticationUnit( const TestAuthenticationConfig& cfg)
	:AuthenticationUnit(cfg.structure.m_identifier),m_pattern(cfg.structure.m_pattern)
{}

const char** TestAuthenticationUnit::mechs() const
{
	static const char* ar[2] = {"TEST",""};
	return ar;
}

AuthenticatorSlice* TestAuthenticationUnit::slice( const std::string& mech, const net::RemoteEndpoint&)
{
	if (boost::algorithm::iequals( mech, "TEST"))
	{
		return new TestAuthenticatorSlice( identifier(), m_pattern);
	}
	else
	{
		LOG_ERROR << "Mech not available '" << mech << "'";
		return 0;
	}
}

struct MemBlockHdr
{
	int chk;
};

void* TestAuthenticatorSlice::operator new( std::size_t n)
{
	MemBlockHdr* blk = (MemBlockHdr*)std::malloc( sizeof(MemBlockHdr) + n);
	blk->chk = 123;
	return blk+1;
}

void TestAuthenticatorSlice::operator delete( void* ptr)
{
	MemBlockHdr* hdr = (MemBlockHdr*)ptr - 1;
	if (hdr->chk != 123) throw std::logic_error( "illegal free");
	std::free( hdr);
}


TestAuthenticatorSlice::TestAuthenticatorSlice( const std::string& identifier_, const std::string& pattern_)
	:m_state(Init),m_status(AuthenticatorSlice::MESSAGE_AVAILABLE),m_identifier(identifier_),m_pattern(pattern_),m_user(0)
{
	m_mechs.push_back("TEST");
}

TestAuthenticatorSlice::~TestAuthenticatorSlice()
{
	if (m_user) delete m_user;
}

bool TestAuthenticatorSlice::setMech( const std::string& mech)
{
	if (boost::algorithm::iequals( mech, "TEST"))
	{
		m_state = AskUsername;
		LOG_DEBUG << "Authentication mech set to TEST";
		return true;
	}
	else
	{
		LOG_ERROR << "Mech not available '" << mech << "'";
		return false;
	}
}

static int alphaIdx( char ch)
{
	if (ch >= 'A' && ch <= 'Z')
	{
		return (int)(ch - 'A' + 1);
	}
	if (ch >= 'a' && ch <= 'z')
	{
		return -(int)(ch - 'a' + 1);
	}
	return 0;
}

static char decode( char ch, char pt)
{
	if (ch >= 'A' && ch <= 'Z')
	{
		return 'A' + ((ch - 'A' + alphaIdx(pt) + 26) % 26);
	}
	if (ch >= 'a' && ch <= 'z')
	{
		return 'a' + ((ch - 'a' + alphaIdx(pt) + 26) % 26);
	}
	return ch;
}

void TestAuthenticatorSlice::messageIn( const std::string& msg)
{
	switch (m_state)
	{
		case Init:
			LOG_ERROR << "test authentication protocol error: Got message in state 'Init'";
			m_status = AuthenticatorSlice::SYSTEM_FAILURE;
			m_state = Done;
			break;
		case AskUsername:
			LOG_ERROR << "test authentication protocol error: Got message in state 'AskUsername'";
			m_status = AuthenticatorSlice::SYSTEM_FAILURE;
			m_state = Done;
			break;
		case WaitUsername:
			if (m_pattern.empty())
			{
				m_username = msg;
			}
			else
			{
				std::string decoded_msg;
				std::size_t idx;
				std::string::const_iterator mi = msg.begin(), me = msg.end();
				for (; mi != me; ++mi,idx=(idx+1)%m_pattern.size())
				{
					m_username.push_back( decode(*mi, m_pattern[idx]));
				}
			}
			m_state = AskPassword;
			break;
		case AskPassword:
			LOG_ERROR << "test authentication protocol error: Got message in state 'AskPassword'";
			m_status = AuthenticatorSlice::SYSTEM_FAILURE;
			m_state = Done;
			break;
		case WaitPassword:
			if (msg.size() && msg[0] >= 'A' && msg[0] <= 'Z')
			{
				m_status = AuthenticatorSlice::AUTHENTICATED;
				if (m_user) delete m_user;
				m_user = 0;
				m_user = new User( "Test", boost::algorithm::to_lower_copy(m_username), m_username);
			}
			else
			{
				LOG_ERROR << "authentication with mech 'Test' failed: invalid credentials";
				m_status = AuthenticatorSlice::INVALID_CREDENTIALS;
			}
			m_state = Done;
			break;
		case Done:
			LOG_ERROR << "test authentication protocol error: Got message in state 'Done'";
			m_status = AuthenticatorSlice::SYSTEM_FAILURE;
			m_state = Done;
			break;
	}
}

const std::string& TestAuthenticatorSlice::message( const char* cmd, const std::string& content)
{
	m_message = cmd;
	if (content.size())
	{
		m_message.push_back( ' ');
		m_message.append( content);
	}
	return m_message;
}

const std::string& TestAuthenticatorSlice::messageOut()
{
	switch (m_state)
	{
		case Init:
			LOG_ERROR << "Mech not set";
			m_status = AuthenticatorSlice::SYSTEM_FAILURE;
			m_state = Done;
			return message( "ERR", "protocol");

		case AskUsername:
			m_state = WaitUsername;
			return message( "UN", m_pattern);

		case WaitUsername:
			LOG_ERROR << "Protocol error";
			m_status = AuthenticatorSlice::SYSTEM_FAILURE;
			m_state = Done;
			return message( "ERR", "protocol");

		case AskPassword:
			m_state = WaitPassword;
			return message( "PW", m_pattern);

		case WaitPassword:
			LOG_ERROR << "Protocol error";
			m_status = AuthenticatorSlice::SYSTEM_FAILURE;
			m_state = Done;
			return message( "ERR", "protocol");

		case Done:
			if (m_status == AuthenticatorSlice::AUTHENTICATED)
			{
				return message( "AUTH");
			}
			else
			{
				return message( "ERR", "authentication denied");
			}
	}
	m_state = Done;
	return message( "ERR", "internal illegal state");
}

AuthenticatorSlice::Status TestAuthenticatorSlice::status() const
{
	switch (m_state)
	{
		case Init:
			return AuthenticatorSlice::MESSAGE_AVAILABLE;
		case AskUsername:
			return AuthenticatorSlice::MESSAGE_AVAILABLE;
		case AskPassword:
			return AuthenticatorSlice::MESSAGE_AVAILABLE;
		case WaitUsername:
			return AuthenticatorSlice::AWAITING_MESSAGE;
		case WaitPassword:
			return AuthenticatorSlice::AWAITING_MESSAGE;
		case Done:
			return m_status;
	}
	throw std::logic_error("bad state in test authenticator");
}

User* TestAuthenticatorSlice::user()
{
	switch (m_state)
	{
		case Init:
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


