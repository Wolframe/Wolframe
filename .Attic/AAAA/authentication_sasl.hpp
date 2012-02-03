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
// authentication_textfile.hpp
//

#ifndef _AUTHENTICATION_SASL_HPP_INCLUDED
#define _AUTHENTICATION_SASL_HPP_INCLUDED

#include "AAAA/authentication.hpp"

#include <string>

#ifdef WITH_SASL

#include "sasl/sasl.h"

namespace _Wolframe {
namespace AAAA {

class SaslAuthenticator : public Authenticator {
	private:
		enum {
			_Wolframe_SASL_STATE_NEW,
			_Wolframe_SASL_STATE_NEGOTIATE_MECHS,
			_Wolframe_SASL_STATE_INITIAL_DATA,
			_Wolframe_SASL_STATE_START,
			_Wolframe_SASL_STATE_WAIT,
			_Wolframe_SASL_STATE_STEP,
			_Wolframe_SASL_STATE_ERROR
		} m_state;

		sasl_callback_t m_callbacks[3];
		sasl_conn_t *m_connection;

		std::string m_mech;
		std::string m_client_data;
		
		std::string m_error;
		std::string m_data;
		std::string m_token;

	public:
		virtual ~SaslAuthenticator( );
		virtual Step::AuthStep nextStep( );
		virtual std::string sendData( );
		virtual std::string token( );
		virtual void receiveData( const std::string data );
		virtual std::string getError( );

		std::string getConfPath( ) { return m_confpath; }
};

}} // namespace _Wolframe::AAAA

#endif // WITH_SASL

#endif // _AUTHENTICATION_SASL_HPP_INCLUDED
