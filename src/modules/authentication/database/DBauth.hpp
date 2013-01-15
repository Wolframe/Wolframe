/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
// database authenticator
//

#ifndef _DB_AUTHENTICATION_HPP_INCLUDED
#define _DB_AUTHENTICATION_HPP_INCLUDED

#include "AAAA/authentication.hpp"
#include "constructor.hpp"

namespace _Wolframe {
namespace AAAA {

static const char* DB_AUTHENTICATION_CLASS_NAME = "DBAuth";

class DBAuthConfig : public config::NamedConfiguration
{
	friend class DBauthConstructor;
public:
	DBAuthConfig( const char* cfgName, const char* logParent, const char* logName )
		: config::NamedConfiguration( cfgName, logParent, logName )	{}

	virtual const char* className() const		{ return DB_AUTHENTICATION_CLASS_NAME; }

	/// methods
	bool parse( const config::ConfigurationTree& pt, const std::string& node,
		    const module::ModulesDirectory* modules );
	bool check() const;

	void print( std::ostream& os, size_t indent ) const;

	void setCanonicalPathes( const std::string& /*refPath*/ )	{}
private:
	std::string	m_identifier;
	std::string	m_dbLabel;
};


class DBauthenticator : public AuthenticationUnit
{
public:
	DBauthenticator( const std::string& Identifier, const std::string& dbLabel );
	~DBauthenticator();

	const char* className() const			{ return DB_AUTHENTICATION_CLASS_NAME; }

	bool resolveDB( const db::DatabaseProvider& db );

	AuthenticatorInstance* instance()		{ return NULL; }
private:
	const std::string	m_dbLabel;
	const db::Database*	m_db;
};

class DBauthConstructor : public ConfiguredObjectConstructor< AuthenticationUnit >
{
public:
	virtual ObjectConstructorBase::ObjectType objectType() const
						{ return AUTHENTICATION_OBJECT; }
	const char* objectClassName() const			{ return DB_AUTHENTICATION_CLASS_NAME; }
	DBauthenticator* object( const config::NamedConfiguration& conf );
};

}} // namespace _Wolframe::AAAA

#endif // _DB_AUTHENTICATION_HPP_INCLUDED
