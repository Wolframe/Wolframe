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
// moduleDirectory.cpp
//

#include <boost/algorithm/string.hpp>
#include "logger-v1.hpp"
#include "module/moduleDirectory.hpp"
#include "module/moduleLoader.hpp"
#include "utils/fileUtils.hpp"

using namespace _Wolframe;
using namespace _Wolframe::module;

ModulesDirectory::~ModulesDirectory()
{
	while ( ! m_simpleBuilder.empty() )	{
		assert( m_simpleBuilder.front() != NULL );
		delete m_simpleBuilder.front();
		m_simpleBuilder.pop_front();
	}
// Most of the builder creation functions do not allocate an object.
// The objects are static variables in those functions so we don't delete them.
//
//	while ( ! m_cfgdBuilder.empty() )	{
//		assert( m_cfgdBuilder.front() != NULL );
//		LOG_ALERT << "Destroying builder " << m_cfgdBuilder.front()->identifier();
//		delete m_cfgdBuilder.front();
//		LOG_ALERT << "Builder for '" << m_cfgdBuilder.front()->identifier() << "' destroyed";
//		m_cfgdBuilder.pop_front();
//	}
}

std::vector<std::pair<std::string,std::string> > ModulesDirectory::getConfigurableSectionKeywords( ObjectConstructorBase::ObjectType objtype) const
{
	std::vector< std::pair<std::string,std::string> > rt;
	std::list< ConfiguredBuilder* >::const_iterator ii = m_cfgdBuilder.begin(), ee = m_cfgdBuilder.end();
	for (; ii != ee; ++ii)
	{
		if ((*ii)->objectType() == objtype)
		{
			rt.push_back( std::pair<std::string,std::string>( (*ii)->m_section, (*ii)->m_keyword));
		}
	}
	return rt;
}

bool ModulesDirectory::addBuilder( ConfiguredBuilder* builder )
{
	for ( std::list< ConfiguredBuilder* >::const_iterator it = m_cfgdBuilder.begin();
							it != m_cfgdBuilder.end(); it++ )	{
		if ( boost::algorithm::iequals( (*it)->m_section, builder->m_section ) &&
				boost::algorithm::iequals( (*it)->m_keyword, builder->m_keyword ))	{
			LOG_ALERT << "A configuration module for section '" << builder->m_section
				  << "' keyword '" << builder->m_keyword << "' already exists";
			return false;
		}
		if ( boost::algorithm::iequals( (*it)->m_className, builder->m_className ))	{
			LOG_ALERT << "A module container named '" << builder->m_className
				  << "' already exists";
			return false;
		}
	}
	m_cfgdBuilder.push_back( builder );
	LOG_DEBUG << "Module '" << builder->m_className << "' registered for section '"
		  << builder->m_section << "' keyword '" << builder->m_keyword << "'";
	return true;
}

bool ModulesDirectory::addBuilder( SimpleBuilder* builder )
{
	for ( std::list< SimpleBuilder* >::const_iterator it = m_simpleBuilder.begin();
							it != m_simpleBuilder.end(); it++ )	{
		if ( boost::algorithm::iequals( (*it)->m_className, builder->m_className ))	{
			LOG_ALERT << "A module object named '" << builder->m_className
				  << "' already exists";
			return false;
		}
	}
	m_simpleBuilder.push_back( builder );
	LOG_DEBUG << "Module object '" << builder->m_className << "' registered";
	return true;
}

ConfiguredBuilder* ModulesDirectory::getBuilder( const std::string& section,
						 const std::string& keyword ) const
{
	for ( std::list< ConfiguredBuilder* >::const_iterator it = m_cfgdBuilder.begin();
							it != m_cfgdBuilder.end(); it++ )	{
		if ( boost::algorithm::iequals( (*it)->m_keyword, keyword ) &&
				boost::algorithm::iequals( (*it)->m_section, section ))
			return *it;
	}
	return NULL;
}

ConfiguredBuilder* ModulesDirectory::getBuilder( const std::string& objectClassName ) const
{
	for ( std::list< ConfiguredBuilder* >::const_iterator it = m_cfgdBuilder.begin();
							it != m_cfgdBuilder.end(); it++ )
	{
		LOG_DATA << "Get builder of '" << objectClassName << "' test '" << (*it)->m_className << "'";
		if ( boost::algorithm::iequals( (*it)->m_className, objectClassName ))
		{
			return *it;
		}
	}
	return NULL;
}


#define DO_STRINGIFY(x)	#x
#define STRINGIFY(x)	DO_STRINGIFY(x)

#if defined( DEFAULT_MODULE_LOAD_DIR )
std::string ModulesDirectory::getAbsoluteModulePath( const std::string& moduleName, const std::string& configuredDirectory, bool useDefaultModuleDir) const
#else
std::string ModulesDirectory::getAbsoluteModulePath( const std::string& moduleName, const std::string& configuredDirectory, bool) const
#endif
{
	// Add the module extension, if not defined
	std::string moduleName_ = moduleName;
#if defined(_WIN32)
	if (utils::getFileExtension( moduleName).empty())
	{
		moduleName_.append( ".dll");
	}
#else
	if (utils::getFileExtension( moduleName).empty())
	{
		moduleName_.append( ".so");
	}
#endif
	// Building up list of module directories ascending in order of picking priority:
	std::vector<std::string> pathpriolist;
	if (!configuredDirectory.empty())
	{
		std::string configuredDirectoryAbsolute = utils::getCanonicalPath( configuredDirectory, m_confDir);
		pathpriolist.push_back( configuredDirectoryAbsolute);
	}
#if defined( DEFAULT_MODULE_LOAD_DIR )
	if (useDefaultModuleDir)
	{
		pathpriolist.push_back( std::string( STRINGIFY( DEFAULT_MODULE_LOAD_DIR )));
	}
#endif
	pathpriolist.push_back( m_confDir);

	// Find the first match of an existing file in the module path priority list:
	std::vector<std::string>::const_iterator pi = pathpriolist.begin(), pe = pathpriolist.end();
	for (; pi != pe; ++pi)
	{
		std::string modulePathAbsolute = utils::getCanonicalPath( moduleName_, *pi);
		if (utils::fileExists( modulePathAbsolute))
		{
			return modulePathAbsolute;
		}
	}

	// Return empty string, if not found
	return std::string();
}

bool module::ModulesDirectory::loadModules( const std::list< std::string >& modFiles)
{
	bool retVal = true;

	for ( std::list< std::string >::const_iterator it = modFiles.begin();
							it != modFiles.end(); it++ )
	{
		ModuleEntryPoint* entry = loadModuleEntryPoint( *it);
		if ( !entry )
		{
			LOG_ERROR << "Failed to load module '" << *it << "'";
			retVal = false;
			break;
		}
		for ( unsigned short i = 0; entry->createBuilder[ i ]; i++ )
		{
			BuilderBase* builder = entry->createBuilder[ i ]();
			SimpleBuilder* simpleBuilder = dynamic_cast<SimpleBuilder*>(builder);
			ConfiguredBuilder* configuredBuilder = dynamic_cast<ConfiguredBuilder*>(builder);
			if (configuredBuilder)
			{
				addBuilder( configuredBuilder);
			}
			else if (simpleBuilder)
			{
				addBuilder( simpleBuilder);
			}
			else
			{
				LOG_ERROR << "Unknown type of builder in module '" << entry->name << "'";
			}
		}
		LOG_DEBUG << "Module '" << entry->name << "' loaded";
	}
	return retVal;
}
