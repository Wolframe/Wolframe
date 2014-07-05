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
///\file testCompileTDL.cpp
///\brief Test the parsing and mapping of TDL programs to its internal compiled representation
#include "loadTransactionProgram.hpp"
#include "database/databaseLanguage.hpp"
#include "database/vmTransactionInput.hpp"
#include "tdlTransactionFunction.hpp"
#include "tdlTransactionInput.hpp"
#include "vm/inputStructure.hpp"
#include "utils/fileUtils.hpp"
#include "types/propertyTree.hpp"
#include "filter/ptreefilter.hpp"
#include "logger-v1.hpp"
#include "gtest/gtest.h"
#include "wtest/testReport.hpp"
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace _Wolframe;
using namespace _Wolframe::db;

static int g_gtest_ARGC = 0;
static char* g_gtest_ARGV[2] = {0, 0};
static boost::filesystem::path g_testdir;
static boost::filesystem::path g_outputdir;
static LanguageDescription g_dblang;
static std::string g_selectedTestName;

class CompileTDLTest
	:public ::testing::TestWithParam<std::string>
{
protected:
	CompileTDLTest() {}
	virtual ~CompileTDLTest() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};

static void fillInputStructure( const types::PropertyTree& ptree, vm::InputStructure& input)
{
	langbind::PropertyTreeInputFilter filter( ptree.root());
	langbind::TypedInputFilter::ElementType type;
	types::VariantConst elem;
	int taglevel = 0;

	while (filter.getNext( type, elem))
	{
		switch (type)
		{
			case langbind::FilterBase::OpenTag:
				++taglevel;
				input.openTag( elem);
				break;
			case langbind::FilterBase::CloseTag:
				--taglevel;
				if (taglevel >= 0)
				{
					input.closeTag();
				}
				break;
			case langbind::FilterBase::Value:
				input.pushValue( elem);
				break;
			case langbind::FilterBase::Attribute:
				throw std::logic_error("unexpected attribute element returned by property filter");
		}
	}
}

static std::string normalizeOutputCRLF( const std::string& output, const std::string& expected_)
{
	std::string::const_iterator ei = expected_.begin(), ee = expected_.end();
	std::string::const_iterator oi = output.begin(), oe = output.end();
	std::string resultstr;

	for (; ei != ee && oi != oe; ++oi,++ei)
	{
		if (*ei == *oi)
		{
			resultstr.push_back( *ei);
		}
		else if (*ei == '\r' && *oi == '\n')
		{
			++ei;
			if (*ei == '\n')
			{
				resultstr.push_back( '\r');
				resultstr.push_back( '\n');
			}
			else
			{
				break;
			}
		}
		else if (*ei == '\n' && *oi == '\r')
		{
			++oi;
			if (*oi == '\n')
			{
				resultstr.push_back( '\n');
			}
			else
			{
				break;
			}
		}
	}
	if (oi == oe && ei == ee)
	{
		return resultstr;
	}
	else
	{
		return output;
	}
}


