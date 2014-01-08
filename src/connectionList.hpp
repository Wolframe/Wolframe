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
//\file system/connectionList.hpp
//\brief Interface to list of connections
#ifndef _CONNECTION_LIST_HPP_INCLUDED
#define _CONNECTION_LIST_HPP_INCLUDED
#include "system/syncCounter.hpp"
#include "system/syncObjectList.hpp"
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace net {

template <class CONNECTION, class DESCRIPTION>
class ConnectionList
{
public:
	typedef CONNECTION* (*CreateConnectionF)( const DESCRIPTION& descr);
	typedef boost::shared_ptr<CONNECTION> ConnectionR;
	typedef system::SyncObjectList<ConnectionR>::Handle ConnectionHandle;

	ConnectionList( unsigned int maxNofConnections, CreateConnectionF createConn_, system::SyncCounter* globalcounter_)
		:m_globalcounter(globalcounter_)
		,m_counter( maxNofConnections)
		,m_createConn(createConn_){}

	ConnectionHandle createConnection( const DESCRIPTION& descr)
	{
		system::SyncCounter::ScopedAquire gs( *m_globalcounter);
		{
			if (!gs.entered()) throw std::runtime_error( "too many connections");

			system::SyncCounter::ScopedAquire ls( m_counter);
			{
				if (!ls.entered()) throw std::runtime_error( "too many connections");

				ConnectionR conn( m_createConn( descr));
				m_list.insert( conn);
				ls.done();
			}
			gs.done();
		}
	}
private:
	system::SyncCounter* m_globalcounter;
	system::SyncCounter m_counter;
	system::SyncObjectList<ConnectionR> m_list;
	CreateConnectionF m_createConn;
};
}}
#endif


