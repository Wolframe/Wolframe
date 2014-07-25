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
// Wolframe Oracle client
//

#ifndef _ORACLEQL_HPP_INCLUDED
#define _ORACLEQL_HPP_INCLUDED

#include "OracleConfig.hpp"
#include "database/database.hpp"
#include "database/transaction.hpp"
#include "database/transactionExecStatemachine.hpp"
#include "config/configurationBase.hpp"
#include "serialize/configSerialize.hpp"
#include "module/constructor.hpp"
#include "system/objectPool.hpp"
#include "logger-v1.hpp"
#include <list>
#include <vector>
#include <string>
#include <oci.h>

#ifdef _WIN32
#pragma warning(disable:4250)
#endif

namespace _Wolframe {
namespace db {

struct OracleLanguageDescription : public LanguageDescription
{
	virtual std::string stm_argument_reference( int index) const
	{
		std::ostringstream rt;
		rt << "$" << index;
		return rt.str( );
	}
};

class OracleEnvirenment
{
	public:
		OCIEnv *envhp; // OCI environemnt handle
};

class OracleDatabase : public Database
{
public:
	OracleDatabase() : m_unit( NULL )	{}
	~OracleDatabase()			{}

	const std::string& ID() const;
	const char* className() const		{ return ORACLE_DB_CLASS_NAME; }

	Transaction* transaction( const std::string& name );
	void closeTransaction( Transaction* t );

	virtual const LanguageDescription* getLanguageDescription( ) const
	{
		static OracleLanguageDescription langdescr;
		return &langdescr;
	}

private:
	OracleDbUnit*	m_unit;		//< parent database unit

public:
	OracleEnvirenment m_env;	//< Oracle environment
};

class OracleConnection
{
	public:
		OCIError *errhp; // error handle
		OCIServer *srvhp; // server handle
		OCISvcCtx *svchp; // service handle
		OCISession *authp; // user authentication handle
		OCITrans *transhp; // transaction handle
};

class OracleDbUnit : public DatabaseUnit
{
	friend class OracleTransaction;
public:
	OracleDbUnit( const std::string& id,
			  const std::string& host, unsigned short port, const std::string& dbName,
			  const std::string& user, const std::string& password,
			  size_t connections, unsigned short acquireTimeout,
			  unsigned statementTimeout);
	~OracleDbUnit();

	const std::string& ID() const		{ return m_ID; }
	const char* className() const		{ return ORACLE_DB_CLASS_NAME; }
	Database* database();
	static _Wolframe::log::LogLevel::Level getLogLevel( const std::string& severity);

	PoolObject<OracleConnection *> *newConnection( ) { return new PoolObject<OracleConnection *>( m_connPool ); }

private:
	const std::string	m_ID;			//< database ID
	std::string		m_connStr;		//< connection string
	size_t			m_noConnections;	//< number of connections
	ObjectPool< OracleConnection* >	m_connPool;	//< pool of connections
	unsigned		m_statementTimeout;	//< default statement execution timeout
	OracleDatabase	m_db;				//< real database object
};


//***  Oracle database constructor  ***************************************
class OracleConstructor : public ConfiguredObjectConstructor< db::DatabaseUnit >
{
public:
	ObjectConstructorBase::ObjectType objectType() const
						{ return DATABASE_OBJECT; }
	const char* objectClassName() const	{ return ORACLE_DB_CLASS_NAME; }
	OracleDbUnit* object( const config::NamedConfiguration& conf );
};


class OracleTransaction : public Transaction
{
public:
	OracleTransaction( OracleEnvirenment *env_, OracleDatabase& database, const std::string& name_);
	virtual ~OracleTransaction() {}
};





}} // _Wolframe::db

#endif // _ORACLE_HPP_INCLUDED
