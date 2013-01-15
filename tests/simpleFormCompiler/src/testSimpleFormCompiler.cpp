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
///\file tests/testSimpleFormCompiler.cpp
#include "utils/miscUtils.hpp"
#include "ddl/atomicType.hpp"
#include "ddl/structType.hpp"
#include "modules/ddlcompiler/simpleform/simpleFormCompiler.hpp"
#include "modules/normalize/number/integerNormalizeFunction.hpp"
#include "modules/normalize/number/floatNormalizeFunction.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <iostream>
#include "gtest/gtest.h"
#include <boost/thread/thread.hpp>
#include <boost/algorithm/string.hpp>
#include <stdexcept>
#include <boost/filesystem.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

class DDLTypeMap :public ddl::TypeMap
{
public:
	DDLTypeMap(){}

	virtual const ddl::NormalizeFunction* getType( const std::string& name) const
	{
		static IntegerNormalizeFunction int_( true, 10);
		static IntegerNormalizeFunction uint_( false, 10);
		static FloatNormalizeFunction float_( 10, 10);
		if (boost::algorithm::iequals( name, "int")) return &int_;
		if (boost::algorithm::iequals( name, "uint")) return &uint_;
		if (boost::algorithm::iequals( name, "float")) return &float_;
		return 0;
	}
};

using namespace _Wolframe;
static boost::filesystem::path g_testdir;

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

TEST_F( SimpleFormCompilerTest, tests)
{
	unsigned int ti;
	for (ti=0; testDescription[ti].srcfile; ti++)
	{
		boost::filesystem::path pp = g_testdir / "data" / testDescription[ti].srcfile;
		std::string srcfile = pp.string() + ".simpleform";
		ddl::SimpleFormCompiler mm;
		DDLTypeMap typemap;
		std::vector<ddl::Form> sr = mm.compile( utils::readSourceFileContent( srcfile), &typemap);
		std::vector<ddl::Form>::const_iterator si = sr.begin(), se = sr.end();
		for (; si != se; ++si)
		{
			si->print( std::cerr);
		}
	}
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	g_testdir = boost::filesystem::path( utils::getParentPath( argv[0], 2));
	return RUN_ALL_TESTS();
}


