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
// Wolframe PostgreSQL client view
//

#include "database/database.hpp"

#ifndef _POSTGRESQL_HPP_INCLUDED
#define _POSTGRESQL_HPP_INCLUDED

namespace _Wolframe {
namespace db {

/// PostgreSQL server connection configuration
class PostgreSQLconfig : public DatabaseConfig
{
	friend class config::ConfigurationParser;
public:
	const char* type() const			{ return "PostgreSQL"; }

	PostgreSQLconfig( const char* name, const char* logParent, const char* logName );
	~PostgreSQLconfig()				{}

	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
private:
	std::string	host;
	unsigned short	port;
	std::string	dbName;
	std::string	user;
	std::string	password;
	unsigned short	connections;
	unsigned short	acquireTimeout;
};

class PostgreSQLDBcontainer : public Database
{
public:
	PostgreSQLDBcontainer( const PostgreSQLconfig* conf );
	~PostgreSQLDBcontainer()			{}
	const char* type() const			{ return "PostgreSQL"; }
private:
};

}} // _Wolframe::db

#endif // _POSTGRESQL_HPP_INCLUDED
