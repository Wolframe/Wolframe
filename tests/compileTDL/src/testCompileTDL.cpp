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
#include "database/loadTransactionProgram.hpp"
#include "database/databaseLanguage.hpp"
#include "database/vmTransactionInput.hpp"
#include "transactionfunction/InputStructure.hpp"
#include "utils/fileUtils.hpp"
#include "types/propertyTree.hpp"
#include "filter/ptreefilter.hpp"
#include "logger-v1.hpp"
#include "gtest/gtest.h"
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
static LanguageDescription g_dblang;
static std::string selectedTestName;

class CompileTDLTest
	:public ::testing::Test
{
protected:
	CompileTDLTest() {}
	virtual ~CompileTDLTest() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};

static void fillInputStructure( const types::PropertyTree& ptree, tf::InputStructure& input)
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

TEST_F( CompileTDLTest, tests)
{
	std::vector<std::string> tests;
	std::size_t testno;
	boost::filesystem::path outputdir( g_testdir / ".." / "output");

	// [1] Selecting tests to execute:
	if (selectedTestName.size())
	{
		std::cerr << "executing tests matching '" << selectedTestName << "'" << std::endl;
	}
	boost::filesystem::recursive_directory_iterator ditr( g_testdir / ".." / "data" ), dend;
	for (; ditr != dend; ++ditr)
	{
		std::string filename = utils::resolvePath( ditr->path().string());
		if (boost::iequals( boost::filesystem::extension( *ditr), ".tdl"))
		{
			std::string testname = utils::getFileStem( filename);
			if (selectedTestName.size())
			{
				if (std::strstr( testname.c_str(), selectedTestName.c_str()))
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
	std::cerr << "Outputs of failed tests are written to '" << outputdir.string() << "'" << std::endl;

	// [2] Execute tests:
	std::vector<std::string>::const_iterator itr=tests.begin(),end=tests.end();
	for (testno=1; itr != end; ++itr,++testno)
	{
		std::string testname = utils::getFileStem( *itr);
		std::string testdir = utils::getParentPath( *itr);
		std::string tdlfile = testdir + "/" + testname + ".tdl";
		std::string expectfile = testdir + "/" + testname + ".res";
		std::string inputfile = testdir + "/" + testname + ".inp";

		// [2.1] Process test:
		std::cerr << "processing test '" << testname << "'" << std::endl;

		TdlTransactionFunctionList tl = loadTransactionProgramFile2( tdlfile, "testdb", "test", &g_dblang);
		TdlTransactionFunctionList::const_iterator ti = tl.begin(), te = tl.end();

		// [2.2] Print test output to string:
		std::ostringstream out;
		for (; ti != te; ++ti)
		{
			ti->second->print( out);
			out << std::endl;
		}

		// [2.3] Create transaction function from program with input and
		//	print it, if there is an input file defined:
		if (utils::fileExists( inputfile))
		{
			types::PropertyTree input_ptree = utils::readPropertyTreeFile( inputfile);
			for (ti = tl.begin(); ti != te; ++ti)
			{
				tf::InputStructure input( ti->second->program()->pathset.tagtab());
				fillInputStructure( input_ptree, input);

				VmTransactionInput trsinput( *ti->second->program(), input);
				out << "TRANSACTION INPUT:" << std::endl;
				trsinput.print( out);
				out << std::endl;
			}
		}

		// [2.4] Compare result and write dump to the output directory
		//	if not equal to expected:
		bool file_read_exception = false;
		std::string output = out.str();
		std::string expect;
		try
		{
			expect = utils::readSourceFileContent( expectfile);
		}
		catch (const std::runtime_error& e)
		{
			file_read_exception = true;
			std::cerr << "failed to read file with expected content: " << e.what() << std::endl;
		}
		if (file_read_exception || expect != output)
		{
			boost::filesystem::path outputdumpfile( outputdir / (testname + ".res"));
			utils::writeFile( outputdumpfile.string(), output);
		}
		EXPECT_EQ( expect, output);
	}
}


int main( int argc, char **argv)
{
	g_gtest_ARGC = 1;
	g_gtest_ARGV[0] = argv[0];
	g_testdir = boost::filesystem::system_complete( argv[0]).parent_path();

	if (argc >= 2)
	{
		if (std::strcmp( argv[1], "-h") == 0 || std::strcmp( argv[1], "--help") == 0)
		{
			std::cerr << argv[0] << " (no arguments)" << std::endl;
			return 0;
		}
		else if (argc == 2)
		{
			selectedTestName = argv[ 1];
		}
		else
		{
			std::cerr << "Too many arguments (expected no arguments)" << std::endl;
			return 1;
		}
	}
	::testing::InitGoogleTest( &g_gtest_ARGC, g_gtest_ARGV);
	return RUN_ALL_TESTS();
}


