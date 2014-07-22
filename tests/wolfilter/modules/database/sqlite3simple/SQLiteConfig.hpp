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
// Wolframe SQLite client configuration
//

#ifndef _SQLITE_CONFIG_HPP_INCLUDED
#define _SQLITE_CONFIG_HPP_INCLUDED

#include "config/configurationBase.hpp"
#include "config/structSerialize.hpp"
#include <vector>

namespace _Wolframe {
namespace db {

static const char* SQLite_DB_CLASS_NAME = "SQLite";

struct SQLiteConfigStruct
{
	SQLiteConfigStruct();

	std::string	m_ID;
	std::string	m_filename;
	bool		m_foreignKeys;
	bool		m_profiling;
	unsigned short	m_connections;
	std::vector< std::string > m_extensionFiles;	///< list of Sqlite extension modules to load

	/// \brief Structure description for serialization/parsing
	static const serialize::StructDescriptionBase* getStructDescription();
};

/// \brief SQLite database configuration
class SQLiteConfig
	:public config::NamedConfiguration
	,public SQLiteConfigStruct
{
public:
	const char* className() const				{ return SQLite_DB_CLASS_NAME; }

	SQLiteConfig( const char* name, const char* logParent, const char* logName );
	~SQLiteConfig(){}

	bool parse( const config::ConfigurationNode& pt, const std::string& node,
		    const module::ModulesDirectory* modules );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );

	const std::string& ID() const				{ return m_ID; }
	const std::string& filename() const			{ return m_filename; }
	bool foreignKeys() const				{ return m_foreignKeys; }
	bool profiling() const					{ return m_profiling; }
	unsigned short connections() const			{ return m_connections; }
	const std::vector< std::string > extensionFiles() const	{ return m_extensionFiles; }
private:
	config::ConfigurationTree::Position m_config_pos;
};

}} // _Wolframe::db

#endif // _SQLITE_CONFIG_HPP_INCLUDED
