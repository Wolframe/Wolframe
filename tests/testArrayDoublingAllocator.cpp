/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
///\file testArrayDoublingAllocator.cpp
///\brief test program for Array Doubling Allocator (types/allocators.hpp)

#include "gtest/gtest.h"
#include "types/allocators.hpp"

using namespace _Wolframe;
using namespace _Wolframe::types;

TEST( ArrayDoublingAllocator, tests )
{
	static int size[] = {12,45,4566,2324,12343513,1234,234,6,7,98435,43543549,329847932,12345,32432424,0};
	int pp[32];
	ArrayDoublingAllocator mem;
	std::size_t ii;
	for (ii=0; size[ii]; ++ii)
	{
		pp[ii] = mem.alloc( size[ii]);
		char* ptr = (char*)mem.base() + pp[ii];
		memset( ptr, 'A' + ii, size[ii]);
	}
	for (ii=0; size[ii]; ++ii)
	{
		char* ptr = (char*)mem.base() + pp[ii];
		for (int kk=0; kk<size[ii]; ++kk)
		{
			ASSERT_EQ (ptr[kk], ('A' + ii));
		}
	}
	SUCCEED();
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
