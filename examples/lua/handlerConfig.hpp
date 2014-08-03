//
// handlerConfig.hpp
//

#ifndef _HANDLERCONFIG_HPP_INCLUDED
#define _HANDLERCONFIG_HPP_INCLUDED

#include "standardConfigs.hpp"

#include <list>
#include <map>

extern "C" {
	#include "lua.h"
}

namespace _Wolframe {

	typedef int (*LuaModuleEntryFunc)( lua_State *l );
	typedef struct {
		std::string moduleName;
		LuaModuleEntryFunc moduleInit;
	} LuaModuleDefinition;

	class luaConfig {
	public:
		std::string script;
		std::list<std::string> preload_libs;
		std::map<std::string, LuaModuleDefinition> knownLuaModules;
	};

	/// echo configuration
	struct LuaConfiguration : public _Wolframe::config::ConfigurationBase
	{
	public:
		std::string					script;
		std::list<std::string>				preload_libs;
		std::map<std::string, LuaModuleDefinition>	knownLuaModules;

		/// constructor
		LuaConfiguration();

		/// methods
		bool parse( const config::ConfigurationNode& pt, const std::string& node,
			    const module::ModulesDirectory* modules );
		void setCanonicalPathes( const std::string& refPath );
		bool check() const;
		void print( std::ostream& os, size_t indent ) const;

//			Not implemented yet, inherited from base for the time being
//			bool test() const;
	};


	/// Wolframe handler configuration structure
	struct HandlerConfiguration
	{
	public:
		LuaConfiguration	*luaConfig;

		/// constructor
		HandlerConfiguration()
		{
			luaConfig = new LuaConfiguration();
		}

		virtual ~HandlerConfiguration( ) {
			if ( luaConfig ) delete luaConfig;
		}
	};


} // namespace _Wolframe

#endif // _HANDLERCONFIG_HPP_INCLUDED
