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
#include "luaConfig.hpp"
#include "luaDebug.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include <ostream>
#include "config/valueParser.hpp"
#include "miscUtils.hpp"

extern "C" {
	#include <lualib.h>
	#include <lauxlib.h>
	#include <lua.h>
}

namespace _Wolframe {
namespace config {

template<>
bool ConfigurationParser::parse( _Wolframe::iproc::lua::Configuration& cfg,
				 const boost::property_tree::ptree& parentNode, const std::string& /*node*/ )
{
	using namespace _Wolframe::iproc::lua;

	std::string name;
	unsigned int cnt_main = 0;

	for ( boost::property_tree::ptree::const_iterator it = parentNode.begin(); it != parentNode.end(); it++)
	{
		if (boost::algorithm::iequals( it->first, "main"))
		{
			if ( !config::Parser::getValue( cfg.logPrefix().c_str(), *it, name, config::Parser::NonEmptyDomain<std::string>())) return false;
			cfg.m_main = Configuration::Module( name, Configuration::Module::Script);
			cnt_main ++;
		}
		else if (boost::algorithm::iequals( it->first, "module"))
		{
			if ( !config::Parser::getValue( cfg.logPrefix().c_str(), *it, name, config::Parser::NonEmptyDomain<std::string>())) return false;
			cfg.m_modules.push_back( Configuration::Module( name));
		}
		else if (boost::algorithm::iequals( it->first, "input_buffer"))
		{
			if (!config::Parser::getValue( cfg.logPrefix().c_str(), *it, cfg.m_input_bufsize, config::Parser::RangeDomain<unsigned int>(1,(1<<20)))) return false;
		}
		else if (boost::algorithm::iequals( it->first, "output_buffer"))
		{
			if (!config::Parser::getValue( cfg.logPrefix().c_str(), *it, cfg.m_output_bufsize, config::Parser::RangeDomain<unsigned int>(1,(1<<20)))) return false;
		}
		else if (boost::algorithm::iequals( it->first, "cstacksize"))
		{
			if (!config::Parser::getValue( cfg.logPrefix().c_str(), *it, cfg.m_cthread_stacksize, config::Parser::RangeDomain<unsigned int>(64,(1<<20)))) return false;
		}
		else
		{
			LOG_WARNING << cfg.logPrefix() << ": unknown configuration option: '" << it->first << "'";
		}
	}
	if (cnt_main == 0)
	{
		LOG_ERROR << cfg.logPrefix() << ": main script to execute is not defined (configuration option <module>";
		return false;
	}
	return true;
}

}} // namespace _Wolframe::config


using namespace _Wolframe::iproc::lua;

bool Configuration::parse( const boost::property_tree::ptree& pt, const std::string& node )
{
	return config::ConfigurationParser::parse( *this, pt, node );
}

static Configuration::ModuleLoad getLuaModuleEntryFunc( const char* name)
{
	if (strcmp(name,LUA_TABLIBNAME) == 0) return luaopen_table;
	if (strcmp(name,LUA_IOLIBNAME) == 0) return luaopen_io;
	if (strcmp(name,LUA_OSLIBNAME) == 0) return luaopen_os;
	if (strcmp(name,LUA_STRLIBNAME) == 0) return luaopen_string;
	if (strcmp(name,LUA_MATHLIBNAME) == 0) return luaopen_math;
	if (strcmp(name,LUA_DBLIBNAME) == 0) return luaopen_debug;
	if (strcmp(name,LUA_LOADLIBNAME) == 0) return luaopen_package;
	return 0;
}

void Configuration::Module::setType()
{
	if (m_type == Configuration::Module::Undefined)
	{
		m_load = getLuaModuleEntryFunc( m_name.c_str());
		if (m_load)
		{
			m_type = PreloadLib;
		}
		else
		{
			m_type = Script;
		}
	}
}

void Configuration::Module::setCanonicalPath( const std::string& refPath)
{
	if (m_type == Script)
	{
		boost::filesystem::path pt(m_name);
		if (pt.is_absolute())
		{
			m_path = resolvePath(m_name);
		}
		else
		{
			m_path = resolvePath( boost::filesystem::absolute( m_name, boost::filesystem::path( refPath).branch_path()).string());
		}
		m_name = pt.leaf().string();
	}
}

void Configuration::setCanonicalPathes( const std::string& refPath)
{
	m_main.setCanonicalPath( refPath);
	for (std::list<Module>::iterator it = m_modules.begin(); it != m_modules.end(); it++)
	{
		it->setCanonicalPath( refPath);
	}
}

