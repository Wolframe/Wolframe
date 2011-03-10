//
// handlerConfig.hpp
//

#ifndef _HANDLERCONFIG_HPP_INCLUDED
#define _HANDLERCONFIG_HPP_INCLUDED

#include "standardConfigs.hpp"

namespace _Wolframe {

	/// Wolrame handler configuration structure
	struct HandlerConfiguration
	{
	public:
		Configuration::DatabaseConfiguration		*database;
//		Configuration::AuthenticationConfiguration	*auth;

		/// constructor
		HandlerConfiguration()
		{
			database = new Configuration::DatabaseConfiguration();
//			auth = ...
		}

		~HandlerConfiguration()
		{
			if ( database ) delete database;
//			auth = ...
		}
	};


} // namespace _Wolframe

#endif // _HANDLERCONFIG_HPP_INCLUDED
