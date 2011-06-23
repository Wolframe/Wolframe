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
// databaseContainer.hpp
//

#ifndef _DATABASE_CONTAINER_HPP_INCLUDED
#define _DATABASE_CONTAINER_HPP_INCLUDED

#include "config/configurationBase.hpp"
#include "database.hpp"

namespace _Wolframe {
namespace db {

class DatabaseConfig : public config::ModuleConfiguration
{
public:
	DatabaseConfig( const char* name, const char* logParent, const char* logName )
		: ModuleConfiguration( name, logParent, logName )	{}
	virtual ~DatabaseConfig()			{}

	void ID( const std::string& id )		{ m_ID = id; }
	const std::string& ID() const			{ return m_ID; }
private:
	std::string		m_ID;
};

class DatabaseContainer
{
public:
	virtual ~DatabaseContainer()			{}

//	virtual DatabaseContainer* create( const DatabaseConfig* conf ) const = 0;

	virtual const std::string& ID() const = 0;
	virtual const char* typeName() const = 0;
	virtual const Database& database() = 0;
};

}} // namespace _Wolframe::db

#endif // _DATABASE_CONTAINER_HPP_INCLUDED
