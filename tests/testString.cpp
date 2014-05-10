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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file testString.cpp
///\brief Test for types/string.hpp
#include "types/string.hpp"
#include "gtest/gtest.h"
#include "wtest/testReport.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <limits>
#include <cstdlib>
#include <boost/cstdint.hpp>

using namespace _Wolframe;

class StringDescriptionTest
	:public ::testing::Test
{
protected:
	StringDescriptionTest()
	{
		srand(123);
	}
	virtual ~StringDescriptionTest() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};

TEST_F( StringDescriptionTest, tests)
{
	static const char* latinUmlautStr = "äöüéè";
	static const char* asciiStr = "abcdefghijklmnopqrstuvwxyz_";
	std::size_t latinUmlautStrLen = std::strlen(latinUmlautStr);
	std::size_t asciiStrLen = std::strlen(asciiStr);

	types::StringConst luLatin1( latinUmlautStr, latinUmlautStrLen, types::String::ISO8859, 1);
	types::StringConst asLatin1( asciiStr, asciiStrLen, types::String::ISO8859, 1);	

	EXPECT_EQ( std::string((const char*)luLatin1.translateEncoding( types::String::UTF16BE)
				.translateEncoding( types::String::ISO8859, 1).ptr()),
			latinUmlautStr);

	EXPECT_EQ( asLatin1.translateEncoding( types::String::UTF16BE).tostring(),
			asLatin1.translateEncoding( types::String::UTF8).tostring());
	std::cerr << "done" << std::endl;
}

int main( int argc, char **argv)
{
	::testing::InitGoogleTest( &argc, argv);
	WOLFRAME_GTEST_REPORT( argv[0], refpath.string());
	return RUN_ALL_TESTS();
}


