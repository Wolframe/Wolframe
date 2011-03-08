//
// handlerConfig.hpp
//

#ifndef _HANDLERCONFIG_HPP_INCLUDED
#define _HANDLERCONFIG_HPP_INCLUDED

#include "standardConfigs.hpp"

#include <list>

namespace _Wolframe {

	/// echo configuration
	struct LuaConfiguration : public _Wolframe::Configuration::ConfigurationBase
	{
	public:
		std::string		script;
		std::list<std::string>	preload_libs;

		/// constructor
		LuaConfiguration( const std::string& printName )
			: ConfigurationBase( printName ) { }
		/// methods
		bool parse( const boost::property_tree::ptree& pt, const std::string& nodeName, std::ostream& os );
		void setCanonicalPathes( const std::string& refPath );
		bool check( std::ostream& os ) const;
		void print( std::ostream& os ) const;

//			Not implemented yet, inherited from base for the time being
//			bool test( std::ostream& os ) const;
	};


	/// Wolframe handler configuration structure
	struct HandlerConfiguration
	{
	public:
		LuaConfiguration	*luaConfig;

		/// constructor
		HandlerConfiguration()
		{
			luaConfig = new LuaConfiguration( "Lua Example Server" );
		}
	};


} // namespace _Wolframe

#endif // _HANDLERCONFIG_HPP_INCLUDED
