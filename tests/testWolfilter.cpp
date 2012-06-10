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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file testWolfilter.cpp
///\brief Test program for wolfilter like stdin/stdout mapping

#include "langbind/iostreamfilter.hpp"
#include "logger-v1.hpp"
#include "langbind/appGlobalContext.hpp"
#include "wolfilterCommandLine.hpp"
#include "wolfilter/src/employee_assignment_print.hpp"
#include "gtest/gtest.h"
#include "testDescription.hpp"
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/thread/thread.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

///\remark Hack for linking this stuff to the test program. Cannot do it in the makefile unfortunately
#include "wolfilter/src/employee_assignment_print.cpp"

static int g_gtest_ARGC = 0;
static char* g_gtest_ARGV[2] = {0, 0};

using namespace _Wolframe;

///\brief Loads the modules, scripts, etc. defined hardcoded and in the command line into the global context
static void loadGlobalContext( const config::WolfilterCommandLine& cmdline)
{
	langbind::GlobalContext* gct = langbind::getGlobalContext();

	gct->defineFormFunction( "employee_assignment_convert",
					langbind::FormFunction(
						test::convertAssignmentListDoc,
						test::AssignmentListDoc::getFiltermapDescription(),
						test::AssignmentListDoc::getFiltermapDescription()));
	std::vector<std::string>::const_iterator itr = cmdline.scripts().begin(), end = cmdline.scripts().end();
	for (; itr != end; ++itr)
	{
		boost::filesystem::path scriptpath( boost::filesystem::current_path() / "temp" / *itr);
		langbind::LuaScript script( scriptpath.string());
		std::vector<std::string>::const_iterator fi = script.functions().begin(), fe = script.functions().end();
		for (; fi != fe; ++fi)
		{
			gct->defineLuaFunction( *fi, script);
		}
	}
}

class WolfilterTest : public ::testing::Test
{
protected:
	WolfilterTest() {}
	virtual ~WolfilterTest() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};

static std::string selectedTestName;

