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
// SQLite configuration parser
//

#include "SQLite.hpp"
#include "config/valueParser.hpp"
#include "config/configurationTree.hpp"
#include "serialize/struct/structDescription.hpp"
#include "utils/fileUtils.hpp"

#include <boost/algorithm/string.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

namespace _Wolframe {
namespace db {

SQLiteConfigStruct::SQLiteConfigStruct()
	:m_foreignKeys(true)
	,m_profiling(false)
	,m_connections(DEFAULT_SQLITE_CONNECTIONS)
{}

const serialize::StructDescriptionBase* SQLiteConfigStruct::getStructDescription()
{
	struct ThisDescription :public serialize::StructDescription<SQLiteConfig>
	{
	ThisDescription()
	{
		(*this)
		( "identifier", &SQLiteConfig::m_ID)		.mandatory()
		( "file", &SQLiteConfig::m_filename)		.mandatory()
		( "foreignKeys", &SQLiteConfig::m_foreignKeys)	.optional()
		( "profiling", &SQLiteConfig::m_profiling)	.optional()
		( "connections", &SQLiteConfig::m_connections)	.optional()
		( "extension", &SQLiteConfig::m_extensionFiles )
		;
	}
	};
	static const ThisDescription rt;
	return &rt;
}

bool SQLiteConfig::parse( const config::ConfigurationNode& pt, const std::string& /*node*/,
			  const module::ModulesDirectory* /*modules*/ )
{
	try
	{
		serialize::parseConfigStructure( *static_cast<SQLiteConfigStruct*>(this), pt);
		m_config_pos = pt.position();
		return true;
	}
	catch (const std::runtime_error& e)
	{
		LOG_FATAL << logPrefix() << e.what();
		return false;
	}
}

SQLiteConfig::SQLiteConfig( const char* name, const char* logParent, const char* logName )
	: config::NamedConfiguration( name, logParent, logName )
{}

void SQLiteConfig::print( std::ostream& os, size_t indent ) const
{
	std::string indStr( indent, ' ' );

	os << indStr << sectionName() << ":" << std::endl;
	if ( ! m_ID.empty() )
		os << indStr << "   ID: " << m_ID << std::endl;
	os << indStr << "   Filename: " << m_filename << std::endl;
	os << indStr << "      Referential integrity: " << (m_foreignKeys ? "enabled" : "disabled") << std::endl;
	os << indStr << "      Profiling: " << (m_profiling ? "enabled" : "disabled") << std::endl;
}

bool SQLiteConfig::check() const
{
	if ( m_filename.empty() )	{
		LOG_ERROR << logPrefix() << " " << m_config_pos.logtext() << ": SQLite database filename cannot be empty";
		return false;
	}
	return true;
}

void SQLiteConfig::setCanonicalPathes( const std::string& refPath )
{
	if ( ! m_filename.empty() )	{
		std::string oldPath = m_filename;
		m_filename = utils::getCanonicalPath( m_filename, refPath);
		if ( oldPath != m_filename )	{
/*MBa ?!?*/		LOG_NOTICE << logPrefix() << "Using absolute database filename '" << m_filename
				       << "' instead of '" << oldPath << "'";
		}
	}
}

}} // namespace _Wolframe::db
