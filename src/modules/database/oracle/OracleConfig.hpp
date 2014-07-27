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
//
// Wolframe Oracle client configuration
//

#ifndef _ORACLE_CONFIG_HPP_INCLUDED
#define _ORACLE_CONFIG_HPP_INCLUDED

#include "config/configurationBase.hpp"
#include "serialize/configSerialize.hpp"
#include "serialize/descriptiveConfiguration.hpp"
#include <vector>

namespace _Wolframe {
namespace db {

static const char* ORACLE_DB_CLASS_NAME = "Oracle";
enum {
	DEFAULT_ORACLE_PORT = 1521,
	DEFAULT_ORACLE_CONNECTIONS = 4,
	DEFAULT_ORACLE_ACQUIRE_TIMEOUT = 0
};

/// \brief SQLite database configuration
class OracleConfig
	:public _Wolframe::serialize::DescriptiveConfiguration
{
public:
	const char* className() const				{ return ORACLE_DB_CLASS_NAME; }

	OracleConfig()
		:_Wolframe::serialize::DescriptiveConfiguration(ORACLE_DB_CLASS_NAME, "database", "sqlite", getStructDescription())
		,m_host("")
		,m_port(DEFAULT_ORACLE_PORT)
		,m_connections(DEFAULT_ORACLE_CONNECTIONS)
		,m_acquireTimeout(DEFAULT_ORACLE_ACQUIRE_TIMEOUT)
	{
		setBasePtr( (void*)this); // ... mandatory to set pointer to start of configuration
	}

	OracleConfig( const std::string& id_, const std::string& host_,
			unsigned short port_,const std::string& dbName_,
			const std::string& user_, const std::string& password_,
			unsigned short connections_,
			unsigned short acquireTimeout_)
		:_Wolframe::serialize::DescriptiveConfiguration(ORACLE_DB_CLASS_NAME, "database", "sqlite", getStructDescription())
		,m_ID(id_)
		,m_host(host_)
		,m_port(port_)
		,m_dbName(dbName_)
		,m_user(user_)
		,m_password(password_)
		,m_connections(connections_)
		,m_acquireTimeout(acquireTimeout_){}

	OracleConfig( const char* title, const char* logprefix)
		:_Wolframe::serialize::DescriptiveConfiguration( title, "database", logprefix, getStructDescription())
		,m_host("")
		,m_port(DEFAULT_ORACLE_PORT)
		,m_connections(DEFAULT_ORACLE_CONNECTIONS)
		,m_acquireTimeout(DEFAULT_ORACLE_ACQUIRE_TIMEOUT)
	{
		setBasePtr( (void*)this); // ... mandatory to set pointer to start of configuration
	}

	virtual bool parse( const config::ConfigurationNode& cfgTree, const std::string& node,
			    const module::ModulesDirectory* modules );
	virtual bool check() const;
	virtual void print( std::ostream& os, size_t indent ) const;

	const std::string& ID() const			{ return m_ID; }
	const std::string& host() const			{ return m_host; }
	unsigned short port() const			{ return m_port; }
	const std::string& dbName() const		{ return m_dbName; }
	const std::string& user() const			{ return m_user; }
	const std::string& password() const		{ return m_password; }
	unsigned short connections() const		{ return m_connections; }
	unsigned short acquireTimeout() const		{ return m_acquireTimeout; }

public:
	/// \brief Structure description for serialization/parsing
	static const serialize::StructDescriptionBase* getStructDescription();

private:
	std::string	m_ID;			//< database identifier
	std::string	m_host;			//< server host
	unsigned short	m_port;			//< server port
	std::string	m_dbName;		//< database name on server
	std::string	m_user;			//< database user
	std::string	m_password;		//< and password
	unsigned short	m_connections;	 	//< number of database connection (pool size)
	unsigned short	m_acquireTimeout;	//< timeout when acquiring a connection from the pool

	config::ConfigurationTree::Position m_config_pos;

private:
	//\brief Check the domains of the configured values and do some mappings (e.g. instantiating enum values from strings)
	bool mapValueDomains();
};

}} // _Wolframe::db

#endif // _ORACLE_CONFIG_HPP_INCLUDED
