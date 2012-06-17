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
///\\file iprocHandlerXMLTest.cpp class unit tests using google test framework (gTest)

#include "iprocHandler.hpp"
#include "connectionHandler.hpp"
#include "appConfig.hpp"
#include "handlerConfig.hpp"
#include "langbind/appGlobalContext.hpp"
#include "moduleInterface.hpp"
#include "config/ConfigurationTree.hpp"
#include "testHandlerTemplates.hpp"
#include "testUtils.hpp"
#include <iostream>
#include <list>
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <stdio.h>
#include <boost/lexical_cast.hpp>
#include "gtest/gtest.h"

using namespace _Wolframe;
using namespace _Wolframe::iproc;

struct TestDescription
{
	const char* name;			///< determines the name of the result and of the expected result file
	const char* scriptfile;			///< script to execute
	const char* datafile;			///< input to feed
	unsigned int elementBuffersize;		///< additional buffer for the output to hold at least one result element
};

enum
{
	EoDBufferSize = 4			///< size of buffer needed for end of data marker
};

static const TestDescription testDescriptions[] =
{
	{
		"echo_xml_IsoLatin1",
		"test_echo_xml.lua",
		"test_IsoLatin1.xml", 32
	},
#ifdef WITH_LIBXML2
	{
		"echo_xml_utf8_with_libxml",
		"test_echo_xml_libxml2.lua",
		"test_UTF8.xml", 48
	},
#endif
	{
		"echo_xml_UCS4",
		"test_echo_xml.lua",
		"test_UCS4BE.xml", 128
	},
	{
		"echo_xml_IsoLatin1_to_utf16",
		"test_echo_xml_utf16.lua",
		"test_IsoLatin1.xml", 96
	},
	{
		"echo_xml_utf16_to_utf8",
		"test_echo_xml_utf8.lua",
		"test_UTF16.xml", 64
	},
	{
		"echo_xml_IsoLatin1_to_utf8",
		"test_echo_xml_utf8.lua",
		"test_IsoLatin1.xml", 32
	},
	{
		"echo_char_IsoLatin1",
		"test_echo_char.lua",
		"test_IsoLatin1.xml", 1
	},
	{0,0,0,0}
};

static int g_gtest_ARGC = 0;
static char* g_gtest_ARGV[2] = {0, 0};

class IProcTestConfiguration :public Configuration
{
public:
	IProcTestConfiguration( const IProcTestConfiguration& o)
		:Configuration(o)
		,m_appConfig(o.m_appConfig)
		,m_langbindConfig(o.m_langbindConfig)
	{}
	IProcTestConfiguration( const std::string& scriptpath, std::size_t ib, std::size_t ob)
	{
		m_appConfig.addConfig( "proc", this);
		m_appConfig.addConfig( "env", &m_langbindConfig);

		boost::filesystem::path configFile( boost::filesystem::current_path() / "temp" / "test.cfg");
		std::ostringstream config;
		config << "env {" << std::endl;
		config << "   script {" << std::endl;
		config << "      name run" << std::endl;
		config << "      sourcepath \"" << scriptpath << "\"" << std::endl;
		config << "   }" << std::endl;
		config << "}" << std::endl;
		config << "proc {" << std::endl;
		config << "   cmd run" << std::endl;
		config << "}" << std::endl;
		wtest::Data::writeFile( configFile.string().c_str(), config.str());

		if (boost::filesystem::exists( configFile))
		{
			if (!m_appConfig.parse( configFile.string().c_str(), config::ApplicationConfiguration::CONFIG_INFO))
			{
				throw std::runtime_error( "Error in configuration");
			}
		}
		m_appConfig.finalize();

		setBuffers( ib, ob);
		langbind::defineGlobalContext( new langbind::GlobalContext());
		langbind::getGlobalContext()->load( m_langbindConfig);
	}

private:
	config::ApplicationConfiguration m_appConfig;
	langbind::ApplicationEnvironmentConfig m_langbindConfig;
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
		wtest::Data data( testDescriptions[ti].name, testDescriptions[ti].datafile);

		for (unsigned int ib=0; ib<NofBufferSizes; ib++)
		{
			for (unsigned int ob=0; ob<NofBufferSizes; ob++)
			{
				std::string testoutput;
				std::string scriptpath( "../scripts/");
				scriptpath.append( testDescriptions[ti].scriptfile);

				IProcTestConfiguration config(
						scriptpath, BufferSize[ib]+EoDBufferSize,
						BufferSize[ob]);
				iproc::Connection connection( ep, &config);

				EXPECT_EQ( 0, test::runTestIO( data.input, testoutput, connection));
				data.check( testoutput);
				ASSERT_EQ( data.expected, testoutput);
			}
		}
	}
}

int main( int argc, char **argv )
{
	g_gtest_ARGC = 1;
	g_gtest_ARGV[0] = argv[0];
	if (argc > 1)
	{
		std::cerr << "too many arguments passed to " << argv[0] << std::endl;
		return 1;
	}
	wtest::Data::createDataDir( "temp");
	wtest::Data::createDataDir( "result");
	::testing::InitGoogleTest( &g_gtest_ARGC, g_gtest_ARGV );
	return RUN_ALL_TESTS();
}


