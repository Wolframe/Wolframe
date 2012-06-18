/************************************************************************

 Copyright (C) 2011 Project Wolframe.
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
#include "miscUtils.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "miscUtils.hpp"

using namespace _Wolframe::utils;

namespace _Wolframe	{
	namespace net	{

#ifdef WITH_SSL
/// set SSL files path to absolute pathes
void ServerSSLendpoint::setAbsolutePath( const std::string& refPath )
{
	if ( ! cert_.empty() )	{
		if ( ! boost::filesystem::path( cert_ ).is_absolute() )
			cert_ = resolvePath( boost::filesystem::absolute( cert_,
							boost::filesystem::path( refPath ).branch_path()).string());
		else
			cert_ = resolvePath( cert_ );
	}
	if ( ! key_.empty() )	{
		if ( ! boost::filesystem::path( key_ ).is_absolute() )
			key_ = resolvePath( boost::filesystem::absolute( key_,
							boost::filesystem::path( refPath ).branch_path()).string());
		else
			key_ = resolvePath( key_ );
	}
	if ( ! CAdir_.empty() )	{
		if ( ! boost::filesystem::path( CAdir_ ).is_absolute() )
			CAdir_ = resolvePath( boost::filesystem::absolute( CAdir_,
							boost::filesystem::path( refPath ).branch_path()).string());
		else
			CAdir_ = resolvePath( CAdir_ );
	}
	if ( ! CAchain_.empty() )	{
		if ( ! boost::filesystem::path( CAchain_ ).is_absolute() )
			CAchain_ = resolvePath( boost::filesystem::absolute( CAchain_,
							boost::filesystem::path( refPath ).branch_path()).string());
		else
			CAchain_ = resolvePath( CAchain_);
	}
}
#endif // WITH_SSL

	} // namespace net
} // namespace _Wolframe

