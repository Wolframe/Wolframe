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
#include "serialize/configSerialize.hpp"
#include "serialize/descriptiveConfiguration.hpp"
#include <vector>

namespace _Wolframe {
namespace db {

static const char* SQLITE_DB_CLASS_NAME = "SQLite";
enum {DEFAULT_SQLITE_CONNECTIONS = 4};

/// \brief SQLite database configuration
class SQLiteConfig
	:public _Wolframe::serialize::DescriptiveConfiguration
{
public:
	const char* className() const				{ return SQLITE_DB_CLASS_NAME; }

	SQLiteConfig()
		:_Wolframe::serialize::DescriptiveConfiguration(SQLITE_DB_CLASS_NAME, "database", "sqlite", getStructDescription())
		,m_foreignKeys(true)
		,m_profiling(false)
		,m_connections(DEFAULT_SQLITE_CONNECTIONS)
	{
		setBasePtr( (void*)this); // ... mandatory to set pointer to start of configuration
	}

	SQLiteConfig( const std::string& id_, const std::string& filename_,
			bool foreignKeys_, bool profiling_,
			unsigned short connections_,
			const std::vector<std::string>& extensionFiles_ )
		:_Wolframe::serialize::DescriptiveConfiguration(SQLITE_DB_CLASS_NAME, "database", "sqlite", getStructDescription())
		,m_ID(id_)
		,m_filename(filename_)
		,m_foreignKeys(foreignKeys_)
		,m_profiling(profiling_)
		,m_connections(connections_)
		,m_extensionFiles(extensionFiles_){}

	SQLiteConfig( const char* title, const char* logprefix)
		:_Wolframe::serialize::DescriptiveConfiguration( title, "database", logprefix, getStructDescription())
		,m_foreignKeys(true)
		,m_profiling(false)
		,m_connections(DEFAULT_SQLITE_CONNECTIONS)
	{
		setBasePtr( (void*)this); // ... mandatory to set pointer to start of configuration
	}

	virtual bool check() const;
	virtual void print( std::ostream& os, size_t indent ) const;
	virtual void setCanonicalPathes( const std::string& referencePath );

	const std::string& ID() const				{ return m_ID; }
	const std::string& filename() const			{ return m_filename; }
	bool foreignKeys() const				{ return m_foreignKeys; }
	bool profiling() const					{ return m_profiling; }
	unsigned short connections() const			{ return m_connections; }
	const std::vector< std::string > extensionFiles() const	{ return m_extensionFiles; }

public:
	/// \brief Structure description for serialization/parsing
	static const serialize::StructDescriptionBase* getStructDescription();

private:
	std::string	m_ID;
	std::string	m_filename;
	bool		m_foreignKeys;
	bool		m_profiling;
	unsigned short	m_connections;
	std::vector< std::string > m_extensionFiles;	///< list of Sqlite extension modules to load

	config::ConfigurationTree::Position m_config_pos;
};

}} // _Wolframe::db

#endif // _SQLITE_CONFIG_HPP_INCLUDED
