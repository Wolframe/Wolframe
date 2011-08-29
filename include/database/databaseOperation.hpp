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
// databaseOperation.hpp
//

#ifndef _DATABASE_OPERATION_HPP_INCLUDED
#define _DATABASE_OPERATION_HPP_INCLUDED

#include "database.hpp"

namespace _Wolframe {
namespace db {

class DatabaseOperation	{
public:
	enum Operation	{
		INSERT,
		DELETE,
		UPDATE,
		SELECT
	};

	virtual ~DatabaseOperation()			{}
	virtual Operation operation() const = 0;
};

class DBinsert : public DatabaseOperation
{
public:
	virtual Operation operation()		{ return INSERT; }
};

class DBdelete : public DatabaseOperation
{
public:
	virtual Operation operation()		{ return DELETE; }
};

class DBupdate : public DatabaseOperation
{
public:
	virtual Operation operation()		{ return UPDATE; }
};

class DBselect : public DatabaseOperation
{
public:
	virtual Operation operation()		{ return SELECT; }
};


/***********************************************************************/

namespace request {

/// get a password string / digest for an user
struct UserPassword : public DatabaseRequest
{
	virtual TransactionType type() const	{ return AUTHENTICATION; }
};


/// get the client configuration
struct ClientConfiguration : public DatabaseRequest
{
	virtual TransactionType type() const	{ return CLIENT_CONFIG; }
};


/// transaction request
struct Transaction : public DatabaseRequest
{
	virtual TransactionType type() const	{ return TRANSACTION; }
};

} // namespace request

/***********************************************************************/

namespace answer {

/// get password string / digest answer
struct UserPassword : public DatabaseAnswer
{
	virtual TransactionType type() const	{ return AUTHENTICATION; }
};


/// client configuration answer
struct ClientConfiguration : public DatabaseAnswer
{
	virtual TransactionType type() const	{ return CLIENT_CONFIG; }
};


/// transaction answer
struct Transaction : public DatabaseAnswer
{
	virtual TransactionType type() const	{ return TRANSACTION; }
};

} // namespace answer

/***********************************************************************/

}} // namespace _Wolframe::db

#endif // _DATABASE_OPERATION_HPP_INCLUDED
