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
///
/// \file AAAAinformation.hpp
/// Header file for the objects used for AAAA information.
///

#include "connectionEndpoint.hpp"

#ifndef _AAAA_OBJECTS_HPP_INCLUDED
#define _AAAA_OBJECTS_HPP_INCLUDED

namespace _Wolframe {
namespace AAAA {

/// Base class for AAAA information objects.
class Information
{
public:
	enum Type	{
		CONNECTION,			///< object is connection infromation
		LOGIN,				///< object is login information
		LOGOUT,				///< object is logout information
		TRANSACTION			///< object is a transaction information
	};

	virtual Type type() const = 0;
};

/// Connection intiation info for AAAA purposes
struct ConnectInfo : public Information
{
	const net::LocalEndpoint&	local;	///< local endpoint info
	const net::RemoteEndpoint&	remote;	///< remote endpoint info

	ConnectInfo( const net::LocalEndpoint& lcl, const net::RemoteEndpoint& rmt )
		: local( lcl ), remote( rmt )	{}

	Information::Type type() const	{ return CONNECTION; }
};

}} // namespace _Wolframe::AAAA

#endif // _AAAA_OBJECTS_HPP_INCLUDED
