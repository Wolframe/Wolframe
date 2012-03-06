/************************************************************************
Copyright (C) 2011 Project Wolframe.
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
/// \file tests/simpleFormCompilerTest.cpp
#include "tests/testUtils.hpp"
#include "ddl/compiler/simpleFormCompiler.hpp"
#ifdef _WIN32
#pragma warning(disable:4996)
#pragma warning(disable:4127)
#endif
#include <iostream>
#include <gtest/gtest.h>
#include <boost/thread/thread.hpp>

using namespace _Wolframe;

struct TestDescription
{
	const char* srcfile;
};

static const TestDescription testDescription[2] = {
{
	"directmap_compiler"
},
{0}
};

class SimpleFormCompilerTest : public ::testing::Test
{
protected:
	SimpleFormCompilerTest() {}
	virtual ~SimpleFormCompilerTest() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};

TEST_F( SimpleFormCompilerTest, tests)
{
	unsigned int ti;
	for (ti=0; testDescription[ti].srcfile; ti++)
	{
		std::string srcfile = wtest::Data::getDataFile( testDescription[ti].srcfile, "data", ".frm");
		std::string srcstring;
		if (!wtest::Data::readFile( srcfile.c_str(), srcstring)) throw std::runtime_error("could not read test input file");
		ddl::SimpleFormCompiler mm;
		ddl::StructType sr;
		std::string error;
		EXPECT_EQ( true, mm.compile( srcstring, sr, error));
		ASSERT_EQ( "", error);
	}
}

int main( int argc, char **argv )
{
	wtest::Data::createDataDir( "result");
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}


