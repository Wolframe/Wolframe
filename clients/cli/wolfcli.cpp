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

#include <iostream>
#include <cstdlib>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#ifdef WITH_SSL
#include <boost/asio/ssl.hpp>
#endif // WITH_SSL

int main( int argc, char *argv[] )
{
	if( argc != 3 ) {
		std::cerr << "Usage: wolfcli <host> <port>" << std::endl;
		return 1;
	}
	char *host = argv[1];
	int iport;
	iport = atoi( argv[2] );
	if( iport == 0 || iport == INT_MAX || iport == INT_MIN ||
            iport > USHRT_MAX ) {
		std::cerr << "Illegal port '" << argv[2] << "'!" << std::endl;
		return 1;
	}
	unsigned short port = iport;

	boost::asio::io_service io_service;
	boost::asio::ip::tcp::resolver resolver( io_service );
	boost::asio::ip::tcp::resolver::query query( host, argv[2] );
        boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve( query );
        endpoint.port( port );

	return 0;
}
