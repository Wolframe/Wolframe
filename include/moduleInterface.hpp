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
///\file moduleInterface.hpp
//

#ifndef _MODULE_INTERFACE_HPP_INCLUDED
#define _MODULE_INTERFACE_HPP_INCLUDED

#include <string>
#include <list>
#include <boost/property_tree/ptree.hpp>
#include "config/configurationBase.hpp"
#include "container.hpp"

namespace _Wolframe {
namespace module {

struct ModuleConfiguration
{
	const char* title;
	const char* section;
	const char* keyword;
	bool (*parseFunc)( config::ConfigurationBase&,
			   const boost::property_tree::ptree&, const std::string& node,
			   const module::ModulesDirectory* modules );
public:
	ModuleConfiguration( const char* Title, const char* Section, const char* Keyword,
			     bool (*pf)( config::ConfigurationBase& configuration,
					 const boost::property_tree::ptree& pt,
					 const std::string& node,
					 const module::ModulesDirectory* modules ) )
		: title( Title ), section( Section), keyword( Keyword ),
		  parseFunc( pf )		{}

	virtual ~ModuleConfiguration()		{}

	virtual config::ObjectConfiguration* create( const char* logPrefix ) = 0;
};

template< class T >
struct ConfigurationDescription : public ModuleConfiguration
{
public:
	ConfigurationDescription( const char* Title, const char* Section, const char* Keyword,
				  bool (*pf)( config::ConfigurationBase& configuration,
					      const boost::property_tree::ptree& pt,
					      const std::string& node,
					      const module::ModulesDirectory* modules ) )
		: ModuleConfiguration( Title, Section, Keyword, pf )
	{}

	virtual ~ConfigurationDescription()	{}

	virtual config::ObjectConfiguration* create( const char* logPrefix )
					{ return new T( title, logPrefix, keyword ); }
};


struct ModuleContainer
{
	const char* name;
public:
	ModuleContainer( const char* Name ) : name( Name ){}

	virtual ~ModuleContainer()		{}

	virtual Container* create( const config::ObjectConfiguration& conf ) = 0;
};

template < class T, class Tconf >
class ContainerDescription : public ModuleContainer
{
public:
	ContainerDescription( const char* Name ) :
		ModuleContainer( Name )		{}
	virtual ~ContainerDescription()		{}

	virtual Container* create( const config::ObjectConfiguration& conf )	{
		return new T( dynamic_cast< const Tconf& >( conf ));
	}
};

///
class ModulesDirectory
{
public:
	ModulesDirectory()	{}
	~ModulesDirectory()	{
		for ( std::list< ModuleConfiguration* >::const_iterator it = m_config.begin();
									it != m_config.end();
									it++ )
			delete *it;
		for ( std::list< ModuleContainer* >::const_iterator it = m_container.begin();
									it != m_container.end();
									it++ )
			delete *it;
	}

	bool addConfig( ModuleConfiguration* description );
	bool addContainer( ModuleContainer* container );

	ModuleConfiguration* getConfig( const std::string& section, const std::string& keyword ) const;
	ModuleContainer* getContainer( const std::string& name ) const;
private:
	std::list< ModuleConfiguration* >	m_config;
	std::list< ModuleContainer* >		m_container;
};

bool LoadModules( ModulesDirectory& modules );

}} // namespace _Wolframe::module

#endif // _MODULE_INTERFACE_HPP_INCLUDED
