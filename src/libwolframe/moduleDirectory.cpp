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
#include "processor/moduleDirectory.hpp"

namespace _Wolframe {
namespace module {

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
							it != m_cfgdBuilder.end(); it++ )	{
		if ( boost::algorithm::iequals( (*it)->m_className, objectClassName ))
			return *it;
	}
	return NULL;
}

}} // namespace _Wolframe::module
