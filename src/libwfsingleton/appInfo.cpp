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
// Application Info is an application wide singleton
//

#include "appInfo.hpp"

#include <boost/thread/mutex.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/locks.hpp>


namespace _Wolframe	{

ApplicationInfo& ApplicationInfo::instance()
{
	static boost::scoped_ptr< ApplicationInfo >	m_t;
	static boost::mutex				m_mutex;
	static bool					m_initialized = false;

	if ( !m_initialized )	{
		boost::lock_guard< boost::mutex > lock( m_mutex );
		if ( !m_initialized )	{
			m_t.reset( new ApplicationInfo() );
			m_initialized = true;
		}
	}
	return *m_t;
}

ApplicationInfo::ApplicationInfo()
{}

ApplicationInfo::~ApplicationInfo()
{}

const Version& ApplicationInfo::version() const
{
	return m_version;
}

void ApplicationInfo::version( const Version& ver )
{
	m_version = ver;
}

} // namespace _Wolframe

