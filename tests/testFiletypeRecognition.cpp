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
///\file testFiletypeRecognition.cpp
///\brief Test the recognition of configuration file types
#include "logger-v1.hpp"
#include "gtest/gtest.h"
#include "wtest/testReport.hpp"
#include "utils/fileUtils.hpp"
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace _Wolframe;

static boost::filesystem::path g_testdir;

class FiletypeRecognitionTest
	:public ::testing::TestWithParam<std::string>
{
protected:
	FiletypeRecognitionTest() {}
	virtual ~FiletypeRecognitionTest() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};

static std::string getFileTypeString( const std::string& filename)
{
	static const char* encodingName[] = {"none","UTF-8","UCS-2BE","UCS-2LE","UCS-4BE","UCS-4LE"};
	static const char* ext[] = {".unknown",".xml",".info"};

	utils::FileType filetype = utils::getFileType( filename);
	return std::string(encodingName[ (int)filetype.encoding]) + ext[(int)filetype.format];
}

TEST_P( FiletypeRecognitionTest, test)
{
	std::string filename = GetParam();
	std::string testname = boost::filesystem::basename( filename);
		
	std::cerr << "processing test '" << testname << "'" << std::endl;

	std::string tp = getFileTypeString( filename);
	std::string expect( std::string( boost::filesystem::extension(boost::filesystem::basename(filename)).c_str() +1) + boost::filesystem::extension(filename));

	EXPECT_EQ( expect, tp);
}

static std::vector<std::string> tests;

INSTANTIATE_TEST_CASE_P(AllFiletypeRecognitionTests,
                        FiletypeRecognitionTest,
                        ::testing::ValuesIn(tests));

int main( int argc, char **argv)
{
	g_testdir = boost::filesystem::system_complete( argv[0]).parent_path();

	// [1] Selecting tests to execute:
	boost::filesystem::recursive_directory_iterator ditr( g_testdir / "doc" / "filetype"), dend;
	for (; ditr != dend; ++ditr)
	{
		if (boost::iequals( boost::filesystem::extension( *ditr), ".xml")
		||  boost::iequals( boost::filesystem::extension( *ditr), ".info")
		||  boost::iequals( boost::filesystem::extension( *ditr), ".unknown"))
		{
			std::string testname = boost::filesystem::basename(*ditr);
			tests.push_back( ditr->path().string());
		}
		else if (!boost::filesystem::is_directory( *ditr))
		{
			std::cerr << "ignoring file '" << *ditr << "'" << std::endl;
		}
	}
	std::sort( tests.begin(), tests.end());
	
	// [2] Instantiate test cases with INSTANTIATE_TEST_CASE_P (see above)

	// [3] Execute tests:
	::testing::InitGoogleTest( &argc, argv );
	WOLFRAME_GTEST_REPORT( argv[0], refpath.string());
	return RUN_ALL_TESTS();
}


