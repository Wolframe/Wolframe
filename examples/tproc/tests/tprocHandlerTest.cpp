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
//
// tprocHandler class unit tests using google test framework (gTest)
//

#include "tprocHandler.hpp"
#include "connectionHandler.hpp"
#include "handlerConfig.hpp"
#include "testHandlerTemplates.hpp"
#include <gtest/gtest.h>

using namespace _Wolframe;
using namespace _Wolframe::tproc;

class TestConfiguration :public Configuration
{
public:
	TestConfiguration( const TestConfiguration& o)
		:Configuration(o)
	{}

	TestConfiguration( std::size_t ib, std::size_t ob)
	{
		m_data.input_bufsize = ib;
		m_data.output_bufsize = ob;
	}
};

struct TestDescription
{
	const char* input;
	const char* expected;
};
static TestDescription testDescription[] =
{
	{"CMD1A\r\nCMD1A 'hi arg'\r\nCMD2A 'huga'\r\nCMD3A 1\r\n", "OK CMD1A ?\r\nOK CMD1A 'hi arg'\r\nOK CMD2A 'huga'\r\nOK CMD3A '1'\r\n"},
	{0,0}
};

class TProcHandlerTest : public ::testing::Test
{
protected:
	TProcHandlerTest() {}
	virtual ~TProcHandlerTest() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};

class TProcHandlerTestInstance
{
private:
	net::LocalTCPendpoint ep;
	tproc::Connection* m_connection;
	TestConfiguration m_config;
	std::string m_input;
	std::string m_output;
	std::string m_expected;
	std::size_t m_inputBufferSize;
	std::size_t m_outputBufferSize;

	enum
	{
		EoDBufferSize=4,
		MinOutBufferSize=16
	};
public:
	TProcHandlerTestInstance( const TestDescription& descr, std::size_t ib, std::size_t ob)
		:ep( "127.0.0.1", 12345)
		,m_connection(0)
		,m_config( ib + EoDBufferSize, ob + MinOutBufferSize)
		,m_input( descr.input)
		,m_expected( descr.expected)
		,m_inputBufferSize(ib)
		,m_outputBufferSize(ob)
		{
			m_connection = new tproc::Connection( ep, &m_config);
		}

	~TProcHandlerTestInstance()
	{
		delete m_connection;
	}

	const std::string& input() const {return m_input;}
	const std::string& output() const {return m_output;}
	const std::string& expected() const {return m_expected;}

	int run()
	{
		char* itr = const_cast<char*>( m_input.c_str());
		m_output.clear();
		return test::runTestIO( itr, itr+m_input.size(), m_output, *m_connection);
	}
};

TEST_F( TProcHandlerTest, tests)
{
	unsigned int ti;
	enum {NOF_IB=8,NOF_OB=3};
	std::size_t ib[ NOF_IB] = {1,2,3,5,11,13,23,127};
	std::size_t ob[ NOF_OB] = {1,2,3};

	for (ti=0; testDescription[ti].input; ti++)
	{
		for (int ii=0; ii<NOF_IB; ii++)
		{
			for (int oo=0; oo<NOF_OB; oo++)
			{
				TProcHandlerTestInstance test( testDescription[ti], ib[ii], ob[oo]);
				EXPECT_EQ( 0, test.run());
				EXPECT_EQ( test.expected(), test.output());
			}
		}
	}
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	_Wolframe::log::LogBackend::instance().setConsoleLevel( _Wolframe::log::LogLevel::LOGLEVEL_INFO );
	return RUN_ALL_TESTS();
}

