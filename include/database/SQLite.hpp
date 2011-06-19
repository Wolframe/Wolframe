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
// Wolframe SQLite client view
//

#include "database/database.hpp"

#ifndef _SQLITE_HPP_INCLUDED
#define _SQLITE_HPP_INCLUDED

namespace _Wolframe {
namespace db {

/// SQLite database configuration
class SQLiteConfig : public DatabaseConfig
{
	friend class config::ConfigurationParser;
public:
	const char* type() const			{ return "SQLite"; }

	SQLiteConfig( const char* name, const char* logParent, const char* logName );
	~SQLiteConfig()					{}

	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	virtual void setCanonicalPathes( const std::string& referencePath );
private:
	std::string	filename;
	bool		flag;
};

class SQLiteDBcontainer : public Database
{
public:
	SQLiteDBcontainer( const SQLiteConfig* conf );
	~SQLiteDBcontainer();

	const char* type() const			{ return "SQLite"; }
};

}} // _Wolframe::db

#endif // _SQLITE_HPP_INCLUDED
