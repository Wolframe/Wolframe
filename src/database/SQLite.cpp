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
// Wolframe SQLite client view implementation
//

#include "logger.hpp"
#include "SQLite.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "miscUtils.hpp"

#include "sqlite3.h"

namespace _Wolframe {
namespace db {

//***  SQLite configuration functions  **************************************
SQLiteConfig::SQLiteConfig( const char* name, const char* logParent, const char* logName )
	: ModuleConfiguration< SQLiteConfig, DatabaseConfig >( name, logParent, logName )
{
	flag = false;
}

void SQLiteConfig::print( std::ostream& os, size_t indent ) const
{
	std::string indStr( indent, ' ' );

	os << indStr << sectionName() << ":" << std::endl;
	if ( ! ID().empty() )
		os << indStr << "   ID: " << ID() << std::endl;
	os << indStr << "   Filename: " << filename << std::endl;
	os << indStr << "   Flags: " << (flag ? "True Flag" : "False Flag") << std::endl;
}

bool SQLiteConfig::check() const
{
	if ( filename.empty() )	{
		LOG_ERROR << logPrefix() << "SQLite database filename cannot be empty";
		return false;
	}
	return true;
}

void SQLiteConfig::setCanonicalPathes( const std::string& refPath )
{
	using namespace boost::filesystem;

	if ( ! filename.empty() )	{
		if ( ! path( filename ).is_absolute() )
			filename = resolvePath( absolute( filename,
							  path( refPath ).branch_path()).string());
		else
			filename = resolvePath( filename );
	}
}


//***  SQLite database functions  *******************************************
SQLiteDatabase::SQLiteDatabase(  const std::string& id,
				 const std::string& filename, bool flag )
	: m_ID( id ), m_filename( filename ), m_flag( flag )
{
	LOG_NOTICE << "SQLite database '" << m_ID << "' created with "
		   << "filename '" << m_filename << "'";
}

SQLiteDatabase::~SQLiteDatabase( )
{
}

//***  SQLite database container  *******************************************
SQLiteContainer::SQLiteContainer( const SQLiteConfig& conf )
	: m_db( conf.ID(), conf.filename, conf.flag )
{
	LOG_NOTICE << "SQLite database container for '" << conf.ID() << "' created";
}

SQLiteContainer::~SQLiteContainer( )
{
}

}} // _Wolframe::db


