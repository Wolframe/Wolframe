/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
#include "config/ConfigurationTree.hpp"

#include <boost/algorithm/string.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "utils/miscUtils.hpp"

using namespace _Wolframe::utils;

namespace _Wolframe {
namespace db {

bool SQLiteConfig::parse( const config::ConfigurationTree& pt, const std::string& /*node*/,
			  const module::ModulesDirectory* /*modules*/ )
{
	using namespace _Wolframe::config;
	bool retVal = true;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "identifier" ))	{
			bool isDefined = ( !m_ID.empty() );
			std::string id;
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, id, &isDefined ))
				retVal = false;
			else
				m_ID = id;
		}
		else if ( boost::algorithm::iequals( L1it->first, "file" ) ||
			  boost::algorithm::iequals( L1it->first, "filename" ))	{
			bool isDefined = ( !filename.empty() );
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, filename, &isDefined ))
				retVal = false;
			else	{
				if ( ! boost::filesystem::path( filename ).is_absolute() )
					MOD_LOG_WARNING << logPrefix() << "database file path is not absolute: "
						    << filename;
			}
		}
		else if ( boost::algorithm::iequals( L1it->first, "flag" ))	{
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, flag, Parser::BoolDomain() ))
				retVal = false;
		}
		else	{
			MOD_LOG_WARNING << logPrefix() << "unknown configuration option: '"
				    << L1it->first << "'";
		}
	}
	return retVal;
}

SQLiteConfig::SQLiteConfig( const char* name, const char* logParent, const char* logName )
	: config::ObjectConfiguration( name, logParent, logName )
{
	flag = false;
}

void SQLiteConfig::print( std::ostream& os, size_t indent ) const
{
	std::string indStr( indent, ' ' );

	os << indStr << sectionName() << ":" << std::endl;
	if ( ! m_ID.empty() )
		os << indStr << "   ID: " << m_ID << std::endl;
	os << indStr << "   Filename: " << filename << std::endl;
	os << indStr << "   Flags: " << (flag ? "True Flag" : "False Flag") << std::endl;
}

bool SQLiteConfig::check() const
{
	if ( filename.empty() )	{
		MOD_LOG_ERROR << logPrefix() << "SQLite database filename cannot be empty";
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

}} // namespace _Wolframe::db
