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
///\file testTestAllocator.cpp
///\brief test program for malloc wrapper

#include "gtest/gtest.h"
#include "utils/malloc.hpp"

using namespace _Wolframe;

TEST( TestAllocator, tests )
{
	static int size[] = {12,45,4566,2324,123,1234,21134,6,7,18435,3549,31932,12345,31424,0};
	void* pp[32];
	std::size_t ii;
	for (ii=0; size[ii]; ++ii)
	{
		pp[ii] = types::malloc( size[ii]);
	}
	for (ii=0; size[ii]; ++ii)
	{
		types::free( pp[ii]);
	}
	SUCCEED();
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