TEST_F( WolfilterTest, tests)
{
	enum {ibarsize=11,obarsize=7,EoDBufferSize=4};
	std::size_t ibar[ibarsize] = {127,2,3,5,7,11,13,17,19,23,41};
	std::size_t obar[obarsize] = {127,4,5,7,11,13,17};
	std::vector<std::string> tests;
	std::size_t testno;

	// [1] Selecting tests to execute:
	boost::filesystem::recursive_directory_iterator ditr( boost::filesystem::current_path() / "wolfilter" / "data"), dend;
	if (selectedTestName.size())
	{
		std::cerr << "executing tests matching '" << selectedTestName << "'" << std::endl;
	}
	for (; ditr != dend; ++ditr)
	{
		if (boost::iequals( boost::filesystem::extension( *ditr), ".tst"))
		{
			if (selectedTestName.size())
			{
				if (std::search( ditr->path().string().begin(), ditr->path().string().end(), selectedTestName.begin(), selectedTestName.end()) != ditr->path().string().end())
				{
					std::cerr << "selected test '" << *ditr << "'" << std::endl;
					tests.push_back( ditr->path().string());
				}
			}
			else
			{
				tests.push_back( ditr->path().string());
			}
		}
		else if (!boost::filesystem::is_directory( *ditr))
		{
			std::cerr << "ignoring file '" << *ditr << "'" << std::endl;
		}
	}
	std::sort( tests.begin(), tests.end());

	// [2] Execute tests:
	std::vector<std::string>::const_iterator itr=tests.begin(),end=tests.end();
	for (testno=0; itr != end; ++itr,++testno)
	{
		// [2.1] Remove old temporary files:
		boost::filesystem::path tempdir( boost::filesystem::current_path() / "temp");
		if (boost::filesystem::exists( tempdir) && boost::filesystem::is_directory( tempdir))
		{
			boost::filesystem::remove_all( tempdir);
			boost::filesystem::create_directory( tempdir);
		}
		wtest::TestDescription td( *itr);
		if (td.requires.size())
		{
			// [2.2] Skip tests when disabled
			std::cerr << "skipping test '" << *itr << "' ( is " << td.requires << ")" << std::endl;
			continue;
		}
		// [2.3] Define I/O buffer sizes
		std::size_t ib = ibar[ testno % ibarsize];
		std::size_t ob = obar[ testno % obarsize];

		// [2.4] Parse command line in config section of the test description
		std::vector<std::string> cmd;
		std::string cmdstr( td.config);
		boost::algorithm::trim( cmdstr);
		{
			std::vector<std::string> cmd_e;
			boost::split( cmd_e, cmdstr, boost::is_any_of("\n\t\r "));
			std::vector<std::string>::const_iterator vi=cmd_e.begin(), ve=cmd_e.end();
			for (; vi != ve; ++vi) if (!vi->empty()) cmd.push_back( *vi);
		}
		std::cerr << "processing test '" << *itr << "'" << std::endl;
		enum {MaxNofArgs=31};
		int cmdargc = cmd.size()+1;
		const char* cmdargv[MaxNofArgs+1];
		if (cmdargc > MaxNofArgs) throw std::runtime_error( "too many arguments in test");
		cmdargv[0] = g_gtest_ARGV[0];
		for (int ci=1; ci<cmdargc; ++ci)
		{
			cmdargv[ci] = cmd[ci-1].c_str();
		}
		config::WolfilterCommandLine cmdline( cmdargc, cmdargv);

		// [2.5] Call iostreamfilter
		if (cmdline.printhelp()) std::cerr << "ignored option --help" << std::endl;
		if (cmdline.printversion()) std::cerr << "ignored option --version" << std::endl;
		if (cmdline.inputfile().size()) std::cerr << "ignored option --inputfile" << std::endl;

		loadGlobalContext( cmdline);

		std::istringstream in( td.input, std::ios::in | std::ios::binary);
		std::ostringstream out( std::ios::out | std::ios::binary);

		bool trt = langbind::iostreamfilter( cmdline.cmd(), cmdline.inputfilter(), ib, cmdline.outputfilter(), ob, in, out);
		if (!trt)
		{
			boost::this_thread::sleep( boost::posix_time::seconds( 1 ) );
			EXPECT_EQ( true, trt);
			continue;
		}
		EXPECT_EQ( true, trt);
		if (td.expected != out.str())
		{
			// [2.6] Dump test contents to files in case of error
			boost::filesystem::path OUTPUT( boost::filesystem::current_path() / "temp" / "OUTPUT");
			std::fstream oo( OUTPUT.string().c_str(), std::ios::out | std::ios::binary);
			oo.write( out.str().c_str(), out.str().size());
			if (oo.bad()) std::cerr << "error writing file '" << OUTPUT.string() << "'" << std::endl;
			oo.close();

			boost::filesystem::path EXPECT( boost::filesystem::current_path() / "temp" / "EXPECT");
			std::fstream ee( EXPECT.string().c_str(), std::ios::out | std::ios::binary);
			ee.write( td.expected.c_str(), td.expected.size());
			if (ee.bad()) std::cerr << "error writing file '" << EXPECT.string() << "'" << std::endl;
			ee.close();

			boost::filesystem::path INPUT( boost::filesystem::current_path() / "temp" / "INPUT");
			std::fstream ss( INPUT.string().c_str(), std::ios::out | std::ios::binary);
			ss.write( td.input.c_str(), td.input.size());
			if (ss.bad()) std::cerr << "error writing file '" << INPUT.string() << "'" << std::endl;
			ss.close();

			std::cerr << "test output does not match for '" << *itr << "'" << std::endl;
			std::cerr << "INPUT  written to file '"  << INPUT.string() << "'" << std::endl;
			std::cerr << "OUTPUT written to file '" << OUTPUT.string() << "'" << std::endl;
			std::cerr << "EXPECT written to file '" << EXPECT.string() << "'" << std::endl;
			boost::this_thread::sleep( boost::posix_time::seconds( 3 ) );
		}
		EXPECT_EQ( td.expected, out.str());
	}
}

int main( int argc, char **argv )
{
	g_gtest_ARGC = 1;
	g_gtest_ARGV[0] = argv[0];
	if (argc > 2)
	{
		std::cerr << "too many arguments passed to " << argv[0] << std::endl;
		return 1;
	}
	else if (argc == 2)
	{
		selectedTestName = argv[1];
	}
	::testing::InitGoogleTest( &g_gtest_ARGC, g_gtest_ARGV );
	_Wolframe::log::LogBackend::instance().setConsoleLevel( _Wolframe::log::LogLevel::LOGLEVEL_INFO );
	return RUN_ALL_TESTS();
}


