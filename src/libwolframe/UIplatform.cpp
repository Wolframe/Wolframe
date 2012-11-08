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

#include "UIplatform.hpp"

#include <boost/algorithm/string.hpp>

namespace _Wolframe	{
namespace UI	{

Platform::Platform( const std::string& str )
{
	if ( boost::algorithm::iequals( str, "Desktop" ) )		m_platform = PLATFORM_DESKTOP;
	else if ( boost::algorithm::iequals( str, "Linux" ) )		m_platform = PLATFORM_LINUX;
	else if ( boost::algorithm::iequals( str, "Windows" ) )		m_platform = PLATFORM_WINDOWS;
	else if ( boost::algorithm::iequals( str, "MacOS" ) )		m_platform = PLATFORM_MACOS;
	else if ( boost::algorithm::iequals( str, "Android" ) )		m_platform = PLATFORM_ANDROID;
	else if ( boost::algorithm::iequals( str, "iOS" ) )		m_platform = PLATFORM_IOS;
	else if ( boost::algorithm::iequals( str, "Windows Mobile" ) )	m_platform = PLATFORM_WINDOWS_MOBILE;
	else if ( boost::algorithm::iequals( str, "Web" ) )		m_platform = PLATFORM_WEB;
	else m_platform = PLATFORM_UNDEFINED;
}

const char* Platform::name() const
{
	switch( m_platform )	{
		case PLATFORM_DESKTOP:		return "Desktop";
		case PLATFORM_LINUX:		return "Linux";
		case PLATFORM_WINDOWS:		return "Windows";
		case PLATFORM_MACOS:		return "MacOS";
		case PLATFORM_ANDROID:		return "Android";
		case PLATFORM_IOS:		return "iOS";
		case PLATFORM_WINDOWS_MOBILE:	return "Windows Mobile";
		case PLATFORM_WEB:		return "Web";
	}
	return "Undefined";
}

}} // namespace _Wolframe::UI
