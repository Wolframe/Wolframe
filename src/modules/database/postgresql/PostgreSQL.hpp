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
// Wolframe PostgreSQL client
//

#ifndef _POSTGRESQL_HPP_INCLUDED
#define _POSTGRESQL_HPP_INCLUDED

#include "logger-v1.hpp"
#include <libpq-fe.h>
#include <list>
#include "database/database.hpp"
#include "database/transaction.hpp"
#include "PostgreSQLprogram.hpp"
#include "config/configurationBase.hpp"
#include "module/constructor.hpp"
#include "system/objectPool.hpp"

#ifdef _WIN32
#pragma warning(disable:4250)
#endif

namespace _Wolframe {
namespace db {

static const char* POSTGRESQL_DB_CLASS_NAME = "PostgreSQL";

static const unsigned int POSTGRESQL_MIN_DB_VERSION = 80400;
static const unsigned short POSTGRESQL_MIN_PROTOCOL_VERSION = 3;

/// PostgreSQL server connection configuration
class PostgreSQLconfig : public config::NamedConfiguration
{
	friend class PostgreSQLconstructor;
public:
	const char* className() const		{ return POSTGRESQL_DB_CLASS_NAME; }

	PostgreSQLconfig( const char* name, const char* logParent, const char* logName );
	~PostgreSQLconfig()			{}

	bool parse( const config::ConfigurationTree& pt, const std::string& node,
		    const module::ModulesDirectory* modules );
	virtual bool check() const;
	virtual void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );

	const std::string& host() const		{return m_host;}
	unsigned short port() const		{return m_port;}
	const std::string& dbName() const	{return m_dbName;}
	const std::string& user() const		{return m_user;}
	const std::string& password() const	{return m_password;}
	const std::list<std::string>& programFiles() const	{return m_programFiles;}

private:
	std::string	m_ID;			///< database identifier
	std::string	m_host;			///< server host
	unsigned short	m_port;			///< server port
	std::string	m_dbName;		///< database name on server
	std::string	m_user;			///< database user
	std::string	m_password;		///< and password
	std::string	sslMode;		///< SSL connection mode
	std::string	sslCert;		///< client SSL certificate file
	std::string	sslKey;			///< client SSL key file
	std::string	sslRootCert;		///< root SSL certificate file
	std::string	sslCRL;			///< SSL certificate revocation list
	unsigned short	connectTimeout;		///< connection timeout
	unsigned short	connections;		///< number of database connection (pool size)
	unsigned short	acquireTimeout;		///< timeout when acquiring a connection from the pool
	unsigned	statementTimeout;	///< default timeout when executin a statement
	std::list< std::string > m_programFiles;///< list of program files
};


class PostgreSQLdbUnit;
class PostgreSQLdatabase;

class PostgreSQLtransaction : public Transaction
{
public:
	PostgreSQLtransaction( PostgreSQLdatabase& database, const std::string& name_);
	 ~PostgreSQLtransaction();

	virtual const std::string& databaseID() const;

	virtual void putInput( const TransactionInput& input_ )		{ m_input = input_; }
	virtual const TransactionOutput& getResult() const		{ return m_output; }

	virtual void execute();
	virtual void begin();
	virtual void commit();
	virtual void rollback();
	virtual void close();

private:
	void execute_statement( const char* statement );
	///\brief Execute as transaction (all operations belong to one transaction implicitely defined)
	void execute_as_transaction();
	///\brief Execute with transaction context defined in outer context
	void execute_as_operation();

private:
	PostgreSQLdatabase&	m_db;		//< parent database
	PostgreSQLdbUnit&	m_unit;		//< parent database unit
	std::string		m_name;		//< name of transaction
	TransactionInput	m_input;	//< input data structure
	TransactionOutput	m_output;	//< output data structure
	PoolObject<PGconn*>* m_conn;		//< reference to connection object from pool
};


struct PostgreSQLLanguageDescription : public LanguageDescription
{
	virtual std::string stm_argument_reference( int index)
	{
		std::ostringstream rt;
		rt << "$" << index;
		return rt.str( );
	}
};


class PostgreSQLdatabase : public Database
{
public:
	PostgreSQLdatabase() : m_unit( NULL )	{}
	~PostgreSQLdatabase()			{}

	void setUnit( PostgreSQLdbUnit* unit )	{ m_unit = unit; }
	bool hasUnit() const			{ return m_unit != NULL; }
	PostgreSQLdbUnit& dbUnit() const	{ return *m_unit; }

	const std::string& ID() const;
	const char* className() const		{ return POSTGRESQL_DB_CLASS_NAME; }

	virtual void loadProgram( const std::string& filename );
	virtual void loadAllPrograms();
	virtual void addProgram( const std::string& program );

	Transaction* transaction( const std::string& name );
	void closeTransaction( Transaction* t );

	virtual const LanguageDescription* getLanguageDescription( ) const
	{
		static PostgreSQLLanguageDescription langdescr;
		return &langdescr;
	}

private:
	PostgreSQLdbUnit*	m_unit;		///< parent database unit
};


class PostgreSQLdbUnit : public DatabaseUnit
{
	friend class PostgreSQLtransaction;
public:
	PostgreSQLdbUnit( const std::string& id,
			  const std::string& host, unsigned short port, const std::string& dbName,
			  const std::string& user, const std::string& password,
			  std::string sslMode, std::string sslCert, std::string sslKey,
			  std::string sslRootCert, std::string sslCRL,
			  unsigned short connectTimeout,
			  size_t connections, unsigned short acquireTimeout,
			  unsigned statementTimeout,
			  const std::list<std::string>& programFiles_);
	~PostgreSQLdbUnit();

	const std::string& ID() const		{ return m_ID; }
	const char* className() const		{ return POSTGRESQL_DB_CLASS_NAME; }
	Database* database();
	static _Wolframe::log::LogLevel::Level getLogLevel( const std::string& severity);

	virtual void loadProgram( const std::string& filename );
	/// MBa: I have to think a bit how to handle this
	virtual void loadAllPrograms();

	virtual void addProgram( const std::string& program )
						{ m_program.load( program ); }

	static void noticeProcessor( void* this_void, const char * message);

private:
	const std::string	m_ID;			///< database ID
	std::string		m_connStr;		///< connection string
	size_t			m_noConnections;	///< number of connections
	ObjectPool< PGconn* >	m_connPool;		///< pool of connections
	unsigned		m_statementTimeout;	///< default statement execution timeout
	PostgreSQLdatabase	m_db;			///< real database object
	PostgreSQLprogram	m_program;
	std::list<std::string>	m_programFiles;
};


//***  PostgreSQL database constructor  ***************************************
class PostgreSQLconstructor : public ConfiguredObjectConstructor< db::DatabaseUnit >
{
public:
	ObjectConstructorBase::ObjectType objectType() const
						{ return DATABASE_OBJECT; }
	const char* objectClassName() const	{ return POSTGRESQL_DB_CLASS_NAME; }
	PostgreSQLdbUnit* object( const config::NamedConfiguration& conf );
};

}} // _Wolframe::db

#endif // _POSTGRESQL_HPP_INCLUDED
