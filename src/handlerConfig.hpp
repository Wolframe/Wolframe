//
// handlerConfig.hpp
//

#ifndef _HANDLERCONFIG_HPP_INCLUDED
#define _HANDLERCONFIG_HPP_INCLUDED

#include "standardConfigs.hpp"

namespace _SMERP {

	/// SMERP handler configuration structure
	struct HandlerConfiguration
	{
	public:
		Configuration::DatabaseConfiguration	*database;

		/// constructor
		HandlerConfiguration()
		{
			database = new Configuration::DatabaseConfiguration( "database", "Database Server" );
		}
	};


} // namespace _SMERP

#endif // _HANDLERCONFIG_HPP_INCLUDED
