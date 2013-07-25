/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
// serverEndpoint.cpp
//

#include <string>
#include "serverEndpoint.hpp"
#include "utils/fileUtils.hpp"
#include "logger-v1.hpp"

#include <boost/filesystem.hpp>
#include "utils/fileUtils.hpp"

namespace _Wolframe	{
namespace net	{

#ifdef WITH_SSL
/// set SSL files path to absolute pathes
void ServerSSLendpoint::setAbsolutePath( const std::string& refPath )
{
	if ( ! m_cert.empty() )	{
		std::string oldPath = m_cert;
		m_cert = utils::getCanonicalPath( m_cert, refPath );
		if ( oldPath != m_cert )	{
/*MBa ?!?*/		LOG_NOTICE << "Using absolute SSL certificate filename '" << m_cert
				   << "' instead of '" << oldPath << "'";
		}
	}
	if ( ! m_key.empty() )	{
		std::string oldPath = m_key;
		m_key = utils::getCanonicalPath( m_key, refPath );
		if ( oldPath != m_key )	{
/*MBa ?!?*/		LOG_NOTICE << "Using absolute SSL key filename '" << m_key
				   << "' instead of '" << oldPath << "'";
		}
	}
	if ( ! m_CAdir.empty() )	{
		std::string oldPath = m_CAdir;
		m_CAdir = utils::getCanonicalPath( m_CAdir, refPath );
		if ( oldPath != m_CAdir )	{
/*MBa ?!?*/		LOG_NOTICE << "Using absolute CA directory '" << m_CAdir
				   << "' instead of '" << oldPath << "'";
		}
	}
	if ( ! m_CAchain.empty() )	{
		std::string oldPath = m_CAchain;
		m_CAchain = utils::getCanonicalPath( m_CAchain, refPath );
		if ( oldPath != m_CAchain )	{
/*MBa ?!?*/		LOG_NOTICE << "Using absolute CA chain filename '" << m_CAchain
				   << "' instead of '" << oldPath << "'";
		}
	}
}
#endif // WITH_SSL

}} // namespace _Wolframe::net

