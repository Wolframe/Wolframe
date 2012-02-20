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
#include "object.hpp"

namespace _Wolframe {
namespace module {

class ObjectBuilder
{
	friend class ModulesDirectory;
protected:
	const char* m_name;
public:
	ObjectBuilder( const char* name )
		: m_name( name )		{}

	virtual ~ObjectBuilder()		{}

	virtual Object* object() = 0;
};

class ContainerBuilder
{
	friend class ModulesDirectory;
protected:
	const char* m_title;
	const char* m_section;
	const char* m_keyword;
	const char* m_name;
public:
	ContainerBuilder( const char* title, const char* section, const char* keyword,
			 const char* name )
		: m_title( title ), m_section( section), m_keyword( keyword ),
		  m_name( name ){}

	virtual ~ContainerBuilder()		{}

	virtual config::ObjectConfiguration* configuration( const char* logPrefix ) = 0;
	virtual Container* container( const config::ObjectConfiguration& conf ) = 0;
};

template < class T, class Tconf >
class ContainerDescription : public ContainerBuilder
{
public:
	ContainerDescription( const char* title, const char* section, const char* keyword,
			      const char* name )
		: ContainerBuilder( title, section, keyword, name )	{}
	virtual ~ContainerDescription()		{}

	virtual config::ObjectConfiguration* configuration( const char* logPrefix ){
		return new Tconf( m_title, logPrefix, m_keyword );
	}
	virtual Container* container( const config::ObjectConfiguration& conf ){
		return new T( dynamic_cast< const Tconf& >( conf ));
	}
};

///
class ModulesDirectory
{
public:
	ModulesDirectory()	{}
	~ModulesDirectory()	{}

	bool addContainer( ContainerBuilder* container );
	bool addObject( ObjectBuilder* object );

	ContainerBuilder* getContainer( const std::string& section, const std::string& keyword ) const;
	ContainerBuilder* getContainer( const std::string& name ) const;

private:
	std::list< ContainerBuilder* >	m_container;
	std::list< ObjectBuilder* >	m_object;
};


bool LoadModules( ModulesDirectory& modDir, std::list< std::string >& modFiles );


//*********** Module interface *********

enum ModuleType	{
	CONTAINER_MODULE = 1
};

struct ModuleEntryPoint
{
	enum	SignSize	{
		MODULE_SIGN_SIZE = 16
	};

	char signature[MODULE_SIGN_SIZE];
	unsigned short ifaceVersion;
	ModuleType moduleType;
	const char* name;
	ContainerBuilder* (*create)();
	void (*setLogger)(void*);
public:
	ModuleEntryPoint( unsigned short iVer, ModuleType modType, const char* modName,
			  ContainerBuilder* (*createFunc)(),
			  void (*setLoggerFunc)(void*))
		: ifaceVersion( iVer ), moduleType( modType ), name( modName ),
		  create( createFunc ), setLogger( setLoggerFunc )
	{
		std::memcpy ( signature, "Wolframe Module", MODULE_SIGN_SIZE );
	}

};

#if !defined(_WIN32)	// POSIX module loader
	extern "C" ModuleEntryPoint	entryPoint;
#else
	extern "C" __declspec( dllexport ) ModuleEntryPoint entryPoint;
#endif

}} // namespace _Wolframe::module

#endif // _MODULE_INTERFACE_HPP_INCLUDED
