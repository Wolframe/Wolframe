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
///\file wtest/testReport.hpp
///\brief Interface to a minimal test report written for tests based on gtest
#ifndef _Wolframe_wtest_TESTREPORT_HPP_INCLUDED
#define _Wolframe_wtest_TESTREPORT_HPP_INCLUDED
#include <string>
#include <cstring>
#include <vector>
#include <errno.h>
#include "gtest/gtest.h"

namespace _Wolframe {
namespace wtest {

// Provides alternative output mode which produces minimal amount of
// information about tests.
class TestReportListener
	:public ::testing::EmptyTestEventListener
{
public:
	TestReportListener( const std::string& testprg, const std::string& report_filename)
		:m_count_failure(0),m_count_success(0),m_state(Init)
	{
		const char* nam = testprg.c_str();
		while (std::strchr( nam, '/') != 0) nam = std::strchr( nam, '/')+1;
		while (std::strchr( nam, '\\') != 0) nam = std::strchr( nam, '\\')+1;
		m_testprg = nam;
		const char* ext = std::strchr( m_testprg.c_str(), '.');
		if (ext) m_testprg.resize( ext - m_testprg.c_str());

#ifdef _WIN32
		errno_t err;
		FILE* fh;
		err = fopen_s( &fh, report_filename.c_str(), "a+");
		if (!err)
#else
		FILE* fh = fopen( report_filename.c_str(), "a+");
#endif
		if (!fh)
		{
			throw std::runtime_error( std::string( "failed (errno " + boost::lexical_cast<std::string>(errno) + ") to open file ") + report_filename + "' for appending");
		}
		m_outfile = boost::shared_ptr<FILE>( fh, fclose);
	}
	~TestReportListener()
	{
		const char* status = (m_state == Terminated && m_count_failure==0)?"OK":"ERROR";
		fprintf( m_outfile.get(), "%s %s\n", m_testprg.c_str(), status);
		fflush( m_outfile.get());
	}

private:
	// Called before any test activity starts.
	virtual void OnTestProgramStart(const ::testing::UnitTest& /* unit_test */)
	{
		m_state = Init;
	}

	// Called after all test activities have ended.
	virtual void OnTestProgramEnd(const ::testing::UnitTest& /* unit_test */)
	{
		m_state = Terminated;
	}

	// Called before a test starts.
	virtual void OnTestStart(const ::testing::TestInfo& test_info)
	{
		m_state = Start;
	}

	// Called after a failed assertion or a SUCCEED() invocation.
	virtual void OnTestPartResult( const ::testing::TestPartResult& test_part_result)
	{
		if (test_part_result.failed())
		{
			m_count_failure += 1;
		}
		else
		{
			m_count_success += 1;
		}
	}

	// Called after a test ends.
	virtual void OnTestEnd( const ::testing::TestInfo& test_info)
	{
		m_state = End;
	}

private:
	boost::shared_ptr<FILE> m_outfile;
	unsigned int m_count_failure;
	unsigned int m_count_success;
	enum State {Init,Start,End,Terminated};
	State m_state;
	std::string m_testprg;
};
}}//namespace

#define WOLFRAME_GTEST_REPORT(prgfilename,reportfilename)\
	::testing::UnitTest& unit_test = *::testing::UnitTest::GetInstance();\
	::testing::TestEventListeners& listeners = unit_test.listeners();\
	listeners.Append( new wtest::TestReportListener(prgfilename,reportfilename));

#endif


