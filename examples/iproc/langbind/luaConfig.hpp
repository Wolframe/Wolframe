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
/// \file luaConfig.hpp
/// \brief Configuration of the lua application processor
#ifndef _iproc_LUA_CONFIG_HPP_INCLUDED
#define _iproc_LUA_CONFIG_HPP_INCLUDED
#include "standardConfigs.hpp"
#include <string>
#include <vector>

extern "C" {
#include "lua.h"
}

namespace _Wolframe {
namespace iproc {
namespace lua {

/// \class Configuration
/// \brief configuration object of the lua application processor
class Configuration :public config::ConfigurationBase
{
public:
	/// \brief module load function for a lua state
	/// \param ls lua state to initialize with the load of the module for this state object
	typedef int (*ModuleLoad)( lua_State *ls);

	/// \brief constructor
	/// \param name configuration name
	/// \param prefix configuration prefix
	Configuration( const char* name, const char* prefix) :ConfigurationBase(name, NULL, prefix),m_input_bufsize(512),m_output_bufsize(512){}

	/// \brief interface implementation of ConfigurationBase::parse(const boost::property_tree::ptree&, const std::string&)
	virtual bool parse( const boost::property_tree::ptree&, const std::string&);

	/// \brief interface implementation of ConfigurationBase::setCanonicalPathes(const std::string&)
	virtual void setCanonicalPathes( const std::string&);

	/// \brief interface implementation of ConfigurationBase::test() const
	virtual bool test() const;

	/// \brief interface implementation of ConfigurationBase::check() const
	virtual bool check() const;

	/// \brief interface implementation of ConfigurationBase::print(std::ostream& os, size_t indent) const
	virtual void print( std::ostream&, size_t indent=0) const;

	/// \brief loads the configuration settings for a lua virtual machine state
	/// \param[in,out] ls lua state to initialize
	/// \return true if success, else false
	bool load( lua_State* ls) const;

	/// \class Module
	/// \brief loadable module description
	class Module
	{
	public:
		/// \enum Type
		/// \brief type of the module
		enum Type
		{
			Undefined,	///< unknown (cannot be loaded)
			Script,		///< lua script with helper functions
			PreloadLib,	///< one of the lua preload core library
			UserLib		///< a user defined library
		};
		/// \brief returns the module type 't' as descriptive string for an error or status message
		static const char* typeName( Type t)	{const char* ar[3] = {"undefined", "script", "preloaded library"}; return ar[t];}

		/// \brief Default constructor
		Module()							:m_type(Undefined),m_load(0){}
		/// \brief Constructor
		/// \param module name
		Module( const std::string& name_, Type type_=Undefined)		:m_type(type_),m_name(name_),m_load(0) {if (type_==Undefined) setType();}
		/// \brief Copy constructor
		/// \param module to copy
		Module( const Module& o)					:m_type(o.m_type),m_name(o.m_name),m_path(o.m_path),m_load(o.m_load){}

		/// \brief Canonical module file path decomposition based on the application reference path
		/// Initialize the absolute path of this module by combining the application reference path with the module file name.
		/// remove all path parts from the name. After this call the module name is the plain name abd path the full path of the module.
		/// \param[in] refPath the application reference path
		void setCanonicalPath( const std::string& refPath);

		/// \brief Determines the type of the module
		void setType();

		/// \brief Return the name of the module
		/// \return the plain name of the module
		const std::string& name() const		{return m_name;}

		/// \brief Return the full path of the module
		/// \return the full path of the module
		const std::string& path() const		{return m_path;}

		/// \brief Return the type of the module
		/// \return the type of the module
		const Type& type() const		{return m_type;}

		/// \brief Loads the configuration settings for a lua virtual machine state
		/// \param[in,out] ls lua state to initialize
		/// \return true if success, else false
		bool load( lua_State* ls) const;

		/// \brief Checks the module configuratin (does the module exist)
		/// \return true if success, else false
		virtual bool check() const;

	private:
		Type m_type;				///< type of the module
		std::string m_name;			///< name of the module
		std::string m_path;			///< full path of the module
		ModuleLoad m_load;			///< function to load the module into the interpreter context
	};
	/// \brief return the main module
	const Module& main() const			{return m_main;}
	/// \brief return the list of modules without the main
	const std::list<Module>& modules() const	{return m_modules;}

	/// \brief return size of the buffer used for input network messages in bytes
	unsigned int input_bufsize() const		{return m_input_bufsize;}
	/// \brief return size of the buffer used for output network messages in bytes
	unsigned int output_bufsize() const		{return m_output_bufsize;}

	/// \brief Return the name of the function to execute for a command
	/// \param[in] protocolcmd command from the protocol
	/// \return script function name to execute
	const char* scriptFunctionName( const char*) const		{return "run_echo";}

	/// \brief Tell wheter the command has IO (protocol command content) or not
	/// \param[in] protocolcmd command from the protocol
	/// \return true, if the command has IO, false, if not
	bool scriptFunctionHasIO( const char*) const			{return true;}
private:
	Module m_main;					///< main module
	std::list<Module> m_modules;			///< list of modules without the main
	unsigned int m_input_bufsize;			///< size of input network message buffers in bytes
	unsigned int m_output_bufsize;			///< size of output network message buffers in bytes
};

}}}//namespace
#endif

