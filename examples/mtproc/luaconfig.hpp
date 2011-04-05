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
/// \file luaconfig.hpp
/// \brief Configuration of the lua application processor
#ifndef _MTPROC_LUA_CONFIG_HPP_INCLUDED
#define _MTPROC_LUA_CONFIG_HPP_INCLUDED
#include "standardConfigs.hpp"
#include <string>
#include <vector>

extern "C" {
#include "lua.h"
}

namespace _Wolframe {
namespace mtproc {
namespace lua {

class LuaConfiguration :public Configuration::ConfigurationBase
{
public:
	/// \brief constructor
	/// \param name configuration base name
	LuaConfiguration( const std::string& name ) :ConfigurationBase(name){}

	/// \brief interface implementation of ConfigurationBase::parse(const boost::property_tree::ptree&, const std::string&)
	virtual bool parse( const boost::property_tree::ptree&, const std::string&);

	/// \brief interface implementation of ConfigurationBase::setCanonicalPathes(const std::string&)
	virtual void setCanonicalPathes( const std::string&);

	/// \brief interface implementation of ConfigurationBase::check() const
	virtual bool check() const;

	/// \brief interface implementation of ConfigurationBase::print(std::ostream& os) const
	virtual void print( std::ostream&);

	/// \brief loads the configuration settings for the lua virtual machine
	/// \param[in,out] ls lua state to initialize
	/// \return true if success, else false
	bool load( lua_State* ls) const;

	/// \class Module
	/// \brief loadable module description
	class Module
	{
	public:
		enum Type
		{
			Undefined, Script, PreloadLib
		};
		static const char* typeName( Type t)	{const char* ar[3] = {"undefined", "script", "preloaded library"}; return ar[t];}

		/// \brief constructor
		Module(){}
		/// \brief constructor
		/// \param module name
		Module( const std::string& name_)	:m_type(Undefined),m_name(name_){}
		/// \brief copy constructor
		/// \param module to copy
		Module( const Module& o)		:m_name(o.m_name),m_path(o.m_path){}

		/// \brief Canonical module file path decomposition based on the application reference path
		/// Initialize the absolute path of this module by combining the application reference path with the module file name.
		/// remove all path parts from the name. After this call the module name is the plain name abd path the full path of the module.
		/// \param[in] refPath the application reference path
		void setCanonicalPath( const std::string& refPath);

		/// \brief return the name of the module
		/// \return the plain name of the module
		const std::string& name() const		{return m_name;}

		/// \brief return the full path of the module
		/// \return the full path of the module
		const std::string& path() const		{return m_path;}

		/// \brief return the type of the module
		/// \return the type of the module
		const Type& type() const		{return m_type;}

	private:
		Type m_type;				///< type of the module
		std::string m_name;			///< name of the module
		std::string m_path;			///< full path of the module
	};
	/// \brief return the main module
	const Module& main() const			{return m_main;}
	/// \brief return the list of modules without the main
	const std::list<Module>& modules() const	{return m_modules;}
private:
	Module m_main;					///< main module
	std::list<Module> m_modules;			///< list of modules without the main
};

}}}//namespace
#endif

