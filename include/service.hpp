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
#ifndef _WOLFRAME_SERVICE_INTERFACE_HPP_INCLUDED
#define _WOLFRAME_SERVICE_INTERFACE_HPP_INCLUDED

#include "appProperties.hpp"
#include "standardConfigs.hpp"
#include <iostream>

namespace _Wolframe
{
class ServiceInterface
{
public:
	virtual ~ServiceInterface(){}

	virtual bool loadConfig( int argc, char* argv[])=0;

	virtual const _Wolframe::AppProperties* appProperties() const=0;

	virtual log::LoggerConfiguration& loggerConfig()=0;

	virtual bool start()=0;
	virtual void stop()=0;

	virtual bool configcheck() const=0;
	virtual bool configtest() const=0;
	virtual void printconfig( std::ostream& os ) const=0;

	virtual bool foreground() const=0;
	virtual const char* pidfile() const=0;

	virtual const char* user() const=0;
	virtual const char* group() const=0;
	
	virtual const char* command() const=0;
};


int serviceMain( ServiceInterface* service, int argc, char* argv[]);

}//namespace
#endif

