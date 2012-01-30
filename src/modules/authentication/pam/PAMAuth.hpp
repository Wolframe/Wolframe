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
// text file authentication
//

#ifndef _PAM_AUTHENTICATION_HPP_INCLUDED
#define _PAM_AUTHENTICATION_HPP_INCLUDED

#include <string>
#include "AAAA/authentication.hpp"
#include "moduleInterface.hpp"

namespace _Wolframe {
namespace AAAA {

class PAMAuthConfig :  public config::ObjectConfiguration
{
	friend class PAMAuthContainer;
public:
	PAMAuthConfig( const char* cfgName, const char* logParent, const char* logName )
		: config::ObjectConfiguration( cfgName, logParent, logName ) {}

	virtual const char* objectName() const		{ return "PAMAuth"; }

	/// methods
	bool parse( const config::ConfigurationTree& pt, const std::string& node,
		    const module::ModulesDirectory* modules );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
private:
	std::string	m_service;
};


class PAMAuthenticator : public AuthenticationUnit
{
public:
	PAMAuthenticator( const std::string& service );
	~PAMAuthenticator();
	virtual const char* typeName() const		{ return "PAMAuth"; }

private:
	std::string		m_service;
};


class PAMAuthContainer : public ObjectContainer< AuthenticationUnit >
{
public:
	PAMAuthContainer( const PAMAuthConfig& conf );
	~PAMAuthContainer()			{}

	virtual const char* objectName() const		{ return m_auth->typeName(); }
	virtual AuthenticationUnit* object() const	{ return m_auth; }
	void dispose()					{ m_auth = NULL; delete this; }
private:
	PAMAuthenticator*	m_auth;
};

}} // namespace _Wolframe::AAAA

#endif // _PAM_AUTHENTICATION_HPP_INCLUDED
