//
// handlerConfig.hpp
//

#ifndef _HANDLERCONFIG_HPP_INCLUDED
#define _HANDLERCONFIG_HPP_INCLUDED

#include "standardConfigs.hpp"

namespace _Wolframe {

	/// echo configuration
	struct pEchoConfiguration : public _Wolframe::Configuration::ConfigurationBase
	{
	public:
		unsigned short		timeout;

		/// constructor
		pEchoConfiguration( const std::string& printName )
			: ConfigurationBase( printName )	{ timeout = 0; }
		/// methods
		bool parse( const boost::property_tree::ptree& pt, const std::string& nodeName, std::ostream& os );
		bool check( std::ostream& os ) const;
		void print( std::ostream& os ) const;

//			Not implemented yet, inherited from base for the time being
//			bool test( std::ostream& os ) const;
	};


	/// Wolframe handler configuration structure
	struct HandlerConfiguration
	{
	public:
		pEchoConfiguration	*pechoConfig;

		/// constructor
		HandlerConfiguration()
		{
			pechoConfig = new pEchoConfiguration( "pEcho Server" );
		}
	};


} // namespace _Wolframe

#endif // _HANDLERCONFIG_HPP_INCLUDED
