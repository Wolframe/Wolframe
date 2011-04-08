//
// handlerConfig.hpp
//

#ifndef _HANDLERCONFIG_HPP_INCLUDED
#define _HANDLERCONFIG_HPP_INCLUDED

#include "standardConfigs.hpp"

namespace _Wolframe {

	/// echo configuration
	struct EchoConfiguration : public _Wolframe::config::ConfigurationBase
	{
	public:
		unsigned short		timeout;

		/// constructor
		EchoConfiguration( const std::string& printHeader )
			: ConfigurationBase( printHeader )	{ timeout = 0; }
		/// methods
		bool parse( const boost::property_tree::ptree& pt, const std::string& nodeName );
		bool check() const;
		void print( std::ostream& os ) const;

//			Not implemented yet, inherited from base for the time being
//			bool test() const;
	};


	/// Wolframe handler configuration structure
	struct HandlerConfiguration
	{
	public:
		EchoConfiguration	*echoConfig;

		/// constructor
		HandlerConfiguration()
		{
			echoConfig = new EchoConfiguration( "Echo Server" );
		}

		~HandlerConfiguration()
		{
			if ( echoConfig ) delete echoConfig;
		}
	};


} // namespace _Wolframe

#endif // _HANDLERCONFIG_HPP_INCLUDED
