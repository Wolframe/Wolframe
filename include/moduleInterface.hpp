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
///
/// \file moduleInterface.hpp
///

#ifndef _MODULE_INTERFACE_HPP_INCLUDED
#define _MODULE_INTERFACE_HPP_INCLUDED

#include <string>
#include <list>
#include <boost/property_tree/ptree.hpp>
#include "config/configurationBase.hpp"
#include "container.hpp"

namespace _Wolframe {
namespace module {

///
class ContainerBuilder
{
	friend class ModulesDirectory;
protected:
	const char* m_identifier;
public:
	ContainerBuilder( const char* name )
		: m_identifier( name )			{}

	virtual ~ContainerBuilder()			{}

	const char* identifier() const			{ return m_identifier; }

	virtual Container* object() = 0;
};

///
class ConfiguredContainerBuilder
{
	friend class ModulesDirectory;
protected:
	const char* m_title;
	const char* m_section;		///< configuration section to which it reacts
	const char* m_keyword;		///< configuration keyword (element)
	const char* m_identifier;	///< identifier of the builder
public:
	ConfiguredContainerBuilder( const char* title, const char* section, const char* keyword,
				    const char* name )
		: m_title( title ), m_section( section), m_keyword( keyword ),
		  m_identifier( name )			{}

	virtual ~ConfiguredContainerBuilder()		{}

	const char* identifier() const			{ return m_identifier; }

	virtual config::NamedConfiguration* configuration( const char* logPrefix ) = 0;
	virtual Container* container( const config::NamedConfiguration& conf ) = 0;
};

///
template < class T, class Tconf >
class ConfiguredContainerDescription : public ConfiguredContainerBuilder
{
public:
	ConfiguredContainerDescription( const char* title, const char* section,
					const char* keyword, const char* name )
		: ConfiguredContainerBuilder( title, section, keyword, name )
	{}

	virtual ~ConfiguredContainerDescription()	{}

	virtual config::NamedConfiguration* configuration( const char* logPrefix )	{
		return new Tconf( m_title, logPrefix, m_keyword );
	}
	virtual Container* container( const config::NamedConfiguration& conf )	{
		return new T( dynamic_cast< const Tconf& >( conf ));
	}
};

///
class ModulesDirectory
{
public:
	ModulesDirectory()				{}
	~ModulesDirectory();

	bool addBuilder( ConfiguredContainerBuilder* container );
	bool addBuilder( ContainerBuilder* container );

	ConfiguredContainerBuilder* getContainer( const std::string& section, const std::string& keyword ) const;
	ConfiguredContainerBuilder* getContainer( const std::string& identifier ) const;
	ContainerBuilder* getObject( const std::string& identifier ) const;

	class container_iterator
	{
		friend class ModulesDirectory;
	public:
		container_iterator()			{}
		container_iterator( const container_iterator& it )
			: m_it( it.m_it )		{}

		ContainerBuilder* operator->() const	{ return *m_it; }
		ContainerBuilder* operator*() const	{ return *m_it; }
		container_iterator& operator++()	{ ++m_it; return *this; }
		container_iterator operator++( int )	{ container_iterator rtrn( *this ); ++m_it; return rtrn; }
		bool operator == ( const container_iterator& rhs )
							{ return m_it == rhs.m_it; }
		bool operator != ( const container_iterator& rhs )
							{ return m_it != rhs.m_it; }

	private:
		std::list< ContainerBuilder* >::const_iterator	m_it;

		container_iterator( const std::list< ContainerBuilder* >::const_iterator& it )
			: m_it( it )			{}
	};

	container_iterator objectsBegin() const		{ return container_iterator( m_container.begin() ); }
	container_iterator objectsEnd() const		{ return container_iterator( m_container.end() ); }

private:
	std::list< ConfiguredContainerBuilder* >	m_cfgdContainer; ///< list of configurable containers
	std::list< ContainerBuilder* >			m_container;	 ///< list of simple containers
};


bool LoadModules( ModulesDirectory& modDir, const std::list< std::string >& modFiles );


//*********** Module interface *********

enum ModuleObjectType	{
	MODULE_CONTAINER = 1,
	MODULE_OBJECT = 2
};

typedef ConfiguredContainerBuilder* (*createCfgContainerFunc)();
typedef ContainerBuilder* (*createContainerFunc)();

struct ModuleEntryPoint
{
	enum	SignSize	{
		MODULE_SIGN_SIZE = 16
	};

	char signature[MODULE_SIGN_SIZE];
	unsigned short ifaceVersion;
	const char* name;
	void (*setLogger)(void*);
	unsigned short		cfgdContainers;		///< number of configured containers
	createCfgContainerFunc	*createCfgdContainer;	///< vector of creation functions for configured containers
	unsigned short		containers;		///< number of simple (unconfigured) containers
	createContainerFunc	*createContainer;	///< vector of creation functions for simple containers
public:
	ModuleEntryPoint( unsigned short iVer, const char* modName,
			  void (*setLoggerFunc)(void*),
			  unsigned short nrContainers, createCfgContainerFunc* containerFunc,
			  unsigned short nrObjects, createContainerFunc* objectFunc
			  )
		: ifaceVersion( iVer ), name( modName ),
		  setLogger( setLoggerFunc ),
		  cfgdContainers( nrContainers ), createCfgdContainer( containerFunc ),
		  containers( nrObjects ), createContainer( objectFunc )
	{
		std::memcpy ( signature, "Wolframe Module", MODULE_SIGN_SIZE );
		if ( createCfgdContainer == NULL ) cfgdContainers = 0;
		if ( createContainer == NULL ) containers = 0;
	}

};

#if !defined(_WIN32)	// POSIX module loader
extern "C" ModuleEntryPoint	entryPoint;
#else
extern "C" __declspec( dllexport ) ModuleEntryPoint entryPoint;
#endif

}} // namespace _Wolframe::module

#endif // _MODULE_INTERFACE_HPP_INCLUDED
