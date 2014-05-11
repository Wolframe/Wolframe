/************************************************************************

 Copyright (C) 2011 - 2014 Project Wolframe.
 All rights reserved.

 This file is part of Project Wolframe.

 Commercial Usage
    Licensees holding valid Project Wolframe Commercial licenses may
    use this file in accordance with the Project Wolframe
    Commercial License Agreement provided with the Software or,
    alternatively, in accordance with the terms contained
    in a written agreement between the licensee and Project Wolframe.

 GNU General Public License Usage
    Alternatively, you can redistribute this file and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Wolframe is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
#ifndef _WOLFRAME_SERVICE_HPP_INCLUDED
#define _WOLFRAME_SERVICE_HPP_INCLUDED

#include "appProperties.hpp"
#include "wolframedCommandLine.hpp"
#include "wolframeHandler.hpp"
#include "standardConfigs.hpp"
#include "service.hpp"
#include "server.hpp"
#include "wolframe.hpp"
#include "version.hpp"
#include "appInfo.hpp"
#include "appConfig.hpp"
#include "module/moduleInterface.hpp"
#include "module/moduleDirectory.hpp"
#include <boost/thread.hpp>

namespace _Wolframe
{

class WolframeAppProperties
	:public AppProperties
{
public:
	virtual const char* applicationName() const		{ return "Wolframe"; }
	virtual const Version applicationVersion() const	{ return Version(
										  WOLFRAME_MAJOR_VERSION,
										  WOLFRAME_MINOR_VERSION,
										  WOLFRAME_REVISION
										  ); }

	virtual const char* defaultMainConfig() const		{ return "/etc/wolframe.conf"; }
	virtual const char* defaultUserConfig() const		{ return "~/wolframe.conf"; }
	virtual const char* defaultLocalConfig() const		{ return "./wolframe.conf"; }

	virtual unsigned short defaultTCPport() const		{ return 7660; }
	virtual unsigned short defaultSSLport() const		{ return 7960; }

	virtual const char* defaultServiceName() const		{ return "wolframe"; }
#if defined( _WIN32 )
	const char* defaultServiceDisplayName() const		{ return "Wolframe Daemon"; }
	const char* defaultServiceDescription() const		{ return "a daemon for wolframeing"; }
#endif // defined( _WIN32 )
};


class WolframeService
	:public ServiceInterface
{
public:
	explicit WolframeService( const AppProperties* appProperties_)
		:m_appProperties(appProperties_)
		,m_appInfo(&ApplicationInfo::instance())
		,m_appConfig(appProperties_)
		,m_handler(0)
		,m_server(0)
		,m_thread(0){}

	virtual ~WolframeService()
	{
		stop();
	}

	virtual bool loadConfig( int argc, char* argv[]);

	const AppProperties* appProperties() const
	{
		return m_appProperties;
	}

	virtual log::LoggerConfiguration& loggerConfig()
	{
		return *m_appConfig.loggerCfg;
	}

	virtual bool start();
	virtual void stop();

	virtual bool configcheck() const
	{
		return m_appConfig.check();
	}

	virtual bool configtest() const
	{
		//PF:REMARK: Not implemented yet: 'return m_appConfig.test();'
		return m_appConfig.check();
	}

	virtual void printconfig( std::ostream& os ) const
	{
		m_appConfig.print( os);
	}

	virtual const char* command() const
	{
		return config::CmdLineConfig::commandName( m_cmdLineCfg.command);
	}

	virtual bool foreground() const
	{
		return m_appConfig.foreground;
	}

	virtual const char* pidfile() const
	{
		return m_appConfig.serviceCfg->pidFile.c_str();
	}

	virtual const char* user() const
	{
		return m_appConfig.serviceCfg->user.empty()?0:m_appConfig.serviceCfg->user.c_str();
	}

	virtual const char* group() const
	{
		return m_appConfig.serviceCfg->group.empty()?0:m_appConfig.serviceCfg->group.c_str();
	}

private:
	const AppProperties* m_appProperties;
	ApplicationInfo* m_appInfo;
	module::ModulesDirectory m_modDir;
	config::CmdLineConfig m_cmdLineCfg;
	config::ApplicationConfiguration m_appConfig;
	ServerHandler* m_handler;
	net::server* m_server;
	boost::thread* m_thread;
};

}//namespace
#endif


