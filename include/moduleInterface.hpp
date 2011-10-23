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

struct ConfigDescriptionBase
{
	const char* title;
	const char* section;
	const char* keyword;
	bool (*parseFunc)( config::ConfigurationBase&,
			   const boost::property_tree::ptree&, const std::string& node,
			   const module::ModulesDirectory* modules );
public:
	ConfigDescriptionBase( const char* Title, const char* Section, const char* Keyword,
			       bool (*pf)( config::ConfigurationBase& configuration,
					   const boost::property_tree::ptree& pt,
					   const std::string& node,
					   const module::ModulesDirectory* modules ) )
		: title( Title ), section( Section), keyword( Keyword ),
		  parseFunc( pf )		{}

	virtual ~ConfigDescriptionBase()	{}

	virtual config::ObjectConfiguration* create( const char* logPrefix ) = 0;
};

template< class T >
struct ConfigurationDescription : public ConfigDescriptionBase
{
public:
	ConfigurationDescription( const char* Title, const char* Section, const char* Keyword,
				  bool (*pf)( config::ConfigurationBase& configuration,
					      const boost::property_tree::ptree& pt,
					      const std::string& node,
					      const module::ModulesDirectory* modules ) )
		: ConfigDescriptionBase( Title, Section, Keyword, pf )
	{}

	virtual ~ConfigurationDescription()	{}

	virtual config::ObjectConfiguration* create( const char* logPrefix )
					{ return new T( title, logPrefix, keyword ); }
};


class ModuleContainerBase
{
};


template < class T, class Tconf, class Tbase >
class ModuleContainer : public ObjectContainer< Tbase >, public ModuleContainerBase
{
public:
	virtual ~ModuleContainer()		{}
	virtual const char* typeName() const = 0;

	static ObjectContainer< Tbase >* create( const config::ObjectConfiguration& conf )	{
		return new T( dynamic_cast< const Tconf& >( conf ));
	}
};

template < class T >
struct ContainerDescription
{
	const char* name;
	T* ( *createFunc )( const config::ObjectConfiguration& conf );
public:
	ContainerDescription( const char* n, T* ( *f )( const config::ObjectConfiguration& conf ) )
		: name( n ), createFunc( f )	{}
};


///
class ModulesDirectory
{
public:
	ModulesDirectory()	{}
	~ModulesDirectory()	{
		for ( std::list< ConfigDescriptionBase* >::const_iterator it = m_config.begin();
									it != m_config.end();
									it++ )
			delete *it;
		for ( std::list< ModuleContainerBase* >::const_iterator it = m_container.begin();
									it != m_container.end();
									it++ )
			delete *it;
	}

	bool addConfiguration( ConfigDescriptionBase* description );
	bool addContainer( ModuleContainerBase* description );

	ConfigDescriptionBase* getConfig( const std::string& section, const std::string& keyword ) const;
	ConfigDescriptionBase* getContainer( const std::string& section, const std::string& name ) const;
private:
	std::list< ConfigDescriptionBase* >	m_config;
	std::list< ModuleContainerBase* >	m_container;
};

bool LoadModules( ModulesDirectory& modules );

}} // namespace _Wolframe::module

#endif // _MODULE_INTERFACE_HPP_INCLUDED
