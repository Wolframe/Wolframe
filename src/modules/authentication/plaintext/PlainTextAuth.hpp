/************************************************************************

 Copyright (C) 2011 Project Wolframe.
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
//
// Plain text authentication
//

#ifndef _PLAIN_TEXT_AUTHENTICATION_HPP_INCLUDED
#define _PLAIN_TEXT_AUTHENTICATION_HPP_INCLUDED

#include <string>
#include "AAAA/authentication.hpp"
#include "moduleInterface.hpp"

namespace _Wolframe {
namespace AAAA {

class PlainTextAuthConfig :  public config::ObjectConfiguration
{
	friend class PlainTextAuthContainer;
public:
	PlainTextAuthConfig( const char* cfgName, const char* logParent, const char* logName )
		: config::ObjectConfiguration( cfgName, logParent, logName ) {}

	virtual const char* objectName() const		{ return "PlainTextAuth"; }

	/// methods
	bool parse( const config::ConfigurationTree& pt, const std::string& node,
		    const module::ModulesDirectory* modules );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );
private:
	std::string		m_file;
};


class PlainTextAuthenticator : public AuthenticationUnit
{
public:
	PlainTextAuthenticator( const std::string& filename );
	~PlainTextAuthenticator();
	virtual const char* typeName() const		{ return "PlainTextAuth"; }

	AuthenticatorSlice* authSlice()		{ return NULL; }

private:
	std::string		m_file;
};


class PlainTextAuthContainer : public ObjectContainer< AuthenticationUnit >
{
public:
	PlainTextAuthContainer( const PlainTextAuthConfig& conf );
	~PlainTextAuthContainer()			{}

	virtual const char* objectName() const		{ return m_auth->typeName(); }
	virtual AuthenticationUnit* object() const	{ return m_auth; }
	void dispose()					{ m_auth = NULL; delete this; }
private:
	PlainTextAuthenticator*	m_auth;
};

}} // namespace _Wolframe::AAAA

#endif // _PLAIN_TEXT_AUTHENTICATION_HPP_INCLUDED
