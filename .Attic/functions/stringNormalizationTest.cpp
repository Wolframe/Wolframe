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

//
// string normalization unit tests
//

#include "utils/strNormalization.hpp"
#include "gtest/gtest.h"
#include "wtest/testReport.hpp"

using namespace _Wolframe::utils;

static const char* testStr = " ? This is ,  a nOt \n \tnormalized string\nwith\tüöä$éàè˝àč :\t. \"characters\'\n";
static const char* expectedStr = "THIS IS A NOT NORMALIZED STRING WITH üöä éàè˝àč CHARACTERS";

TEST( StringNormalizationFixture, InPlaceNormalization )	{
	std::string str = testStr;
	normalizeString( str );
	ASSERT_STREQ( expectedStr, str.c_str());
}

TEST( StringNormalizationFixture, CopyNormalization )	{
	std::string str = testStr;
	std::string ret = normalizeString_copy( str );
	ASSERT_STREQ( testStr, str.c_str());
	ASSERT_STREQ( expectedStr, ret.c_str());
}


int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	WOLFRAME_GTEST_REPORT( argv[0], refpath.string());
	return RUN_ALL_TESTS();
}