TEST_P( CompileTDLTest, tests)
{
	std::string filename = GetParam();

	std::string testname = utils::getFileStem( filename);
	std::string testdir = utils::getParentPath( filename);
	std::string tdlfile = testdir + "/" + testname + ".tdl";
	std::string expectfile = testdir + "/" + testname + ".res";
	std::string inputfile = testdir + "/" + testname + ".inp";

	// [2.1] Process test:
	std::cerr << "processing test '" << testname << "'" << std::endl;

	TdlTransactionFunctionList tl = loadTransactionProgramFile( tdlfile, "testdb", "test", &g_dblang);
	TdlTransactionFunctionList::const_iterator ti = tl.begin(), te = tl.end();

	// [2.2] Print test output to string:
	std::ostringstream out;
	for (; ti != te; ++ti)
	{
		const TdlTransactionFunction* tfunc = dynamic_cast<TdlTransactionFunction*>( ti->second.get());
		tfunc->print( out);
		out << std::endl;
	}

	// [2.3] Create transaction function from program with input and
	//	print it, if there is an input file defined:
	if (utils::fileExists( inputfile))
	{
		types::PropertyTree input_ptree = utils::readPropertyTreeFile( inputfile);
		for (ti = tl.begin(); ti != te; ++ti)
		{
			const TdlTransactionFunction* tfunc = dynamic_cast<TdlTransactionFunction*>( ti->second.get());
			vm::InputStructure input( tfunc->program()->pathset.tagtab());
			fillInputStructure( input_ptree, input);

			TdlTransactionInput trsinput( *tfunc->program(), input);
			out << "TRANSACTION INPUT:" << std::endl;
			trsinput.print( out);
			out << std::endl;
		}
	}

	// [2.4] Compare result and write dump to the output directory
	//	if not equal to expected:
	bool file_read_exception = false;
	std::string expect;
	try
	{
		expect = utils::readBinaryFileContent( expectfile);
	}
	catch (const std::runtime_error& e)
	{
		file_read_exception = true;
		std::cerr << "failed to read file with expected content: " << e.what() << std::endl;
	}
	std::string output = normalizeOutputCRLF( out.str(), expect);

	if (file_read_exception || expect != output)
	{
		boost::filesystem::path outputdumpfile( g_outputdir / (testname + ".res"));
		utils::writeFile( outputdumpfile.string(), output);
	}
	EXPECT_EQ( expect, output);
}

static std::vector<std::string> tests;

INSTANTIATE_TEST_CASE_P(AllCompileTDLTests,
                        CompileTDLTest,
                        ::testing::ValuesIn(tests));

int main( int argc, char **argv)
{
	g_gtest_ARGC = 1;
	g_gtest_ARGV[0] = argv[0];
	g_testdir = boost::filesystem::system_complete( argv[0]).parent_path();
	g_outputdir = g_testdir / ".." / "output";

	if (argc >= 2)
	{
		if (std::strcmp( argv[1], "-h") == 0 || std::strcmp( argv[1], "--help") == 0)
		{
			std::cerr << argv[0] << " (no arguments)" << std::endl;
			return 0;
		}
		else if (argc == 2)
		{
			g_selectedTestName = argv[ 1];
		}
		else
		{
			std::cerr << "Too many arguments (expected no arguments)" << std::endl;
			return 1;
		}
	}
	
	// [1] Selecting tests to execute:
	if (g_selectedTestName.size())
	{
		std::cerr << "executing tests matching '" << g_selectedTestName << "'" << std::endl;
	}
	boost::filesystem::recursive_directory_iterator ditr( g_testdir / ".." / "data" ), dend;
	for (; ditr != dend; ++ditr)
	{
		std::string filename = utils::resolvePath( ditr->path().string());
		if (boost::iequals( boost::filesystem::extension( *ditr), ".tdl"))
		{
			std::string testname = utils::getFileStem( filename);
			if (g_selectedTestName.size())
			{
				if (std::strstr( testname.c_str(), g_selectedTestName.c_str()))
				{
					std::cerr << "selected test '" << testname << "'" << std::endl;
					tests.push_back( filename);
				}
			}
			else
			{
				tests.push_back( filename);
			}
		}
		else if (!boost::filesystem::is_directory( *ditr))
		{
			std::cerr << "ignoring file '" << filename << "'" << std::endl;
		}
	}
	std::sort( tests.begin(), tests.end());
	std::cerr << "Outputs of failed tests are written to '" << g_outputdir.string() << "'" << std::endl;

	// [2] Instantiate test cases with INSTANTIATE_TEST_CASE_P (see above)

	// [3] Execute tests:	
	WOLFRAME_GTEST_REPORT( argv[0], refpath.string());
	::testing::InitGoogleTest( &g_gtest_ARGC, g_gtest_ARGV);
	return RUN_ALL_TESTS();
}


