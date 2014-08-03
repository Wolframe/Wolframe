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
/// \file types/secureString.hpp
/// \brief String constant definition with the intention to hold sensitive data
#ifndef _WOLFRAME_TYPES_SECURE_STRING_HPP_INCLUDED
#define _WOLFRAME_TYPES_SECURE_STRING_HPP_INCLUDED
#include <string>

namespace _Wolframe {
namespace types {

/// \class SecureString
/// \brief String definition with the intention to hold sensitive data
/// \note Standard STL string implementation wipes out all of its data before releasing it
class SecureString
	:public std::string
{
public:
	SecureString(){}
	SecureString( const std::string& o)
		:std::string(o){}

	~SecureString()
	{
		eraseContent();
	}
	void clear()
	{
		eraseContent();
		std::string::clear();
	}

private:
	void eraseContent()
	{
		std::string::iterator si = begin(), se = end();
		for (; si != se; ++si) *si = '\0';
	}
};

}}//namespace
#endif

