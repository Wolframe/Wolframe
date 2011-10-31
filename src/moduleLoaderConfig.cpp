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
// module loader configuration
//

#include "moduleLoaderConfig.hpp"
#include "config/valueParser.hpp"
#include "config/ConfigurationTree.hpp"
#include "logger-v1.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "miscUtils.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <list>
#include <string>
#include <ostream>

namespace _Wolframe {
namespace config {

/// Parse the configuration
bool ModuleLoaderConfiguration::parse( const ConfigurationTree& pt, const std::string& /*node*/,
				       const module::ModulesDirectory* /*modules*/ )
{
	bool retVal = true;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "module" ))	{
			std::string* modFile = new std::string;
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, *modFile ))	{
				retVal = false;
				delete modFile;
			}
			else	{
				if ( ! boost::filesystem::path( *modFile ).is_absolute() )
					LOG_WARNING << logPrefix() << " file path is not absolute: "
						    << *modFile;
				bool isDuplicate = false;
				for ( std::list< std::string* >::const_iterator it = m_moduleFile.begin();
										it != m_moduleFile.end(); it++ )	{
					if ( boost::algorithm::iequals( **it, *modFile ))	{
						LOG_ERROR << "duplicate module file: '" << *modFile << "'";
						retVal = false;
						isDuplicate = true;
						delete modFile;
					}
				}
				if ( ! isDuplicate )
					m_moduleFile.push_back( modFile );
			}
		}
		else	{
			LOG_WARNING << logPrefix() << " unknown configuration option: '"
				    << L1it->first << "'";
		}
	}

	return retVal;
}


ModuleLoaderConfiguration::~ModuleLoaderConfiguration()
{
	for ( std::list< std::string* >::const_iterator it = m_moduleFile.begin();
							it != m_moduleFile.end(); it++ )	{
		assert( !(*it)->empty());
		delete *it;
	}
}

// Server configuration functions
void ModuleLoaderConfiguration::print( std::ostream& os, size_t /* indent */ ) const
{
	os << sectionName() << std::endl;
	for ( std::list< std::string* >::const_iterator it = m_moduleFile.begin();
							it != m_moduleFile.end(); it++ )
		os << "   " << **it << std::endl;
}


/// Check if the server configuration makes sense
bool ModuleLoaderConfiguration::check() const
{
	bool retVal = true;
	for ( std::list< std::string* >::const_iterator it1 = m_moduleFile.begin();
						      it1 != m_moduleFile.end(); it1++ )	{
		std::list< std::string* >::const_iterator it2 = it1;
		it2++;
		for ( ; it2 != m_moduleFile.end(); it2++ )
			if ( boost::algorithm::iequals( **it1, **it2 ))	{
				LOG_ERROR << "duplicate module file: '" << **it1 << "'";
				retVal = false;
			}
	}

	return retVal;
}

void ModuleLoaderConfiguration::setCanonicalPathes( const std::string& refPath )
{
	using namespace boost::filesystem;

	for ( std::list< std::string* >::const_iterator it = m_moduleFile.begin();
							it != m_moduleFile.end(); it++ )	{
		assert( ! (*it)->empty() );
		if ( ! path( **it ).is_absolute() )
			**it = resolvePath( absolute( **it,
						      path( refPath ).branch_path()).string());
		else
			**it = resolvePath( **it );
	}
}

}} // namespace _Wolframe::config

