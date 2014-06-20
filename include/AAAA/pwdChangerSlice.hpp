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
///
/// \file pwdChangerSlice.hpp
/// \brief Password changer slice interface
///

#ifndef _PASSWORD_CHANGER_SLICE_HPP_INCLUDED
#define _PASSWORD_CHANGER_SLICE_HPP_INCLUDED

#include <string>

#include "AAAA/user.hpp"

namespace _Wolframe {
namespace AAAA {

/// AuthenticatorSlice
/// This is the base class for authenticator slices implementations
/// An authenticator has (usually) several authenticator slices
/// The AuthenticatorSlice(s) are provided by the their respective
/// AuthenticationUnit(s) in the AAAA provider
///
class PasswordChangerSlice
{
public:
	enum Status	{
		MESSAGE_AVAILABLE,	///< an output message is available
		AWAITING_MESSAGE,	///< waiting for an input message
		PASSWORD_CHANGED,	///< the password has been successfuly changed
		INVALID_DATA,		///< the password change message is not valid,
					///  most likely the credentials are invalid
		SYSTEM_FAILURE		///< some other error occurred
	};

	static const char* statusName( Status i )
	{
		static const char* ar[] = {	"MESSAGE_AVAILABLE",
						"AWAITING_MESSAGE",
						"PASSWORD_CHANGED",
						"INVALID_DATA",
						"SYSTEM_FAILURE"
					  };
		return ar[i];
	}

	/// The virtual destructor
	virtual ~PasswordChangerSlice()	{}

	/// Dispose of the authenticator
	///
	/// \note	In many cases this is a suicidal function (delete this),
	///		so you should be very careful how you use it.
	///		You should use this function instead of delete
	///		because not all instances are created with new.
	virtual void dispose() = 0;

	/// The class name of the authentication / changer unit / subunit
	///\note	This is the name of the authentication type / class
	virtual const char* className() const = 0;

	/// The identifier of the authentication /changer unit / slice
	///\note	This is the identifier of the authentication unit / slice
	virtual const std::string& identifier() const = 0;

	/// The input message
	/// \param [in]	message	the input message
	virtual void messageIn( const std::string& message ) = 0;

	/// The output message
	/// \returns		the output message
	virtual std::string messageOut() = 0;

	/// The current status of the authenticator
	virtual Status status() const = 0;
};

}} // namespace _Wolframe::AAAA

#endif // _PASSWORD_CHANGER_SLICE_HPP_INCLUDED
