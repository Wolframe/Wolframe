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
//
// application properties
//

#ifndef _APP_PROPERTIES_HPP_INCLUDED
#define _APP_PROPERTIES_HPP_INCLUDED

#include "version.hpp"

namespace _Wolframe	{

class AppProperties
{
public:
	virtual const char* applicationName() const=0;
	virtual const Version applicationVersion() const=0;

	virtual const char* defaultMainConfig() const=0;
	virtual const char* defaultUserConfig() const=0;
	virtual const char* defaultLocalConfig() const=0;

	virtual const char* defaultServiceName() const=0;
#if defined( _WIN32 )
	virtual const char* defaultServiceDisplayName() const=0;
	virtual const char* defaultServiceDescription() const=0;
#endif // defined( _WIN32 )
	virtual unsigned short defaultTCPport() const=0;
	virtual unsigned short defaultSSLport() const=0;
};
} // namespace _Wolframe

#endif // _APP_PROPERTIES_HPP_INCLUDED
