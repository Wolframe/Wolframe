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
///\file tests/testSimpleFormCompiler.cpp
#include "ddl/compiler/simpleFormCompiler.hpp"
#ifdef _WIN32
#pragma warning(disable:4996)
#pragma warning(disable:4127)
#endif
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <iostream>
#include "gtest/gtest.h"
#include <boost/thread/thread.hpp>
#include <stdexcept>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

using namespace _Wolframe;

struct TestDescription
{
	const char* srcfile;
};

static const TestDescription testDescription[2] = {
{
	"test1"
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

static bool readFile( const char* fn, std::string& out)
{
	char buf;
	std::fstream ff;
	ff.open( fn, std::ios::in | std::ios::binary);
	while (ff.read( &buf, sizeof(buf)))
	{
		out.push_back( buf);
	}
	bool rt = ((ff.rdstate() & std::ifstream::eofbit) != 0);
	ff.close();
	return rt;
}

TEST_F( SimpleFormCompilerTest, tests)
{
	unsigned int ti;
	for (ti=0; testDescription[ti].srcfile; ti++)
	{
		boost::filesystem::path pp = boost::filesystem::current_path() / "simpleFormCompiler" / "data" / testDescription[ti].srcfile;
		std::string srcfile = pp.string() + ".simpleform";
		std::string srcstring;
		if (!readFile( srcfile.c_str(), srcstring)) throw std::runtime_error("could not read test input file");
		ddl::SimpleFormCompiler mm;
		ddl::StructType sr;
		std::string error;
		EXPECT_EQ( true, mm.compile( srcstring, sr, error));
		ASSERT_EQ( "", error);
		sr.print( std::cerr);

	}
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}