static int function_printlog( lua_State *ls)
{
	/* first parameter maps to a log level, rest gets printed depending on
	 * whether it's a string or a number
	 */
	int ii,nn = lua_gettop(ls);
	if (nn <= 0)
	{
		LOG_ERROR << "no arguments passed to 'printlog'";
		return 0;
	}
	const char *logLevel = luaL_checkstring( ls, 1);
	std::string logmsg;

	for (ii=2; ii<=nn; ii++)
	{
		if (!getDescription( ls, ii, logmsg))
		{
			LOG_ERROR << "failed to map 'printLog' arguments to a string";
		}
	}
	_Wolframe::log::LogLevel::Level lv = _Wolframe::log::LogLevel::strToLogLevel( logLevel);
	if (lv == _Wolframe::log::LogLevel::LOGLEVEL_UNDEFINED)
	{
		LOG_ERROR << "'printLog' called with undefined loglevel '" << logLevel << "' as first argument";
	}
	else
	{
		_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( lv )
			<< _Wolframe::log::LogComponent::LogLua
			<< logmsg;
	}
	return 0;
}

bool Configuration::Module::load( lua_State* ls) const
{
	if (m_type == PreloadLib && m_load)
	{
		m_load(ls);
	}
	else if (m_type == UserLib)
	{
		lua_pushcfunction( ls, m_load);
		lua_pushstring( ls, m_name.c_str());
		if (lua_pcall( ls, 1, 1, 0) != 0)
		{
			LOG_ERROR << "Runtime error when loading base module " << m_name << ":" << lua_tostring( ls, -1);
			return false;
		}
	}
	else if (m_type == Script)
	{
		if (luaL_loadfile( ls, m_path.empty()?m_name.c_str():m_path.c_str()))
		{
			LOG_ERROR << "Failed to load script '" << m_name << "':" << lua_tostring( ls, -1);
			lua_pop( ls, 1);
			return false;
		}
		// register logging function already here because then it can be used in the script initilization part
		lua_pushstring( ls, "printlog");
		lua_pushcfunction( ls, &function_printlog);
		lua_settable( ls, LUA_GLOBALSINDEX);

		// call main, we may have to initialize LUA modules there
		if (lua_pcall( ls, 0, LUA_MULTRET, 0) != 0)
		{
			LOG_FATAL << "Unable to call main entry of script: " << lua_tostring( ls, -1 );
			lua_pop( ls, 1 );
			throw new std::runtime_error( "Can't initialize LUA processor" );
		}
	}
	else
	{
		LOG_WARNING << "Could not load module '" << m_name << "'";
	}
	return true;
}

bool Configuration::Module::check() const
{
	switch (m_type)
	{
		case PreloadLib:
		case UserLib:
			break;
		case Script:
			if (!boost::filesystem::exists( m_path))
			{
				LOG_ERROR << "Script " << m_name << " ( " << m_path << ") does not exist";
				return false;
			}
			break;
		case Undefined:
			break;
	}
	return true;
}

bool Configuration::load( lua_State *ls) const
{
	luaopen_base( ls);
	for (std::list<Module>::const_iterator it = m_modules.begin(); it != m_modules.end(); it++)
	{
		if (it->type() == Module::PreloadLib)
		{
			if (!it->load( ls)) return false;
		}
	}
	for (std::list<Module>::const_iterator it = m_modules.begin(); it != m_modules.end(); it++)
	{
		if (it->type() != Module::PreloadLib)
		{
			if (!it->load( ls)) return false;
		}
	}
	if (!m_main.load( ls)) return false;
	return true;
}

bool Configuration::check() const
{
	bool rt = true;
	for (std::list<Module>::const_iterator it = m_modules.begin(); it != m_modules.end(); it++)
	{
		rt &= it->check();
	}
	return rt;
}

bool Configuration::test() const
{
	lua_State *ls = luaL_newstate();
	if (!ls)
	{
		LOG_ERROR << "failed to create lua state in configuration check";
		return false;
	}
	bool rt = load( ls);
	lua_close( ls);
	return rt;
}

void Configuration::print( std::ostream& os, size_t /*indent*/) const
{
	os << "Configuration of " << logPrefix() << ":" << std::endl;
	os << "   Main Script: " << m_main.name() << " (" << m_main.path() << ")" << std::endl;

	if( !m_modules.empty())
	{
		os << "   Modules: ";
		for (std::list<Module>::const_iterator it = m_modules.begin(); it != m_modules.end(); it++)
		{
			os << "      " << Module::typeName(it->type()) << " " << it->name();
			if (it->type() == Module::Script)
			{
				os << " (" << it->path() << ")";
			}
			os << std::endl;
		}
		os << std::endl;
	}
}


