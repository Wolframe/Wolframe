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
// lua configuration functions
//

#include "handlerConfig.hpp"
#include "config/configurationParser.hpp"
#include "config/valueParser.hpp"
#include "logger.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "miscUtils.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <ostream>

extern "C" {
	#include "lualib.h"
	#include "lauxlib.h"
}

namespace _Wolframe {
namespace config {

template<>
bool ConfigurationParser::parse( LuaConfiguration& cfg,
				 const boost::property_tree::ptree& pt, const std::string& /*node*/,
				 const module::ModulesConfiguration* /*modules*/ )
{
	bool retVal = true;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin();
							L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "script" ))	{
			bool isDefined = ( !cfg.script.empty());
			if ( !config::Parser::getValue( cfg.logPrefix().c_str(), *L1it, cfg.script, &isDefined ))
				retVal = false;
			else	{
				if ( ! boost::filesystem::path( cfg.script ).is_absolute() )
					LOG_WARNING << cfg.logPrefix() << "script file path is not absolute: "
						    << cfg.script;
			}
		} else if ( boost::algorithm::iequals( L1it->first, "preload_lib" ))	{
			std::string preload_lib;
			if ( !config::Parser::getValue( cfg.logPrefix().c_str(), *L1it, preload_lib ))
				retVal = false;
			cfg.preload_libs.push_back( preload_lib );
		} else {
			LOG_WARNING << cfg.logPrefix() << "unknown configuration option: '"
				    << L1it->first << "'";
			return false;
		}
	}

	return retVal;
}

} // namespace config


LuaConfiguration::LuaConfiguration()
	: ConfigurationBase( "Lua Example Server", NULL, "Lua Example configuration " )
{
	LuaModuleDefinition x;
	x.moduleName = "";
	x.moduleInit = luaopen_base;
	knownLuaModules["base"] = x;
	x.moduleName = LUA_TABLIBNAME;
	x.moduleInit = luaopen_table;
	knownLuaModules[LUA_TABLIBNAME] = x;
	x.moduleName = LUA_IOLIBNAME;
	x.moduleInit = luaopen_io;
	knownLuaModules[LUA_IOLIBNAME] = x;
	x.moduleName = LUA_OSLIBNAME;
	x.moduleInit = luaopen_os;
	knownLuaModules[LUA_OSLIBNAME] = x;
	x.moduleName = LUA_STRLIBNAME;
	x.moduleInit = luaopen_string;
	knownLuaModules[LUA_STRLIBNAME] = x;
	x.moduleName = LUA_MATHLIBNAME;
	x.moduleInit = luaopen_math;
	knownLuaModules[LUA_MATHLIBNAME] = x;
	x.moduleName = LUA_DBLIBNAME;
	x.moduleInit = luaopen_debug;
	knownLuaModules[LUA_DBLIBNAME] = x;
	x.moduleName = LUA_LOADLIBNAME;
	x.moduleInit = luaopen_package;
	knownLuaModules[LUA_LOADLIBNAME] = x;
}

void LuaConfiguration::print( std::ostream& os, size_t /* indent */ ) const
{
	os << sectionName() << std::endl;
	os << "   LUA script: " << script << std::endl;
	if( !preload_libs.empty( ) ) {
		os << "       preload modules: ";
		for( std::list<std::string>::const_iterator it = preload_libs.begin( ); it != preload_libs.end( ); it++ ) {
			os << *it << " ";
		}
		os << std::endl;
	}
}


/// Check if the Lua configuration makes sense
bool LuaConfiguration::check() const
{
	bool correct = true;

	// is there a script?
	if( script.empty( ) ) {
		LOG_ERROR << "No Lua script given";
		correct = false;
	}

	// are the configured preload libraries known?
	for( std::list<std::string>::const_iterator it = preload_libs.begin( ); it != preload_libs.end( ); it++ ) {
		std::map<std::string, LuaModuleDefinition>::const_iterator it2 = knownLuaModules.find( *it );
		if( it2 == knownLuaModules.end( ) ) {
			LOG_ERROR << "Unknown LUA preload library '" << *it << "'";
			correct = false;
		}
	}

	// does the Lua script pass a syntax check?
	lua_State *l = luaL_newstate( );
	if( l ) {
		if( luaL_loadfile( l, script.c_str( ) ) ) {
			LOG_ERROR << "Syntax error in lua script: " << lua_tostring( l, -1 );
			lua_pop( l, 1 );
			correct = false;
		}
		lua_close( l );
	}

	return correct;
}

void LuaConfiguration::setCanonicalPathes( const std::string& refPath )
{
	using namespace boost::filesystem;
	if ( ! script.empty() )	{
		if ( ! path( script ).is_absolute() )
			script = resolvePath( absolute( script,
							path( refPath ).branch_path()).string());
		else
			script = resolvePath( script );
	}
}


} // namespace _Wolframe
