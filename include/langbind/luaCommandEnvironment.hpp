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
///\file luaCommandEnvironment.hpp
///\brief Static environment of a command as call to a lua script
#ifndef _langbind_LUA_COMMAND_ENVIRONMENT_HPP_INCLUDED
#define _langbind_LUA_COMMAND_ENVIRONMENT_HPP_INCLUDED
#include "standardConfigs.hpp"
#include "config/descriptionBase.hpp"
#include "protocol/commandHandler.hpp"
#include <string>
#include <vector>

extern "C" {
#include "lua.h"
}

namespace _Wolframe {
namespace langbind {


///\class Configuration
///\brief configuration object of a command as call to a lua script
class LuaCommandEnvironment :public protocol::CommandEnvironment
{
public:
	///\brief Module load function for a lua state
	///\param[in] ls lua state to initialize with the load of the module for this state object
	typedef int (*ModuleLoad)( lua_State *ls);

	///\brief Constructor
	LuaCommandEnvironment( const std::string& main_, const std::string& mainmodule_, const std::vector<std::string>& modules_=std::vector<std::string>())
		:m_main(main_)
		,m_mainmodule( mainmodule_, Module::Script)
	{
		std::vector<std::string>::const_iterator itr=modules_.begin(),end=modules_.end();
		for (;itr!=end; ++itr)
		{
			m_modules.push_back( Module( *itr));
		}
	}

	///\brief Interface implementation of ConfigurationBase::test() const
	virtual bool test() const;

	///\brief Interface implementation of ConfigurationBase::check() const
	virtual bool check() const;

	///\brief Interface implementation of ConfigurationBase::print(std::ostream& os, size_t indent) const
	virtual void print( std::ostream&, size_t indent=0) const;

	///\brief Loads the configuration settings for a lua virtual machine state
	///\param[in,out] ls lua state to initialize
	///\return true if success, else false
	bool load( lua_State* ls) const;

	///\class Module
	///\brief Loadable module description
	class Module
	{
	public:
		///\enum Type
		///\brief Type of the module
		enum Type
		{
			Undefined,	///< unknown (cannot be loaded)
			Script,		///< lua script with helper functions
			UserLib		///< a library
		};
		///\brief Get the module type 't' as descriptive string for an error or status message
		///\return the module type
		static const char* typeName( Type t)	{const char* ar[3] = {"undefined", "script", "preloaded library"}; return ar[t];}

		///\brief Constructor
		Module()							:m_type(Undefined),m_load(0){}
		///\brief Constructor
		///\param[in] name_ module name
		///\param[in] type_ type of the module
		Module( const std::string& name_, Type type_=Undefined)		:m_type(type_),m_name(name_),m_load(0) {if (type_==Undefined) setType();}
		///\brief Copy constructor
		///\param[in] o module to copy
		Module( const Module& o)					:m_type(o.m_type),m_name(o.m_name),m_load(o.m_load){}

		///\brief Determines the type of the module
		void setType();

		///\brief Get the name of the module
		///\return the plain name of the module
		const std::string& name() const		{return m_name;}

		///\brief Get the type of the module
		///\return type of the module
		const Type& type() const		{return m_type;}

		///\brief Loads the configuration settings for a lua virtual machine state
		///\param[in,out] ls lua state to initialize
		///\return true if success, else false
		bool load( lua_State* ls) const;

		///\brief Checks the module configuratin (does the module exist)
		///\return true if success, else false
		virtual bool check() const;

	private:
		Type m_type;				//< type of the module
		std::string m_name;			//< name of the module
		ModuleLoad m_load;			//< function to load the module into the interpreter context
	};
	///\brief Get the main function
	///\return main function
	const std::string& main() const			{return m_main;}
	///\brief Get the main module
	///\return main module
	const Module& mainmodule() const		{return m_mainmodule;}
	///\brief Get the list of modules without the main
	///\return list of modules without the main
	const std::vector<Module>& modules() const	{return m_modules;}

private:
	std::string m_main;				//< main function
	Module m_mainmodule;				//< main module
	std::vector<Module> m_modules;			//< list of modules without the main
};

}}//namespace
#endif

