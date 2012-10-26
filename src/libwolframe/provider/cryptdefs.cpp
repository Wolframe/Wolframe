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
///\file cryptdefs.cpp
#include "cryptdefs.hpp"

#ifdef _WIN32
#include <Windows.h>
#include <Wincrypt.h>
#endif

using namespace _Wolframe;
using namespace _Wolframe::AAAA;

#ifdef _WIN32
	const char* _Wolframe::AAAA::DEFAULT_RANDOM_GENERATOR()
	{
		return MS_DEF_PROV;
	}
#else
	const char* _Wolframe::AAAA::DEFAULT_RANDOM_GENERATOR()
	{
		return "/dev/urandom";
	}
#endif

