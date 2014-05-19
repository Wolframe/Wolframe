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
/// \file authInstance.hpp
/// \brief AuthenticationInstance interface
///

#ifndef _AUTHENTICATION_INSTANCE_HPP_INCLUDED
#define _AUTHENTICATION_INSTANCE_HPP_INCLUDED

#include "AAAA/authenticator.hpp"

namespace _Wolframe {
namespace AAAA {

/// AuthenticatorInstance
/// This is the base class for authenticator slices implementations
/// An authenticator has (usually) several authenticator instances
/// The AuthenticatorInstance(s) are provided by the their respective
/// AuthenticationUnit(s) in the AAAA provider
///
/// \note	For now the AuthenticatorInstance is just an Authenticator
///		but this is very likely to change in the future


class AuthenticatorInstance : public Authenticator
{

};

}} // namespace _Wolframe::AAAA

#endif // _AUTHENTICATION_INSTANCE_HPP_INCLUDED
