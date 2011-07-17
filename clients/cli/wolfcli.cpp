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

#include <cstring>

#include <boost/program_options.hpp>

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#ifdef WITH_SSL
#include <boost/asio/ssl.hpp>
#endif // WITH_SSL

namespace {

class WolfClient
{
	public:
		virtual void start( boost::asio::ip::tcp::resolver::iterator endpoint_iter ) = 0;
		virtual void stop( ) = 0;
		virtual void write( const char *s ) = 0;
};

#ifdef WITH_SSL
class SSLWolfClient : public WolfClient
{
	private:
		boost::asio::io_service& m_io_service;
		boost::asio::ssl::stream<boost::asio::ip::tcp::socket> m_socket;
		boost::asio::deadline_timer m_deadline_timer;
		boost::asio::streambuf m_input_buffer;
		std::string m_output_buffer;
		unsigned short m_connect_timeout;
		unsigned short m_read_timeout;

	public:
		SSLWolfClient(	boost::asio::io_service& io_service,
			  	boost::asio::ssl::context& ctx,
				unsigned short connect_timeout,
				unsigned short read_timeout )
			: m_io_service( io_service ),
			  m_socket( io_service, ctx ), m_deadline_timer( io_service ),
			  m_connect_timeout( connect_timeout ),
			  m_read_timeout( read_timeout )
		{
		}

		void start( boost::asio::ip::tcp::resolver::iterator endpoint_iter )
		{
			start_connect( endpoint_iter );

			m_deadline_timer.async_wait( boost::bind( &SSLWolfClient::check_deadline, this ) );
		}

		void stop( )
		{
		}

		void write( const char *s )
		{
		}

	private:
		void start_connect( boost::asio::ip::tcp::resolver::iterator endpoint_iter )
		{
			m_deadline_timer.expires_from_now( boost::posix_time::seconds( m_connect_timeout ) );

			m_socket.lowest_layer( ).async_connect( endpoint_iter->endpoint( ),
				boost::bind( &SSLWolfClient::handle_connect, this, _1, endpoint_iter ) );
		}

		void handle_connect( const boost::system::error_code &ec, boost::asio::ip::tcp::resolver::iterator endpoint_iter )
		{
			if( ec ) {
				m_socket.lowest_layer( ).close( );
				m_io_service.stop( );
				std::cerr << "Connect error: " << ec.message( ) << " (" << ec.value( ) << ")" << std::endl;
				return;
			}

			std::cerr << "Connected to " << endpoint_iter->endpoint( ) << std::endl;

			m_deadline_timer.expires_from_now( boost::posix_time::seconds( m_connect_timeout ) );

			m_socket.async_handshake( boost::asio::ssl::stream_base::client,
				boost::bind(&SSLWolfClient::handle_handshake, this, boost::asio::placeholders::error ) );
		}

		void handle_handshake( const boost::system::error_code &ec )
		{
			if( ec ) {
				m_socket.lowest_layer( ).close( );
				m_io_service.stop( );
				std::cerr << "Handshake error: " << ec.message( ) << " (" << ec.value( ) << ")" << std::endl;
				return;
			}

			std::cerr << "SSL handshake succeeded" << std::endl;

			start_read( );
		}

		void check_deadline( )
		{
			if( m_deadline_timer.expires_at( ) <= boost::asio::deadline_timer::traits_type::now( ) ) {
				std::cerr << "Timeout on read.. terminating connection" << std::endl;
				m_socket.lowest_layer( ).close( );
				m_deadline_timer.expires_at( boost::posix_time::pos_infin );
				return;
			}

			m_deadline_timer.async_wait( boost::bind( &SSLWolfClient::check_deadline, this ) );
		}

		void start_read( )
		{
			m_deadline_timer.expires_from_now( boost::posix_time::seconds( m_read_timeout ) );
			boost::asio::async_read_until( m_socket, m_input_buffer, '\n',
				boost::bind( &SSLWolfClient::handle_read, this, _1 ) );
		}

