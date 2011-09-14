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
// database.hpp - Wolframe base database class
//

#ifndef _DATABASE_HPP_INCLUDED
#define _DATABASE_HPP_INCLUDED

#include <string>

namespace _Wolframe {
namespace db {

enum TransactionType	{
	AUTHENTICATION,
	AUTHORIZATION,
	CLIENT_CONFIG,
	TRANSACTION
};


/// base class for database request
class DatabaseRequest
{
public:
	virtual ~DatabaseRequest()			{}
	virtual TransactionType type() const = 0;
};

/// base class for database answer
class DatabaseAnswer
{
public:
	virtual ~DatabaseAnswer()			{}
	virtual TransactionType type() const = 0;
};


/// Base class for database interface
/// All databases should provide this interface
class Database
{
public:
	virtual ~Database()				{}

	/// Database identification.
	/// All databases must have an identifier as they are referenced using this identifier.
	/// The identifier must be unique (of course).
	virtual const std::string& ID() const = 0;

	/// Database type (module type).
	/// All database implementations need a type (name).
	/// Type names must be unique.
	virtual const char* typeName() const = 0;

	/// Perform a database transaction.
	virtual bool doTransaction( DatabaseRequest& request, DatabaseAnswer& answer,
				    unsigned short timeout, unsigned short retries ) = 0;
	/* NOTE */
	/* There should be a connection to the auditing system somewhere */

	/* we should also decide how we handle db encoding
	virtual const char* encoding() const = 0; */

	/// Close the database connetion
	/// This exists for no good reason (mostly to make code look uniform)
	virtual void close()	{}
};


/// Database Unit
/// This is the base class for database unit implementations
class DatabaseUnit
{
public:
	virtual ~DatabaseUnit()			{}

	virtual const Database& database() const = 0;
};

}} // namespace _Wolframe::db

#endif // _DATABASE_HPP_INCLUDED
