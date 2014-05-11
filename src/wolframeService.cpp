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
#include "wolframeService.hpp"
#include "logger-v1.hpp"

using namespace _Wolframe;

bool WolframeService::start()
{
	// Run server in background thread(s).
	try
	{
		m_handler = new _Wolframe::ServerHandler( m_appConfig.handlerCfg, &m_modDir);
		m_server = new _Wolframe::net::server( m_appConfig.serverCfg, *m_handler );
		m_thread = new boost::thread( boost::bind( &_Wolframe::net::server::run, m_server));
	}
	catch (const std::bad_alloc&)
	{
		LOG_FATAL << "out of memory starting service";
		return false;
	}
	catch (const std::exception& e)
	{
		LOG_FATAL << "failed to start service: " << e.what();
		return false;
	}
	return true;
}

void WolframeService::stop()
{
	if (m_server)
	{
		m_server->stop();
	}
	if (m_thread)
	{
		m_thread->join();
		delete m_thread;
	}
	if (m_handler)
	{
		delete m_handler;
		m_handler = 0;
	}
	if (m_server)
	{
		delete m_server;
		m_server = 0;
	}
}

