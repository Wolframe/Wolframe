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
			   const module::ModulesConfiguration* modules );
public:
	ConfigDescriptionBase( const char* Title, const char* Section, const char* Keyword,
			       bool (*pf)( config::ConfigurationBase& configuration,
					   const boost::property_tree::ptree& pt,
					   const std::string& node,
					   const module::ModulesConfiguration* modules ) )
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
					      const module::ModulesConfiguration* modules ) )
		: ConfigDescriptionBase( Title, Section, Keyword, pf )
	{}

	virtual ~ConfigurationDescription()	{}

	virtual config::ObjectConfiguration* create( const char* logPrefix )
					{ return new T( title, logPrefix, keyword ); }
};


template < class T, class Tconf, class Tbase >
class ModuleContainer : public Container< Tbase >
{
public:
	virtual ~ModuleContainer()		{}
	virtual const char* typeName() const = 0;

	static Container< Tbase >* create( const config::ObjectConfiguration& conf )	{
		return new T( dynamic_cast< const Tconf& >( conf ));
	}
};

template < class T, class Tconf >
struct ContainerDescription
{
	const char* name;
	T* ( *createFunc )( const Tconf& conf );
public:
	ContainerDescription( const char* n, T* ( *f )( const Tconf& conf ) )
		: name( n ), createFunc( f )	{}
};


///
class ModulesConfiguration
{
public:
	ModulesConfiguration()	{}
	~ModulesConfiguration()	{
		for ( std::list< ConfigDescriptionBase* >::const_iterator it = m_modules.begin();
									it != m_modules.end();
									it++ )
			delete *it;
	}

	bool add( ConfigDescriptionBase* description );
	ConfigDescriptionBase* get( const std::string& section, const std::string& keyword ) const;
private:
	std::list< ConfigDescriptionBase* >	m_modules;
};

bool LoadModules( ModulesConfiguration& modules );

}} // namespace _Wolframe::module

#endif // _MODULE_INTERFACE_HPP_INCLUDED
