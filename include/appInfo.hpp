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
// application wide singleton
//

#ifndef _APP_INFO_HPP_INCLUDED
#define _APP_INFO_HPP_INCLUDED

#include "version.hpp"

#if defined( _MSC_VER )
	#define WOLFRAME_EXPORT __declspec( dllexport )
#else
	#define WOLFRAME_EXPORT
#endif

namespace _Wolframe	{

class ApplicationInfo
{
public:
	~ApplicationInfo();

	WOLFRAME_EXPORT static ApplicationInfo& instance();

	WOLFRAME_EXPORT const Version& version() const;
	WOLFRAME_EXPORT void version( const Version& ver );

protected:
	ApplicationInfo();

private:
	// make it noncopyable
	ApplicationInfo( const ApplicationInfo& );
	const ApplicationInfo& operator= ( const ApplicationInfo& );

	// Real object data
	Version	m_version;
};

} // namespace _Wolframe

#endif // _APP_INFO_HPP_INCLUDED
