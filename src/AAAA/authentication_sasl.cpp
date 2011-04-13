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

#include "AAAA/authentication_sasl.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/thread/thread.hpp>

#include <stdexcept>

#include "sasl/sasl.h"

namespace _Wolframe {
namespace AAAA {

Authenticator *CreateSaslAuthenticator( AuthenticatorFactory::properties props )
{
	return new SaslAuthenticator(
//		findprop<std::string>( props, "filename" )
	);
}

SaslAuthenticator::SaslAuthenticator( )
{
	sasl_server_init( NULL, "test" );
	m_state = _Wolframe_SASL_STATE_NEED_LOGIN;
}

SaslAuthenticator::~SaslAuthenticator( )
{
	sasl_done( );
}

Step::AuthStep SaslAuthenticator::nextStep( )
{
	switch( m_state ) {
		case _Wolframe_SASL_STATE_NEED_LOGIN:
			m_token = "login";
			return Step::_Wolframe_AUTH_STEP_RECV_DATA;

		case _Wolframe_SASL_STATE_NEED_PASS:
			m_token = "password";
			return Step::_Wolframe_AUTH_STEP_RECV_DATA;

		case _Wolframe_SASL_STATE_COMPUTE:
// user not found
			if( false ) {
				m_state = _Wolframe_SASL_STATE_NEED_LOGIN;
				goto FAIL;
			}
// user found, but password doesn't match
			if( false ) {
				m_state = _Wolframe_SASL_STATE_NEED_LOGIN;
				goto FAIL;
			}

// everythink is peachy
			m_state = _Wolframe_SASL_STATE_NEED_LOGIN;
			return Step::_Wolframe_AUTH_STEP_SUCCESS;
	}

FAIL:
	boost::this_thread::sleep( boost::posix_time::seconds( 1 ) );

	return Step::_Wolframe_AUTH_STEP_FAIL;
}

// never used
std::string SaslAuthenticator::sendData( )
{
	return 0;
}

std::string SaslAuthenticator::token( )
{
	return m_token;
}

void SaslAuthenticator::receiveData( const std::string data )
{
	switch( m_state ) {
		case _Wolframe_SASL_STATE_NEED_LOGIN:
			m_login = data;
			m_state = _Wolframe_SASL_STATE_NEED_PASS;
			break;

		case _Wolframe_SASL_STATE_NEED_PASS:
			m_pass = data;
			m_state = _Wolframe_SASL_STATE_COMPUTE;
			break;

// TODO: application exception
		case _Wolframe_SASL_STATE_COMPUTE:
			throw new std::runtime_error( "Illegal state in auhenticator" );
			break;
	}
}

std::string SaslAuthenticator::getError( )
{
	return "";
}

}} // namespace _Wolframe::AAAA
