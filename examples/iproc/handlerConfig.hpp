//
// handlerConfig.hpp
//

#ifndef _HANDLERCONFIG_HPP_INCLUDED
#define _HANDLERCONFIG_HPP_INCLUDED

#include "standardConfigs.hpp"
#if WITH_LUA
#include "langbind/luaconfig.hpp"
namespace _Wolframe {
struct AppConfiguration :public _Wolframe::iproc::lua::Configuration
{
	AppConfiguration() :_Wolframe::iproc::lua::Configuration( "lua iproc", "iproc") {}
};
}//namespace
#else
#error No application processor language defined (Lua)
#endif

namespace _Wolframe {

struct HandlerConfiguration
{
public:
	AppConfiguration *m_config;

	HandlerConfiguration()
	{
		m_config = new AppConfiguration();
	}

	virtual ~HandlerConfiguration( ) {
		if (m_config) delete m_config;
	}
};

}//namespace _Wolframe
#endif
