/************************************************************************

 Copyright (C) 2011 - 2014 Project Wolframe.
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
/// \file AAAA/authenticator.hpp
///
/// Authenticator interface definition

#ifndef _AUTHENTICATOR_HPP_INCLUDED
#define _AUTHENTICATOR_HPP_INCLUDED

#include <string>
#include <vector>
#include "AAAA/user.hpp"
#include "system/connectionEndpoint.hpp"

namespace _Wolframe {
namespace AAAA {

/// \class Authenticator
/// \brief Authenticator interface
/// This the only interface to be used by the system. All other interfaces
/// are internal to the authentication objects.
///
/// \note	The authenticator works only with complete messages.
///		Sending a message in multiple parts will most likely
///		result in an error. But that depends also on the authentication
///		backend.
class Authenticator
{
public:
	enum Status	{
		INITIALIZED,		///< the authenticator is initialized,
					///  no mech has been selected yet
		MESSAGE_AVAILABLE,	///< an output message is available
		AWAITING_MESSAGE,	///< waiting for an input message
		AUTHENTICATED,		///< a user has been authenticated
		INVALID_CREDENTIALS,	///< the user authentication failed,
					///  either the username or the credentials are invalid
		MECH_UNAVAILABLE,	///< the requested mech is not available,
					///  usually due to configured restrictions
		SYSTEM_FAILURE		///< some other error occurred
	};

	static const char* statusName( Status i)
	{
		static const char* ar[] = {"INITIALIZED","MESSAGE_AVAILABLE","AWAITING_MESSAGE","AUTHENTICATED","INVALID_CREDENTIALS","MECH_UNAVAILABLE","SYSTEM_FAILURE"};
		return ar[i];
	}

	/// The virtual destructor
	virtual ~Authenticator()	{}

	/// Destroy the authenticator
	///
	/// \note	In many cases this is a suicidal function (delete this),
	///		so you should be very careful how you use it.
	///		You should use this function instead of delete
	///		because not all authentication instances are created with new.
	virtual void dispose() = 0;

	/// The list of available mechs
	virtual const std::vector<std::string>& mechs() const = 0;

	/// Set the authentication mech
	/// \param [in]	mech	the name of the mech (case-insensitive)
	/// \returns		true if the mech could be selected
	///			false if the mech is not available
	/// \note	This function works like a reset function
	///		Whenever it is called it will release all the allocated
	///		resources and it will reinitialize all the data structures
	virtual bool setMech( const std::string& mech ) = 0;

	/// The input message
	/// \param [in]	message	the input message
	virtual void messageIn( const std::string& message ) = 0;

	/// The output message
	/// \returns		the output message
	virtual std::string messageOut() = 0;

	/// The current status of the authenticator
	virtual Status status() const = 0;

	/// The authenticated user or NULL if not authenticated
	/// \note	It is intended that this function can be called only once.
	///		As a security precaution, all the instance information regarding
	///		the current authentication operation should be destroyed after
	///		the authentication is complete and the user is no longer
	///		available after the call of this function
	virtual User* user() = 0;
};

}} // namespace _Wolframe::AAAA

#endif // _AUTHENTICATOR_HPP_INCLUDED
