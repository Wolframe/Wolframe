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
		Database::DatabaseConfiguration	*database;
		Configuration::ServiceBanner	*banner;
//		AAAA::AAAAConfiguration		*auth;

		/// constructor
		HandlerConfiguration()
		{
			banner = new Configuration::ServiceBanner();
			database = new Database::DatabaseConfiguration();

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
