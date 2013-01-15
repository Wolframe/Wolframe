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
#include "utils/miscUtils.hpp"
#include "logger-v1.hpp"

#include <boost/filesystem.hpp>
#include "utils/miscUtils.hpp"

using namespace _Wolframe::utils;

namespace _Wolframe	{
namespace net	{

#ifdef WITH_SSL
/// set SSL files path to absolute pathes
void ServerSSLendpoint::setAbsolutePath( const std::string& refPath )
{
	if ( ! cert_.empty() )	{
		std::string oldPath = cert_;
		cert_ = utils::getCanonicalPath( cert_, refPath );
		if ( oldPath != cert_ )	{
/*MBa ?!?*/		LOG_NOTICE << "Using absolute SSL certificate filename '" << cert_
				   << "' instead of '" << oldPath << "'";
		}
	}
	if ( ! key_.empty() )	{
		std::string oldPath = key_;
		key_ = utils::getCanonicalPath( key_, refPath );
		if ( oldPath != key_ )	{
/*MBa ?!?*/		LOG_NOTICE << "Using absolute SSL key filename '" << key_
				   << "' instead of '" << oldPath << "'";
		}
	}
	if ( ! CAdir_.empty() )	{
		std::string oldPath = CAdir_;
		CAdir_ = utils::getCanonicalPath( CAdir_, refPath );
		if ( oldPath != CAdir_ )	{
/*MBa ?!?*/		LOG_NOTICE << "Using absolute CA directory '" << CAdir_
				   << "' instead of '" << oldPath << "'";
		}
	}
	if ( ! CAchain_.empty() )	{
		std::string oldPath = CAchain_;
		CAchain_ = utils::getCanonicalPath( CAchain_, refPath );
		if ( oldPath != CAchain_ )	{
/*MBa ?!?*/		LOG_NOTICE << "Using absolute CA chain filename '" << CAchain_
				   << "' instead of '" << oldPath << "'";
		}
	}
}
#endif // WITH_SSL

}} // namespace _Wolframe::net

