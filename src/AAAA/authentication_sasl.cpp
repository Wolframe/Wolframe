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
// authentication_sasl.cpp
//
//  Basic Server model:
//   1. call sasl_server_init() at startup to load plug-ins
//   2. On connection, call sasl_server_new()
//   3. call sasl_listmech() and send list to client]
//   4. after client AUTH command, call sasl_server_start(), goto 5a
//   5. call sasl_server_step()
//   5a. If SASL_CONTINUE, output to client, wait response, repeat 5
//   5b. If SASL error, then goto 7
//   5c. If SASL_OK, move on
//   6. continue with application protocol until connection closes
//      call sasl_getprop to get username
//      call sasl_getprop/sasl_encode/sasl_decode() if using security layer
//   7. call sasl_dispose(), may return to step 2
//   8. call sasl_done() when program terminates

#include "AAAA/authentication_sasl.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/thread/thread.hpp>

#include <stdexcept>
#include <sstream>

#include "sasl/sasl.h"

#include "logger-v1.hpp"

namespace _Wolframe {
namespace AAAA {

static int sasl_my_log( void* /* context */, int priority, const char *message )
{
	if( message == NULL ) return SASL_BADPARAM;
	
	_Wolframe::log::LogLevel::Level level;
	
	switch( priority ) {
		case SASL_LOG_ERR:
		case SASL_LOG_FAIL:	level = _Wolframe::log::LogLevel::LOGLEVEL_ERROR; break;
		case SASL_LOG_WARN:	level = _Wolframe::log::LogLevel::LOGLEVEL_WARNING; break;
		case SASL_LOG_NOTE:	level = _Wolframe::log::LogLevel::LOGLEVEL_NOTICE; break;
		case SASL_LOG_DEBUG:	level = _Wolframe::log::LogLevel::LOGLEVEL_DEBUG; break;
		case SASL_LOG_TRACE:	level = _Wolframe::log::LogLevel::LOGLEVEL_TRACE; break;
		case SASL_LOG_PASS:	level = _Wolframe::log::LogLevel::LOGLEVEL_DATA; break;
		default:		level = _Wolframe::log::LogLevel::LOGLEVEL_INFO; break;
	}

	_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( level )
		<< _Wolframe::log::LogComponent::LogAuth
		<< "SASL " << message;

	return SASL_OK;
}

static int sasl_my_getconfpath( void* context, char **path )
{
	SaslAuthenticator *a = (SaslAuthenticator *)context;
	*path = strdup( a->getConfPath( ).c_str( ) );
	if( !*path ) return SASL_FAIL;

	return SASL_OK;
}

SaslAuthenticator::SaslAuthenticator( const std::string appName, const std::string service, const std::string confpath )
	: m_appName( appName ),
	  m_service( service ),
	  m_confpath( confpath )
{
// register callbacks
	m_callbacks[0].id = SASL_CB_LOG;
	m_callbacks[0].proc = (int (*)( ))&sasl_my_log;
	m_callbacks[0].context = this;
// SASL_CB_GETCONFPATH appeared only in version 2.1.22
#ifdef SASL_CB_GETCONFPATH
	m_callbacks[1].id = SASL_CB_GETCONFPATH;
	m_callbacks[1].proc = (int (*)( ))&sasl_my_getconfpath;
#endif // SASL_CB_GETCONFPATH
	m_callbacks[1].context = this;
	m_callbacks[2].id = SASL_CB_LIST_END;
	m_callbacks[2].proc = NULL;
	m_callbacks[2].context = NULL;

	m_connection = NULL;
	
// initialize the SASL library
	int result = sasl_server_init( m_callbacks, "test" );
	if( result != SASL_OK ) {
		std::ostringstream ss;
		ss << "Failed to initialize libsasl: " << sasl_errstring( result, NULL, NULL ) << "(" << result << ")";
		throw new std::runtime_error( ss.str( ) );
	}

	m_state = _Wolframe_SASL_STATE_NEW;
}

SaslAuthenticator::~SaslAuthenticator( )
{
	sasl_done( );
}

Step::AuthStep SaslAuthenticator::nextStep( )
{
	int result;
	const char *mechs;
	unsigned int len_mechs;
	int nof_mechs;
	const char *out;
	unsigned int out_len;
	
	switch( m_state ) {
		case _Wolframe_SASL_STATE_NEW:
// create authentication session
			result = sasl_server_new( m_service.c_str( ),
				NULL,	// localdomain
				NULL,	// userdomain
				NULL,	// iplocal
				NULL,	// ipremote
				NULL,	// per connection callbacks
				0,	// flags (SASL_SUCCESS_DATA)
				&m_connection );
			if( result != SASL_OK ) {
				std::ostringstream ss;
				ss	<< "Failed to allocate SASL connection state: " << sasl_errstring( result, NULL, NULL )
					<< "(" << result << "), " << sasl_errdetail( m_connection );
				m_error = ss.str( );
				m_state = _Wolframe_SASL_STATE_ERROR;
				return Step::_Wolframe_AUTH_STEP_GET_ERROR;
			}

// get list of available SASL mechs, go into negiotation
			result = sasl_listmech(
				m_connection,
				NULL,		// ext_authid
				NULL,		// prefix of result
				" ",		// separator of mechs
				NULL,		// suffix of result
				&mechs,		// list of mechs
				&len_mechs,	// length of mech string
				&nof_mechs );	// number of mechs
			if( result != SASL_OK ) {
				std::ostringstream ss;
				ss	<< "Generating list of SASL mechs failed: " << sasl_errstring( result, NULL, NULL )
					<< "(" << result << "), " << sasl_errdetail( m_connection );
				m_error = ss.str( );
				m_state = _Wolframe_SASL_STATE_ERROR;
				return Step::_Wolframe_AUTH_STEP_GET_ERROR;
			}
			m_token = "SASL_mechs";
			m_data = std::string( mechs );
			m_state = _Wolframe_SASL_STATE_NEGOTIATE_MECHS;
			return Step::_Wolframe_AUTH_STEP_SEND_DATA;
		
		case _Wolframe_SASL_STATE_NEGOTIATE_MECHS:
			m_token = "SASL_mech";
			m_state = _Wolframe_SASL_STATE_NEGOTIATE_MECHS;
			return Step::_Wolframe_AUTH_STEP_RECV_DATA;
		
		case _Wolframe_SASL_STATE_INITIAL_DATA:
			m_token = "SASL_data";
			m_state = _Wolframe_SASL_STATE_START;
			return Step::_Wolframe_AUTH_STEP_RECV_DATA;
			
		case _Wolframe_SASL_STATE_START:
// client picked a mech
			result = sasl_server_start(
				m_connection,
				m_mech.c_str( ),	// mech choosen by client
				( m_client_data.length( ) == 0 ? NULL :m_client_data.data( ) ),	// optional client data
				m_client_data.length( ),// length of client data
				&out,			// server data
				&out_len );		// length of server data

// already authenticated
			if( result == SASL_OK ) {
				sasl_dispose( &m_connection );
				m_connection = NULL;
				m_state = _Wolframe_SASL_STATE_NEW;
				return Step::_Wolframe_AUTH_STEP_SUCCESS;
			}

// an error occurred?
			if( result != SASL_CONTINUE ) {
				std::ostringstream ss;
				ss	<< "Starting SASL server failed: " << sasl_errstring( result, NULL, NULL )
					<< "(" << result << "), " << sasl_errdetail( m_connection );
				m_error = ss.str( );
				m_state = _Wolframe_SASL_STATE_ERROR;
				return Step::_Wolframe_AUTH_STEP_GET_ERROR;
			}

// otherwise authentication continues, but now the server has to send something
			m_token = "SASL_data";
			m_data = std::string( out );
			m_state = _Wolframe_SASL_STATE_WAIT;
			return Step::_Wolframe_AUTH_STEP_SEND_DATA;
		
		case _Wolframe_SASL_STATE_WAIT:
// data has been sent by server, await client data now
			m_token = "SASL_data";
			return Step::_Wolframe_AUTH_STEP_RECV_DATA;
		
		case _Wolframe_SASL_STATE_STEP:
// data received from client, executing next SASL step now
			result = sasl_server_step(
				m_connection,
				m_client_data.data( ),		// client data
				m_client_data.length( ),	// size of client data
				&out,				// server data
				&out_len );			// length of server data

// authenticated
			if( result == SASL_OK ) {
				sasl_dispose( &m_connection );
				m_connection = NULL;
				m_state = _Wolframe_SASL_STATE_NEW;
				return Step::_Wolframe_AUTH_STEP_SUCCESS;
			}

// an error occurred?
			if( result != SASL_CONTINUE ) {
				std::ostringstream ss;
				ss	<< "A step in SASL server failed: " << sasl_errstring( result, NULL, NULL )
					<< "(" << result << "), " << sasl_errdetail( m_connection );
				m_error = ss.str( );
				m_state = _Wolframe_SASL_STATE_ERROR;
				return Step::_Wolframe_AUTH_STEP_GET_ERROR;
			}

// otherwise authentication continues, server has to send something
			m_token = "SASL_data";
			m_data = std::string( out );
			m_state = _Wolframe_SASL_STATE_WAIT;
			return Step::_Wolframe_AUTH_STEP_SEND_DATA;
			
		case _Wolframe_SASL_STATE_ERROR:
			goto FAIL;
	}

FAIL:
	if( m_connection != NULL ) {
		sasl_dispose( &m_connection );
		m_connection = NULL;
	}

	boost::this_thread::sleep( boost::posix_time::seconds( 1 ) );

	m_state = _Wolframe_SASL_STATE_NEW;
	return Step::_Wolframe_AUTH_STEP_FAIL;			
}

std::string SaslAuthenticator::sendData( )
{
	return m_data;
}

std::string SaslAuthenticator::token( )
{
	return m_token;
}

void SaslAuthenticator::receiveData( const std::string data )
{
	switch( m_state ) {
		case _Wolframe_SASL_STATE_NEGOTIATE_MECHS:
			m_mech = data;
			m_state = _Wolframe_SASL_STATE_INITIAL_DATA;
			break;
		
		case _Wolframe_SASL_STATE_START:
			m_client_data = data;
			m_state = _Wolframe_SASL_STATE_START;
			break;

		case _Wolframe_SASL_STATE_WAIT:
			m_client_data = data;
			m_state = _Wolframe_SASL_STATE_STEP;
			break;
		
		case _Wolframe_SASL_STATE_NEW:
		case _Wolframe_SASL_STATE_INITIAL_DATA:
		case _Wolframe_SASL_STATE_STEP:
		case _Wolframe_SASL_STATE_ERROR:
			// TODO: application exception
			throw std::runtime_error( "Illegal state in auhenticator" );
			break;
	}
}

std::string SaslAuthenticator::getError( )
{
	return m_error;
}

}} // namespace _Wolframe::AAAA
