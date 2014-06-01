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
#include <boost/algorithm/string.hpp>


using namespace _Wolframe;
using namespace _Wolframe::AAAA;

bool TestAuthenticationConfig::parse( const config::ConfigurationNode& pt, const std::string& node, const module::ModulesDirectory*)
{
	!!!!! HIER WEITER
}

bool TestAuthenticationConfig::check() const
{
	return true;
}

void TestAuthenticationConfig::print( std::ostream& os, size_t indent) const
{
	os << "Test authentication " << m_identifier << std::endl;
}



const std::string* TestAuthenticationUnit::mechs() const
{
	static const std::string mechs[2] = {"TEST",""};
	return mechs;
}

AuthenticatorSlice* TestAuthenticationUnit::slice( const std::string& mech, const net::RemoteEndpoint&)
{
	if (boost::algorithm::iequals( mech, "TEST"))
	{
		return new TestAuthenticatorSlice();
	}
	else
	{
		LOG_ERROR << "Mech not available '" << mech << "'";
		return 0;
	}
}


