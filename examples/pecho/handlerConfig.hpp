//
// handlerConfig.hpp
//

#ifndef _HANDLERCONFIG_HPP_INCLUDED
#define _HANDLERCONFIG_HPP_INCLUDED

#include "standardConfigs.hpp"

namespace _Wolframe {

	/// echo configuration
	struct pEchoConfiguration : public _Wolframe::config::ConfigurationBase
	{
	public:
		unsigned short		timeout;

		/// constructor
		pEchoConfiguration() : ConfigurationBase( "pEcho Server", NULL,  "pEcho Server configuration" )
											{ timeout = 0; }
		/// methods
		bool parse( const config::ConfigurationNode& pt, const std::string& node,
			    const module::ModulesDirectory* modules );
		bool check() const;
		void print( std::ostream& os, size_t indent ) const;

//		Not implemented yet, inherited from base for the time being
//		bool test() const;
	};


	/// Wolframe handler configuration structure
	struct HandlerConfiguration
	{
	public:
		pEchoConfiguration	*pechoConfig;

		/// constructor
		HandlerConfiguration()
		{
			pechoConfig = new pEchoConfiguration();
		}

		/// destructor
		~HandlerConfiguration()
		{
			if ( pechoConfig ) delete pechoConfig;
		}
	};

} // namespace _Wolframe

#endif // _HANDLERCONFIG_HPP_INCLUDED
