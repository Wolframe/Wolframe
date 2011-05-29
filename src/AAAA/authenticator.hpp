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
// authenticator
//

#ifndef _AUTHENTICATOR_HPP_INCLUDED
#define _AUTHENTICATOR_HPP_INCLUDED

#include <string>
#include "config/configurationBase.hpp"

#include "database/database.hpp"

namespace _Wolframe {
namespace AAAA {

/// Authentication type
enum AuthenticationType	{
	AUTH_DATABASE,		///< Wolframe proprietary authentification using db
	AUTH_TEXTFILE,		///< Wolframe proprietary authentification
	AUTH_PAM,		///< *NIX PAM authentification
	AUTH_SASL,		///< SASL authentification (Cyrus SASL)
	AUTH_LDAP,		///< LDAP authentification
	AUTH_UNKNOWN		///< Unknown authentification (i.e. none)
};


class AuthenticationConfigurationBase : public config::ConfigurationBase
{
public:
	/// constructor
	AuthenticationConfigurationBase( const char* name, const char* logParent, const char* logName )
		: config::ConfigurationBase( name, logParent, logName ){}

	virtual ~AuthenticationConfigurationBase()			{}

	virtual AuthenticationType type() const = 0;
};


class DatabaseAuthConfig : public AuthenticationConfigurationBase
{
	friend class DatabaseAuth;
	friend class config::ConfigurationParser;
public:
	DatabaseAuthConfig( const char* cfgName, const char* logParent, const char* logName )
		: AuthenticationConfigurationBase( cfgName, logParent, logName ),
		  m_dbConfig( "", logParent, "Database" )	{}

	virtual AuthenticationType type() const			{ return AUTH_DATABASE; }

	/// methods
	bool check() const					{ return m_dbConfig.check(); }
	void print( std::ostream& os, size_t indent ) const	{
		std::string indStr( indent, ' ' );
		os << indStr << sectionName() << ": " << std::endl;
		m_dbConfig.print( os, indent + 3 );
	}

	void setCanonicalPathes( const std::string& refPath )	{ m_dbConfig.setCanonicalPathes( refPath ); }
private:
	db::SingleDBConfiguration	m_dbConfig;
};


class TextFileAuthConfig : public AuthenticationConfigurationBase
{
	friend class TextFileAuth;
	friend class config::ConfigurationParser;
public:
	TextFileAuthConfig( const char* cfgName, const char* logParent, const char* logName )
		: AuthenticationConfigurationBase( cfgName, logParent, logName ){}

	virtual AuthenticationType type() const			{ return AUTH_TEXTFILE; }

	/// methods
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );
private:
	std::string	m_file;
};


/******************************************************************************************************/
class AuthenticationConfiguration : public config::ConfigurationBase
{
	friend class AAAAprovider;
	friend class config::ConfigurationParser;
public:
	/// constructor
	AuthenticationConfiguration( const char* cfgName, const char* logParent, const char* logName )
		: ConfigurationBase( cfgName, logParent, logName )	{}
	~AuthenticationConfiguration();

	/// methods
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );

	// bool test() const;	// Not implemented yet, inherited from base
private:
	std::list<AuthenticationConfigurationBase*>	m_config;
};

class AuthenticatorBase
{
public:
	virtual	~AuthenticatorBase()				{}
	virtual bool resolveDB( const db::DBprovider& /*db*/ )	{ return true; }
};

class TextFileAuth : public AuthenticatorBase
{
public:
	TextFileAuth( const TextFileAuthConfig& conf );
	~TextFileAuth()						{}
private:
	std::string	m_file;
};


class DatabaseAuth : public AuthenticatorBase
{
public:
	DatabaseAuth( const DatabaseAuthConfig& conf );
	~DatabaseAuth();

	bool resolveDB( const db::DBprovider& db );
private:
	std::string		m_dbLabel;
	const db::Database*	m_db;
};

}} // namespace _Wolframe::AAAA

#endif // _AUTHENTICATOR_HPP_INCLUDED
