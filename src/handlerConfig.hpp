//
// handlerConfig.hpp
//

#ifndef _HANDLERCONFIG_HPP_INCLUDED
#define _HANDLERCONFIG_HPP_INCLUDED

#include "standardConfigs.hpp"

namespace _Wolframe {

	/// Wolframe handler configuration structure
	struct HandlerConfiguration
	{
	public:
		Configuration::DatabaseConfiguration		*database;
		Configuration::ServiceBanner			*banner;
//		Configuration::AuthenticationConfiguration	*auth;

		/// constructor
		HandlerConfiguration()
		{
			banner = new Configuration::ServiceBanner();
			database = new Configuration::DatabaseConfiguration();

//			auth = ...
		}

		~HandlerConfiguration()
		{
			if ( banner ) delete banner;
			if ( database ) delete database;
//			auth = ...
		}
	};


} // namespace _Wolframe

#endif // _HANDLERCONFIG_HPP_INCLUDED
