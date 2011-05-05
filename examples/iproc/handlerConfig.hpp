//
// handlerConfig.hpp
//

#ifndef _HANDLERCONFIG_HPP_INCLUDED
#define _HANDLERCONFIG_HPP_INCLUDED

#include "standardConfigs.hpp"
#include "langbind/luaConfig.hpp"

namespace _Wolframe {

struct HandlerConfiguration
{
public:
	iproc::lua::Configuration *m_appConfig;

	HandlerConfiguration()
	{
		m_appConfig = new iproc::lua::Configuration( "lua iproc", "iproc");
	}

	virtual ~HandlerConfiguration( ) {
		if (m_appConfig) delete m_appConfig;
	}
};

}//namespace _Wolframe
#endif
