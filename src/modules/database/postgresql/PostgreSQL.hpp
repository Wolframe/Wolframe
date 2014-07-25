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
// Wolframe PostgreSQL client
//

#ifndef _POSTGRESQL_HPP_INCLUDED
#define _POSTGRESQL_HPP_INCLUDED

#include "logger-v1.hpp"
#include <libpq-fe.h>
#include <list>
#include <vector>
#include "database/database.hpp"
#include "database/transaction.hpp"
#include "PostgreSQLServerSettings.hpp"
#include "config/configurationBase.hpp"
#include "serialize/configSerialize.hpp"
#include "module/constructor.hpp"
#include "system/objectPool.hpp"

#ifdef _WIN32
#pragma warning(disable:4250)
#endif

namespace _Wolframe {
namespace db {

enum {
	POSTGRESQL_MIN_DB_VERSION = 80400,
	POSTGRESQL_MIN_PROTOCOL_VERSION = 3
};

struct PostgreSQLLanguageDescription : public LanguageDescription
{
	virtual std::string stm_argument_reference( int index) const
	{
		std::ostringstream rt;
		rt << "$" << index;
		return rt.str( );
	}
};

class PostgreSQLDatabase : public Database
{
public:
	PostgreSQLDatabase() : m_unit( NULL )	{}
	~PostgreSQLDatabase()			{}

	void setUnit( PostgreSQLdbUnit* unit )	{ m_unit = unit; }
	bool hasUnit() const			{ return m_unit != NULL; }
	PostgreSQLdbUnit& dbUnit() const	{ return *m_unit; }

	const std::string& ID() const;
	const char* className() const		{ return POSTGRESQL_DB_CLASS_NAME; }

	Transaction* transaction( const std::string& name );
	void closeTransaction( Transaction* t );

	virtual const LanguageDescription* getLanguageDescription( ) const
	{
		static PostgreSQLLanguageDescription langdescr;
		return &langdescr;
	}

private:
	PostgreSQLdbUnit*	m_unit;		 //< parent database unit
};


class PostgreSQLdbUnit : public DatabaseUnit
{
public:
	PostgreSQLdbUnit( const std::string& id,
			  const std::string& host, unsigned short port, const std::string& dbName,
			  const std::string& user, const std::string& password,
			  std::string sslMode, std::string sslCert, std::string sslKey,
			  std::string sslRootCert, std::string sslCRL,
			  unsigned short connectTimeout,
			  size_t connections, unsigned short acquireTimeout,
			  unsigned statementTimeout);
	~PostgreSQLdbUnit();

	const std::string& ID() const		{ return m_ID; }
	const char* className() const		{ return POSTGRESQL_DB_CLASS_NAME; }
	Database* database();
	static _Wolframe::log::LogLevel::Level getLogLevel( const std::string& severity);

	static void noticeProcessor( void* this_void, const char * message);

	PoolObject<PGconn*>* newConnection()	{return new PoolObject<PGconn*>( m_connPool);}

	PostgreSQLServerSettings serverSettings() const
						{ return m_serverSettings; }

private:
	const std::string	m_ID;			//< database ID
	std::string		m_connStr;		//< connection string
	size_t			m_noConnections;	//< number of connections
	PostgreSQLServerSettings m_serverSettings;	//< data like protocol settings, OIDs, etc. loaded at initialization from server
	ObjectPool< PGconn* >	m_connPool;		//< pool of connections
	unsigned		m_statementTimeout;	//< default statement execution timeout
	PostgreSQLDatabase	m_db;			//< real database object
};


//***  PostgreSQL database constructor  ***************************************
class PostgreSQLConstructor : public ConfiguredObjectConstructor< db::DatabaseUnit >
{
public:
	ObjectConstructorBase::ObjectType objectType() const
						{ return DATABASE_OBJECT; }
	const char* objectClassName() const	{ return POSTGRESQL_DB_CLASS_NAME; }
	PostgreSQLdbUnit* object( const config::NamedConfiguration& conf );
};


///\class PostgreSQLtransaction
class PostgreSQLtransaction
	:public Transaction
{
public:
	PostgreSQLtransaction( PostgreSQLDatabase& database, const std::string& name_);
	virtual ~PostgreSQLtransaction(){}
};



}} // _Wolframe::db

#endif // _POSTGRESQL_HPP_INCLUDED
