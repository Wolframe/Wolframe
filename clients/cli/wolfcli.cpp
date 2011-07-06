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

namespace {

class WolfClient
{
	private:
		boost::asio::ip::tcp::socket m_socket;
		boost::asio::deadline_timer m_deadline_timer;
		boost::asio::streambuf m_input_buffer;

	public:
		WolfClient( boost::asio::io_service& io_service )
			: m_socket( io_service ), m_deadline_timer( io_service )
		{
		}

		void start( boost::asio::ip::tcp::resolver::iterator endpoint_iter )
		{
			start_connect( endpoint_iter );

			m_deadline_timer.async_wait( boost::bind( &WolfClient::check_deadline, this ) );
		}

	private:
		void start_connect( boost::asio::ip::tcp::resolver::iterator endpoint_iter )
		{
			m_deadline_timer.expires_from_now( boost::posix_time::seconds( 60 ) );

			m_socket.async_connect( endpoint_iter->endpoint( ),
				boost::bind( &WolfClient::handle_connect, this, _1, endpoint_iter ) );
		}

		void handle_connect( const boost::system::error_code &ec, boost::asio::ip::tcp::resolver::iterator endpoint_iter )
		{
			if( ec ) {
				m_socket.close( );
				std::cerr << "Connect error: " << ec.message( ) << std::endl;
				return;
			}

			std::cerr << "Connected to " << endpoint_iter->endpoint( ) << std::endl;

			start_read( );
			start_write( );
		}

		void start_read( )
		{
			m_deadline_timer.expires_from_now( boost::posix_time::seconds( 30 ) );
			boost::asio::async_read_until( m_socket, m_input_buffer, '\n',
				boost::bind( &WolfClient::handle_read, this, _1 ) );
		}

		void handle_read( const boost::system::error_code &ec )
		{
			if( ec ) {
				std::cerr << "Error reading: " << ec.message( ) << std::endl;
				return;
			}

			std::string line;
			std::istream is( &m_input_buffer );
			std::getline( is, line );

			if( !line.empty( ) ) {
				std::cout << line << std::endl;
			}

			start_read( );
		}

		void start_write( )
		{
			boost::asio::async_write( m_socket, boost::asio::buffer( "\n", 1 ),
				boost::bind( &WolfClient::handle_write, this, _1 ) );
		}

		void handle_write( const boost::system::error_code &ec )
		{
			if( ec ) {
				std::cerr << "Error writing: " << ec.message( ) << std::endl;
				return;
			}
		}

		void check_deadline( )
		{
			if( m_deadline_timer.expires_at( ) <= boost::asio::deadline_timer::traits_type::now( ) ) {
				std::cerr << "Timeout on read.. terminating connection" << std::endl;
				m_socket.close( );
				m_deadline_timer.expires_at( boost::posix_time::pos_infin );
				return;
			}

			m_deadline_timer.async_wait( boost::bind( &WolfClient::check_deadline, this ) );
		}
};

}

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
	WolfClient c( io_service );
	c.start( resolver.resolve( boost::asio::ip::tcp::resolver::query( host, argv[2] ) ) );
	io_service.run( );
//	c.start( boost::asio::ip::tcp::resolver::query query( host, argv[2] );

//        boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve( query );
  //      endpoint.port( port );

	return 0;
}
