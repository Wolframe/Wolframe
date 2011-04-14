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

#include "sasl/sasl.h"

namespace _Wolframe {
namespace AAAA {

class SaslAuthenticator : public Authenticator {
	private:
		enum {
			_Wolframe_SASL_STATE_NEED_LOGIN,
			_Wolframe_SASL_STATE_NEED_PASS,
			_Wolframe_SASL_STATE_COMPUTE
		} m_state;

		sasl_callback_t callbacks[2];

		std::string m_appName;
		std::string m_service;
		
		std::string m_token;
		std::string m_login;
		std::string m_pass;

	public:
		SaslAuthenticator( const std::string appName, const std::string service );
		virtual ~SaslAuthenticator( );
		virtual Step::AuthStep nextStep( );
		virtual std::string sendData( );
		virtual std::string token( );
		virtual void receiveData( const std::string data );
		virtual std::string getError( );
};

}} // namespace _Wolframe::AAAA

#endif // _AUTHENTICATION_SASL_HPP_INCLUDED
