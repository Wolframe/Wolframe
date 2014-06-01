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
///
/// \file authentication.hpp
/// \brief top-level header file for authentication
///

#ifndef _AUTHENTICATION_HPP_INCLUDED
#define _AUTHENTICATION_HPP_INCLUDED

#include <string>

namespace _Wolframe {
namespace AAAA {

// the basic steps the authenticator can be in
class Step {
public:
	enum AuthStep {
		_Wolframe_AUTH_STEP_SUCCESS,		/// successful authentication
		_Wolframe_AUTH_STEP_FAIL,		/// authentication failed
		_Wolframe_AUTH_STEP_SEND_DATA,		/// we need to send some data
		_Wolframe_AUTH_STEP_RECV_DATA,		/// we require some data
		_Wolframe_AUTH_STEP_GET_ERROR,		/// error occurred
		_Wolframe_AUTH_STEP_NEXT		/// call next step
	};
};

// virtual base for all authentication methods
class Authenticator {
public:
	// get next step in authentication
	virtual Step::AuthStep nextStep( ) = 0;

	// used when sending or receiving to indicate
	// the kind of data the Authenticator expects
	// (e.g. "login", "password", "md5")
	virtual std::string token( ) = 0;

	// the authenticator wants us to send out data
	// (for instance a challenge). The message has
	// to be send to the client
	//
	// token() gives you the kind of data (which
	// depends on the authentication method)
	virtual std::string sendData( ) = 0;

	// the authenticator can't continue without getting
	// some data (for instance we need a password)
	//
	// token() indicates the kind of data the authenticator
	// expects (depends on the authentication method)
	virtual void receiveData( const std::string data ) = 0;

	// we got an error (which usually should be logged only,
	// not sent to the client)
	virtual std::string getError( ) = 0;

};

}} // namespace _Wolframe::AAAA

#endif // _AUTHENTICATION_HPP_INCLUDED
