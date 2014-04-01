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
///\brief Error class for databases
///\file database/databaseError.hpp
//
#ifndef _DATABASE_DATABASE_ERROR_HPP_INCLUDED
#define _DATABASE_DATABASE_ERROR_HPP_INCLUDED
#include "logger-v1.hpp"
#include "logger/logObject.hpp"
#include <string>
#include <cstdlib>
#include <stdexcept>

namespace _Wolframe {
namespace db {

struct DatabaseError
{
	DatabaseError( const DatabaseError& o)
		:severity(o.severity)
		,errorcode(o.errorcode)
		,dbname(o.dbname)
		,statement(o.statement)
		,functionidx(o.functionidx)
		,errorclass(o.errorclass)
		,msg(o.msg)
		,usermsg(o.usermsg)
		{}

	DatabaseError(
			log::LogLevel::Level severity_,
			int errorcode_,
			const char* dbname_,
			const char* statement_,
			const char* errorclass_,
			const char* msg_,
			const char* usermsg_,
			int functionidx_=-1)
		:severity(severity_)
		,errorcode(errorcode_)
		,dbname(dbname_?dbname_:"")
		,statement(statement_?statement_:"")
		,functionidx(functionidx_)
		,errorclass(errorclass_?errorclass_:"")
		,msg(msg_?msg_:"")
		,usermsg(usermsg_?usermsg_:"")
		{
			std::replace( usermsg.begin(), usermsg.end(), '\n', ' ');
			std::replace( msg.begin(), msg.end(), '\n', ' ');
			std::replace( statement.begin(), statement.end(), '\n', ' ');
		}

	log::LogLevel::Level severity;
	int errorcode;
	std::string dbname;
	std::string statement;
	int functionidx;
	std::string errorclass;
	std::string msg;
	std::string usermsg;

	static const log::LogObjectDescriptionBase* getLogObjectDescription()
	{
		struct Description :public log::LogObjectDescription<DatabaseError>
		{
			Description():log::LogObjectDescription<DatabaseError>( "error in database '$1' error class '$2' error code $3 ($4 in statement: $5)")
			{
				(*this)
					(&DatabaseError::dbname)
					(&DatabaseError::errorclass)
					(&DatabaseError::errorcode)
					(&DatabaseError::msg)
					(&DatabaseError::statement);
			}
		};
		static Description rt;
		return &rt;
	}
};

struct DatabaseTransactionError
{
	DatabaseTransactionError( const DatabaseTransactionError& o)
		:transaction(o.transaction)
		,severity(o.severity)
		,errorcode(o.errorcode)
		,dbname(o.dbname)
		,statement(o.statement)
		,functionidx(o.functionidx)
		,errorclass(o.errorclass)
		,msg(o.msg)
		,usermsg(o.usermsg)
		{}

	DatabaseTransactionError(
			const std::string& transaction_,
			const DatabaseError& o)
		:transaction(transaction_)
		,severity(o.severity)
		,errorcode(o.errorcode)
		,dbname(o.dbname)
		,statement(o.statement)
		,functionidx(o.functionidx)
		,errorclass(o.errorclass)
		,msg(o.msg)
		,usermsg(o.usermsg)
		{}

	std::string transaction;
	log::LogLevel::Level severity;
	int errorcode;
	std::string dbname;
	std::string statement;
	int functionidx;
	std::string errorclass;
	std::string msg;
	std::string usermsg;

	static const log::LogObjectDescriptionBase* getLogObjectDescription()
	{
		struct Description :public log::LogObjectDescription<DatabaseTransactionError>
		{
			Description():log::LogObjectDescription<DatabaseTransactionError>( "error in transaction '$1' database '$2' error class '$3' error code $4 ($5 in statement: $6)")
			{
				(*this)
					(&DatabaseTransactionError::transaction)
					(&DatabaseTransactionError::dbname)
					(&DatabaseTransactionError::errorclass)
					(&DatabaseTransactionError::errorcode)
					(&DatabaseTransactionError::msg)
					(&DatabaseTransactionError::statement);
			}
		};
		static Description rt;
		return &rt;
	}
};

struct DatabaseErrorException
	:public log::Exception<std::runtime_error,DatabaseError>
{
	DatabaseErrorException( const DatabaseError& o)
		:log::Exception<std::runtime_error,DatabaseError>( o){}
};

struct DatabaseTransactionErrorException
	:public log::Exception<std::runtime_error,DatabaseTransactionError>
{
	DatabaseTransactionErrorException( const DatabaseTransactionError& o)
		:log::Exception<std::runtime_error,DatabaseTransactionError>( o){}
};
}}
#endif
