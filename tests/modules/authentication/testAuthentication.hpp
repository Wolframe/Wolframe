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
///\file testAuthentication.hpp
///\brief Interface of a fake authentication mechanism to test the protocol
#include "module/moduleInterface.hpp"
#include "module/constructor.hpp"
#include "AAAA/authenticator.hpp"
#include <boost/lexical_cast.hpp>

namespace _Wolframe {
namespace AAAA {

class TestAuthenticationConfig
	:public config::NamedConfiguration
{
	friend class TestAuthenticationConstructor;
public:
	TestAuthenticationConfig( const char* cfgName, const char* logParent, const char* logName)
		:config::NamedConfiguration( cfgName, logParent, logName) {}

	virtual const char* className() const	
		{return "TestAuthentication";}

	bool parse( const config::ConfigurationNode& pt, const std::string& node, const module::ModulesDirectory* modules);
	bool check() const;
	void print( std::ostream& os, size_t indent) const;
	void setCanonicalPathes( const std::string&){}

private:
	std::string m_identifier;
	std::string m_pattern;
};


class TestAuthenticationUnit
	:public AuthenticationUnit
{
public:
	TestAuthenticationUnit( const std::string& Identifier, const std::string& filename);

	~TestAuthenticationUnit();

	virtual const char* className() const		{return "TestAuthentication";}

	const std::string* mechs() const;

	AuthenticatorSlice* slice( const std::string& mech, const net::RemoteEndpoint& client);
};


class TestAuthenticatorSlice
	:public AuthenticatorSlice
{
public:
	TestAuthenticatorSlice( const std::string& identifier_)
		:m_identifier(identifier_){}

	~TestAuthenticatorSlice();

	void destroy();

	virtual const char* className() const		{return "Test";}

	virtual const std::string& identifier() const	{return m_identifier;}

	/// Get the list of available mechs
	virtual const std::vector<std::string>& mechs() const;

	/// Set the authentication mech
	virtual bool setMech( const std::string& mech);

	/// The input message
	virtual void messageIn( const std::string& message);

	/// The output message
	virtual const std::string& messageOut();

	/// The current status of the authenticator slice
	virtual Status status() const;

	/// The authenticated user or NULL if not authenticated
	virtual User* user() const;

private:
	std::string m_identifier;
	User* m_user;
};

}}//namespace
#endif


