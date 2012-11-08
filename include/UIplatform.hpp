/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
// User interface platforms
//

#ifndef _USER_INTERFACE_PLATFORM_HPP_INCLUDED
#define _USER_INTERFACE_PLATFORM_HPP_INCLUDED

#include <string>

namespace _Wolframe	{
namespace UI	{

class Platform
{
public:
	enum PlatformType	{
		PLATFORM_DESKTOP,
		PLATFORM_LINUX,
		PLATFORM_WINDOWS,
		PLATFORM_MACOS,
		PLATFORM_ANDROID,
		PLATFORM_IOS,
		PLATFORM_WINDOWS_MOBILE,
		PLATFORM_WEB,
		PLATFORM_UNDEFINED
	};

	Platform( const std::string& str );

	const char* name() const;

private:
	PlatformType	m_platform;
};

}} // namespace _Wolframe::UI

#endif // _USER_INTERFACE_PLATFORM_HPP_INCLUDED
