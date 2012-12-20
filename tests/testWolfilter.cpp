/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
#include "wolfilterCommandLine.hpp"
#include "prgbind/programLibrary.hpp"
#include "gtest/gtest.h"
#include "testDescription.hpp"
#include "moduleInterface.hpp"
#include "utils/miscUtils.hpp"
#include "processor/procProvider.hpp"
#include "prnt/pdfPrinter.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/thread/thread.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

static int g_gtest_ARGC = 0;
static char* g_gtest_ARGV[2] = {0, 0};
static boost::filesystem::path g_testdir;

using namespace _Wolframe;

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
	boost::filesystem::recursive_directory_iterator ditr( g_testdir / "wolfilter" / "data"), dend;
	if (selectedTestName.size())
	{
		std::cerr << "executing tests matching '" << selectedTestName << "'" << std::endl;
	}
	for (; ditr != dend; ++ditr)
	{
		if (boost::iequals( boost::filesystem::extension( *ditr), ".tst"))
		{
			std::string testname = boost::filesystem::basename(*ditr);
			if (selectedTestName.size())
			{
				if (std::strstr( ditr->path().string().c_str(), selectedTestName.c_str()))
				{
					std::cerr << "selected test '" << testname << "'" << std::endl;
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
	for (testno=1; itr != end; ++itr,++testno)
	{
		std::string testname = boost::filesystem::basename(*itr);
		wtest::TestDescription td( *itr, g_gtest_ARGV[0]);
		if (td.requires.size())
		{
			// [2.2] Skip tests when disabled
			std::cerr << "skipping test '" << testname << "' ( is " << td.requires << ")" << std::endl;
			continue;
		}
		// [2.3] Define I/O buffer sizes
		std::size_t ib = ibar[ testno % ibarsize];
		std::size_t ob = obar[ testno % obarsize];

		// [2.4] Parse command line in config section of the test description
		std::vector<std::string> cmd;
		std::string arg;
		std::string::const_iterator ai = td.config.begin(), ae = td.config.end();
		utils::CharTable argop( ""), argtk( "", true);
		for (; ai != ae && utils::parseNextToken( arg, ai, ae, argop, argtk); ++ai) cmd.push_back( arg);

		std::cerr << "processing test '" << testname << "'" << std::endl;
		enum {MaxNofArgs=63};
		int cmdargc = cmd.size()+1;
		char* cmdargv[MaxNofArgs+1];
		if (cmdargc > MaxNofArgs) throw std::runtime_error( "too many arguments in test");
		cmdargv[0] = strdup( g_gtest_ARGV[0] );
		for (int ci=1; ci<cmdargc; ++ci)
		{
			cmdargv[ci] = strdup( cmd[ci-1].c_str() );
		}
		boost::filesystem::path refpath( g_testdir / "temp" / "test.cfg");
		std::string outstr;
		{
			config::WolfilterCommandLine cmdline( cmdargc, cmdargv, refpath.string());

			// [2.5] Call iostreamfilter
			if (cmdline.printhelp()) std::cerr << "ignored option --help" << std::endl;
			if (cmdline.printversion()) std::cerr << "ignored option --version" << std::endl;
			if (cmdline.inputfile().size()) std::cerr << "ignored option --inputfile" << std::endl;

			db::DatabaseProvider databaseProvider( &cmdline.dbProviderConfig(), &cmdline.modulesDirectory());
			prgbind::ProgramLibrary prglib;
			proc::ProcessorProvider processorProvider( &cmdline.procProviderConfig(), &cmdline.modulesDirectory(), &prglib);
			processorProvider.resolveDB( databaseProvider);

			std::istringstream in( td.input, std::ios::in | std::ios::binary);
			std::ostringstream out( std::ios::out | std::ios::binary);

			langbind::iostreamfilter( &processorProvider, cmdline.cmd(), cmdline.inputfilter(), ib, cmdline.outputfilter(), ob, in, out);
			outstr.append( out.str());
		}
		std::vector<std::string>::const_iterator oi = td.outputfile.begin(), oe = td.outputfile.end();
		for (; oi != oe; ++oi)
		{
			std::string outfile = utils::getCanonicalPath( *oi, refpath.string());
			outstr.append( utils::readSourceFileContent( outfile));
		}

		if (td.expected != outstr)
		{
			// [2.6] Dump test contents to files in case of error
			boost::filesystem::path OUTPUT( g_testdir / "temp" / "OUTPUT");
			std::fstream oo( OUTPUT.string().c_str(), std::ios::out | std::ios::binary);
			oo.write( outstr.c_str(), outstr.size());
			if (oo.bad()) std::cerr << "error writing file '" << OUTPUT.string() << "'" << std::endl;
			oo.close();

			boost::filesystem::path EXPECT( g_testdir / "temp" / "EXPECT");
			std::fstream ee( EXPECT.string().c_str(), std::ios::out | std::ios::binary);
			ee.write( td.expected.c_str(), td.expected.size());
			if (ee.bad()) std::cerr << "error writing file '" << EXPECT.string() << "'" << std::endl;
			ee.close();

			boost::filesystem::path INPUT( g_testdir / "temp" / "INPUT");
			std::fstream ss( INPUT.string().c_str(), std::ios::out | std::ios::binary);
			ss.write( td.input.c_str(), td.input.size());
			if (ss.bad()) std::cerr << "error writing file '" << INPUT.string() << "'" << std::endl;
			ss.close();

			std::cerr << "test output does not match for '" << *itr << "'" << std::endl;
			std::cerr << "INPUT  written to file '"  << INPUT.string() << "'" << std::endl;
			std::cerr << "OUTPUT written to file '" << OUTPUT.string() << "'" << std::endl;
			std::cerr << "EXPECT written to file '" << EXPECT.string() << "'" << std::endl;

			boost::this_thread::sleep( boost::posix_time::seconds( 3));
		}
		EXPECT_EQ( td.expected, outstr);
		for (int ci=0; ci<cmdargc; ++ci)
		{
			free( cmdargv[ci] );
		}
	}
}

int main( int argc, char **argv )
{
	g_gtest_ARGC = 1;
	g_gtest_ARGV[0] = argv[0];
	g_testdir = boost::filesystem::system_complete( argv[0]).parent_path();
	int tracelevel = 0;
	int argstart = 1;

	if (argc == 2 && (std::strcmp( argv[1], "-h") == 0 || std::strcmp( argv[1], "--help") == 0))
	{
		std::cout << "Usage " << argv[0] << " [OPTION] [<test name substring>]" << std::endl;
		std::cout << "Description:" << std::endl;
		std::cout << "\tRun wolfilter with a test description loaded from a \"*.tst\" file" << std::endl;
		std::cout << "\t-h:" << " Print usage" << std::endl;
		std::cout << "\t-t:" << " Raise verbosity level (-t,-tt,-ttt,..)" << std::endl;
		return 0;
	}
	if (argc >= argstart+1)
	{
		if (argv[argstart][0] == '-' && argv[argstart][1] == 't')
		{
			tracelevel = 1;
			while (argv[argstart][tracelevel+1] == 't') ++tracelevel;
			if (argv[argstart][tracelevel+1])
			{
				std::cerr << "unknown option " << argv[argstart][tracelevel+1] << std::endl;
			}
			argstart += 1;

		}
	}
	if (argc == argstart+1)
	{
		selectedTestName = argv[argstart];
	}
	else if (argc > argstart+1)
	{
		std::cerr << "too many arguments passed to " << argv[0] << std::endl;
	}
	::testing::InitGoogleTest( &g_gtest_ARGC, g_gtest_ARGV );
	_Wolframe::log::LogLevel::Level loglevel = _Wolframe::log::LogLevel::LOGLEVEL_WARNING;
	if (tracelevel >= 1) loglevel = _Wolframe::log::LogLevel::LOGLEVEL_INFO;
	if (tracelevel >= 2) loglevel = _Wolframe::log::LogLevel::LOGLEVEL_DEBUG;
	if (tracelevel >= 3) loglevel = _Wolframe::log::LogLevel::LOGLEVEL_TRACE;
	if (tracelevel >= 4) loglevel = _Wolframe::log::LogLevel::LOGLEVEL_DATA;

	_Wolframe::log::LogBackend::instance().setConsoleLevel( loglevel);
	return RUN_ALL_TESTS();
}


