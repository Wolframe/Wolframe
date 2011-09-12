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
// DBproviderImplementation.hpp
//

#ifndef _DB_PROVIDER_IMPLEMENTATION_HPP_INCLUDED
#define _DB_PROVIDER_IMPLEMENTATION_HPP_INCLUDED

#include <list>
#include "config/configurationBase.hpp"
#include "database/databaseProvider.hpp"
#include "container.hpp"

namespace _Wolframe {
namespace db {

/// database configuration
class DBproviderConfig : public config::ConfigurationBase
{
	friend class DatabaseProvider_Implementation;
	friend class config::ConfigurationParser;
public:
	/// constructor & destructor
	DBproviderConfig() : ConfigurationBase( "Database(s)", NULL, "Database configuration" )	{}
	~DBproviderConfig();

	/// methods
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	virtual void setCanonicalPathes( const std::string& referencePath );
private:
	std::list< config::TypedConfiguration* >	m_dbConfig;
};

///
///
class DatabaseProvider_Implementation
{
public:
	DatabaseProvider_Implementation( const DBproviderConfig* conf );
	~DatabaseProvider_Implementation();

	const Database* database( const std::string& ID );
private:
	std::list< Container< DatabaseUnit >* >	m_db;
};

}} // namespace _Wolframe::db

#endif // _DB_PROVIDER_IMPLEMENTATION_HPP_INCLUDED
