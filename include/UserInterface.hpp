/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
/// \file UserInterface.hpp
///

#ifndef _USER_INTERFACE_HPP_INCLUDED
#define _USER_INTERFACE_HPP_INCLUDED

#include <string>
#include "version.hpp"

namespace _Wolframe {
namespace UI {

/// User Interface form.
/// For now this is just a rudimentary skeleton
class UIform
{
public:
	UIform()				{}
private:
	std::string	m_name;		///< the name of the form
	Version		m_version;	///< the version of the form
	std::string	m_body;		///< the body of the form
};


/// Supplier of user interface elements.
/// Work in progress...
class UserInterfaceSupplier
{
public:
	UserInterfaceSupplier()				{}

	virtual ~UserInterfaceSupplier()		{}
};


}} // namespace _Wolframe::UI

#endif // _USER_INTERFACE_HPP_INCLUDED