		void handle_read( const boost::system::error_code &ec )
		{
			// EOF is ok in certain conditions (for instance after having stopped to
			// receive data from stdin)
			// TODO: if ec.value( ) == EOF(code?) -> return;
			if( ec ) {
				m_io_service.stop( );
				std::cerr << "Read error: " << ec.message( ) << " (" << ec.value( ) << ")" << std::endl;
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
};
#endif

class PlainWolfClient : public WolfClient
{
	private:
		boost::asio::io_service& m_io_service;
		boost::asio::ip::tcp::socket m_socket;
		boost::asio::deadline_timer m_deadline_timer;
		boost::asio::streambuf m_input_buffer;
		std::string m_output_buffer;
		unsigned short m_connect_timeout;
		unsigned short m_read_timeout;

	public:
		PlainWolfClient(	boost::asio::io_service& io_service,
					unsigned short connect_timeout,
					unsigned short read_timeout )
			: m_io_service( io_service ),
			  m_socket( io_service ), m_deadline_timer( io_service ),
			  m_connect_timeout( connect_timeout ),
			  m_read_timeout( read_timeout )
		{
		}

		void start( boost::asio::ip::tcp::resolver::iterator endpoint_iter )
		{
			start_connect( endpoint_iter );

			m_deadline_timer.async_wait( boost::bind( &PlainWolfClient::check_deadline, this ) );
		}

		void stop( )
		{
			// TODO: send outstanding network data!
			m_io_service.stop( );
		}

		void write( const char *s )
		{
			m_output_buffer.append( s );
			m_output_buffer.append( "\n" );

			start_write( );
		}

	private:
		void start_connect( boost::asio::ip::tcp::resolver::iterator endpoint_iter )
		{
			m_deadline_timer.expires_from_now( boost::posix_time::seconds( m_connect_timeout ) );

			m_socket.async_connect( endpoint_iter->endpoint( ),
				boost::bind( &PlainWolfClient::handle_connect, this, _1, endpoint_iter ) );
		}

		void handle_connect( const boost::system::error_code &ec, boost::asio::ip::tcp::resolver::iterator endpoint_iter )
		{
			if( ec ) {
				m_socket.close( );
				m_io_service.stop( );
				std::cerr << "Connect error: " << ec.message( ) << " (" << ec.value( ) << ")" << std::endl;
				return;
			}

			std::cerr << "Connected to " << endpoint_iter->endpoint( ) << std::endl;

			start_read( );
		}

		void start_read( )
		{
			m_deadline_timer.expires_from_now( boost::posix_time::seconds( m_read_timeout ) );
			boost::asio::async_read_until( m_socket, m_input_buffer, '\n',
				boost::bind( &PlainWolfClient::handle_read, this, _1 ) );
		}

		void handle_read( const boost::system::error_code &ec )
		{
			// EOF is ok in certain conditions (for instance after having stopped to
			// receive data from stdin)
			// TODO: if ec.value( ) == EOF(code?) -> return;
			if( ec ) {
				m_io_service.stop( );
				std::cerr << "Read error: " << ec.message( ) << " (" << ec.value( ) << ")" << std::endl;
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
			boost::asio::write( m_socket, boost::asio::buffer( m_output_buffer,
				m_output_buffer.size( ) ) );

			m_output_buffer.clear( );
		}

		void check_deadline( )
		{
			if( m_deadline_timer.expires_at( ) <= boost::asio::deadline_timer::traits_type::now( ) ) {
				std::cerr << "Timeout on read.. terminating connection" << std::endl;
				m_socket.close( );
				m_deadline_timer.expires_at( boost::posix_time::pos_infin );
				return;
			}

			m_deadline_timer.async_wait( boost::bind( &PlainWolfClient::check_deadline, this ) );
		}

};

}

namespace {

void read_from_stdin( WolfClient *c )
{
	char line[2048];
	do {
		std::cin.getline( line, 2048, '\n' );
		if( !std::cin.eof( ) ) {
			c->write( line );
		}
	} while( !std::cin.eof( ) );

	c->stop( );
}

}

int main( int argc, char *argv[] )
{
        boost::program_options::options_description options;
	boost::program_options::positional_options_description pd;

	options.add_options( )
		( "version,v", "print version" )
		( "help,h", "print help message" )
		( "connect-timeout", "in seconds, how long to wait for connect" )
		( "read-timeout", "in seconds, terminate after inactivity" )
#ifdef WITH_SSL
		( "ssl,S", "use SSL encryption" )
#endif
		( "host", "the host to connect to" )
		( "port", "the port to connect to" )
		;
	pd.add( "host", 1 ).add( "port", 1 );

	boost::program_options::variables_map map;
	store( boost::program_options::command_line_parser( argc, argv )
		.options( options ).positional( pd ).run( ), map );
	notify( map );

	if( map.count( "help" ) ) {
		std::cout << "Usage: wolfcli [options] <host> <port>" << std::endl;
		std::cout << "Options:" << std::endl;
		options.print( std::cout );
		return 1;
	}

	if( !map.count( "host" ) ) {
		std::cerr << "ERROR: a host name or IP is required as first argument!" << std::endl;
		return 1;
	}
	if( !map.count( "port" ) ) {
		std::cerr << "ERROR: a port is required as second argument!" << std::endl;
		return 1;
	}

	char *host = argv[argc-2];
	char *port = argv[argc-1];

	boost::asio::io_service io_service;
	boost::asio::ip::tcp::resolver resolver( io_service );

#ifdef WITH_SSL
	boost::asio::ssl::context ctx( io_service, boost::asio::ssl::context::sslv23 );
	if( map.count( "ssl" ) ) {
		ctx.set_options( boost::asio::ssl::context::default_workarounds
			| boost::asio::ssl::context::no_sslv2 );
		ctx.set_verify_mode( boost::asio::ssl::context::verify_peer );
//		ctx.load_verify_file( "ca.pem" );
	}
#endif // WITH_SSL

	WolfClient *c;
#ifdef WITH_SSL
	if( map.count( "ssl" ) ) {
		c = new SSLWolfClient( io_service, ctx, 20, 5 );
	} else {
#endif // WITH_SSL
		c = new PlainWolfClient( io_service, 20, 5 );
#ifdef WITH_SSL
	}
#endif // WITH_SSL

	c->start( resolver.resolve( boost::asio::ip::tcp::resolver::query( host, port ) ) );

	boost::thread netthread( boost::bind( &boost::asio::io_service::run, &io_service ) );
	boost::thread stdinthread( read_from_stdin, c );

	netthread.join( );
	// no sense to wait for stdin thread to wait here, we have a broken connection!
	// we can't set interruptions points either! So it's either this or a POSIX pipe
	// or WaitForObject on the stdin handle (using or writing asio extensions)

	delete c;
}

