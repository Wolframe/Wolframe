//
// handlerConfig.hpp
//

#ifndef _HANDLERCONFIG_HPP_INCLUDED
#define _HANDLERCONFIG_HPP_INCLUDED

#include "standardConfigs.hpp"

namespace _SMERP {

	/// echo configuration
	struct EchoConfiguration : public _SMERP::Configuration::ConfigurationBase
	{
	public:
		unsigned short		timeout;

		/// constructor
		EchoConfiguration( const std::string& node, const std::string& header )
			: ConfigurationBase( node, header )	{ timeout = 0; }
		/// methods
		bool parse( boost::property_tree::ptree& pt, std::ostream& os );
		bool check( std::ostream& os ) const;
		void print( std::ostream& os ) const;

//			Not implemented yet, inherited from base for the time being
//			bool test( std::ostream& os ) const;
	};


	/// SMERP handler configuration structure
	struct HandlerConfiguration
	{
	public:
		EchoConfiguration	*echoConfig;

		/// constructor
		HandlerConfiguration()
		{
			echoConfig = new EchoConfiguration( "timeout", "Echo Server" );
		}
	};


} // namespace _SMERP

#endif // _HANDLERCONFIG_HPP_INCLUDED
