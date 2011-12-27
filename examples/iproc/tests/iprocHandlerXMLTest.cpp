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
///\file iprocHandlerXMLTest.cpp class unit tests using google test framework (gTest)

#include "iprocHandler.hpp"
#include "connectionHandler.hpp"
#include "handlerConfig.hpp"
#include "testHandlerTemplates.hpp"
#include "testUtils.hpp"
#include <iostream>
#include <list>
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <stdio.h>
#include <boost/lexical_cast.hpp>
#include <gtest/gtest.h>

using namespace _Wolframe;
using namespace _Wolframe::iproc;

struct TestDescription
{
	const char* name;				///< determines the name of the result and of the expected result file
	const char* scriptfile;			///< script to execute
	const char* datafile;			///< input to feed
	unsigned int elementBuffersize;	///< additional buffer for the output to hold at least one result element
};

enum
{
	EoDBufferSize = 4		///< size of buffer needed for end of data marker
};

static const TestDescription testDescriptions[] =
{
	{
		"echo_xml_IsoLatin1_to_utf16",
		"test_echo_xml_utf16",
		"test_IsoLatin1.xml", 96
	},
	{
		"echo_xml_utf16_to_utf8",
		"test_echo_xml_utf8",
		"test_UTF16.xml", 64
	},
#ifdef WITH_LIBXML2
	{
		"echo_xml_utf8_with_libxml",
		"test_echo_xml_libxml2",
		"test_UTF8.xml", 48
	},
#endif
	{
		"echo_xml_IsoLatin1_to_utf8",
		"test_echo_xml_utf8",
		"test_IsoLatin1.xml", 32
	},
	{
		"echo_xml_IsoLatin1",
		"test_echo_xml",
		"test_IsoLatin1.xml", 32
	},
	{
		"echo_char_IsoLatin1",
		"test_echo_char",
		"test_IsoLatin1.xml", 1
	},
	{0,0,0,0}
};

class IProcTestConfiguration :public Configuration
{
public:
	IProcTestConfiguration ( const std::string& scriptpath, std::size_t ib, std::size_t ob)
	{
		m_data.input_bufsize = ib;
		m_data.output_bufsize = ob;
		ScriptConfigStruct sc;
		sc.name = "run";
		sc.main = "run";
		sc.path = scriptpath;
		if (!defineScript( sc)) throw std::logic_error( "cannot define test configuration");
	}
};

class IProcHandlerXMLTest : public ::testing::Test
{
protected:
	IProcHandlerXMLTest() {}
	virtual ~IProcHandlerXMLTest() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};

TEST_F( IProcHandlerXMLTest, tests)
{
	unsigned int ti;
	for (ti=0; testDescriptions[ti].scriptfile; ti++)
	{
		enum {NofBufferSizes=5};
		static int BufferSize[ NofBufferSizes] = {2,3,5,7,127};

		net::LocalTCPendpoint  ep( "127.0.0.1", 12345);
		iproc::Connection* connection = 0;
		wtest::Data data( testDescriptions[ti].name, testDescriptions[ti].datafile);

		for (unsigned int ib=0; ib<NofBufferSizes; ib++)
		{
			for (unsigned int ob=0; ob<NofBufferSizes; ob++)
			{
				std::string testoutput;

				IProcTestConfiguration config(
						testDescriptions[ti].scriptfile,
						BufferSize[ib]+EoDBufferSize,
						BufferSize[ob]+testDescriptions[ti].elementBuffersize);
				connection = new iproc::Connection( ep, &config);

				char* in_start = const_cast<char*>(data.input.c_str());
				char* in_end = const_cast<char*>(data.input.c_str() + data.input.size());

				EXPECT_EQ( 0, test::runTestIO( in_start, in_end, testoutput, *connection));
				data.check( testoutput);
				ASSERT_EQ( data.expected, testoutput);
			}
		}
	}
}

int main( int argc, char **argv )
{
	wtest::Data::createDataDir( "result");
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}


