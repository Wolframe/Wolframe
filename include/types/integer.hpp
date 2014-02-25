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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file types/integer.hpp
///\brief Integer number value type

#ifndef _Wolframe_TYPES_INTEGER_HPP_INCLUDED
#define _Wolframe_TYPES_INTEGER_HPP_INCLUDED

#include <limits>
#include <boost/cstdint.hpp>

#define _WOLFRAME_INTEGER		boost::int64_t
#define _WOLFRAME_UINTEGER		boost::uint64_t

#define _WOLFRAME_MAX_INTEGER		std::numeric_limits<_WOLFRAME_INTEGER>::max()
#define _WOLFRAME_MAX_UINTEGER		std::numeric_limits<_WOLFRAME_UINTEGER>::max()

#define _WOLFRAME_MIN_INTEGER		std::numeric_limits<_WOLFRAME_INTEGER>::min()
#define _WOLFRAME_MIN_UINTEGER		std::numeric_limits<_WOLFRAME_UINTEGER>::min()

#define _WOLFRAME_INTEGER_DIGITS	19
#define _WOLFRAME_UINTEGER_DIGITS	20

#endif
