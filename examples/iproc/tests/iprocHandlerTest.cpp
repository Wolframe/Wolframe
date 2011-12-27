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
// iprocHandler class unit tests using google test framework (gTest)
//

#include "iprocHandler.hpp"
#include "connectionHandler.hpp"
#include "handlerConfig.hpp"
#include "testHandlerTemplates.hpp"
#include <gtest/gtest.h>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread/thread.hpp>

using namespace _Wolframe;
using namespace _Wolframe::iproc;

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

static const char* getRandomAsciiString( unsigned int maxStringSize=4096)
{
	enum {MaxStringSize=4096};
	if (maxStringSize > MaxStringSize || maxStringSize == 0) return 0;
	static char rt[ MaxStringSize+1];

	unsigned int ii=0,nn=rand()%maxStringSize+1;
	while (ii<nn && rand()%104 != 103)
	{
		rt[ii] = 32+rand()%96;

		//avoid random end of content:
		if (ii>=2 && rt[ii-2]=='\n' && rt[ii-1]=='.' && rt[ii]=='\n')
		{
			continue;
		}
		if (ii>=2 && rt[ii-2]=='\n' && rt[ii-1]=='.' && rt[ii]=='\r')
		{
			continue;
		}
		ii++;
	}
	//avoid random end of content:
	while (ii >= 2 && rt[ii-2]=='\n' && rt[ii-1]=='.') ii--;
	if (ii == 1 && rt[ii-1]=='.') ii=0;
	rt[ii] = 0;
	return rt;
}

template <unsigned int InputBufferSize, unsigned int OutputBufferSize>
struct TestDescription
{
	unsigned int inputBufferSize;
	unsigned int outputBufferSize;
	std::string content;

	TestDescription() :inputBufferSize(InputBufferSize),outputBufferSize(OutputBufferSize) {}
};

template <unsigned int InputBufferSize, unsigned int OutputBufferSize>
struct Empty :public TestDescription<InputBufferSize,OutputBufferSize>
{
	Empty() {}
};
template <unsigned int InputBufferSize, unsigned int OutputBufferSize>
struct OneEmptyLine :public TestDescription<InputBufferSize,OutputBufferSize>
{
	typedef TestDescription<InputBufferSize,OutputBufferSize> ThisTestDescription;
	OneEmptyLine() { ThisTestDescription::content.append("\r\n");}
};
template <unsigned int InputBufferSize, unsigned int OutputBufferSize>
struct OneOneCharLine :public TestDescription<InputBufferSize,OutputBufferSize>
{
	typedef TestDescription<InputBufferSize,OutputBufferSize> ThisTestDescription;
	OneOneCharLine() { ThisTestDescription::content.append("?\r\n");}
};
template <unsigned int InputBufferSize, unsigned int OutputBufferSize>
struct OneLine :public TestDescription<InputBufferSize,OutputBufferSize>
{
	typedef TestDescription<InputBufferSize,OutputBufferSize> ThisTestDescription;
	OneLine() { ThisTestDescription::content.append("Hello world!\r\n");}
};
template <unsigned int InputBufferSize, unsigned int OutputBufferSize, unsigned int MaxNofLines, unsigned int MaxStringSize=4096>
struct Random :public TestDescription<InputBufferSize,OutputBufferSize>
{
	typedef TestDescription<InputBufferSize,OutputBufferSize> ThisTestDescription;
	Random()
	{
		unsigned int ii=0,nn=rand()%MaxNofLines+1;
		while (ii++<=nn)
		{
			ThisTestDescription::content.append( getRandomAsciiString( MaxStringSize));
			ThisTestDescription::content.append( "\r\n");
		}
	}
};

static std::string escape( std::string& content)
{
	unsigned int ii,nn;
	std::string rt;
	for (ii=0,nn=content.size(); ii<nn; ii++)
	{
		if (content[ii] == '.' && (ii==0 || content[ii-1] == '\n'))
		{
			rt.push_back( '.');
		}
		rt.push_back( content[ii]);
	}
	return rt;
}


template <class TestDescription>
class IProcHandlerTest : public ::testing::Test
{
public:
	std::string m_input;
	std::string m_expected;
	net::LocalTCPendpoint ep;
	iproc::Connection* m_connection;
	IProcTestConfiguration m_config;
	enum
	{
		EoDBufferSize=4,
		EscBufferSize=1
	};
protected:
	IProcHandlerTest()
		:ep( "127.0.0.1", 12345)
		,m_connection(0)
		,m_config(
			(boost::filesystem::current_path() / "scripts"/ "test_echo_char.lua").string(),
			TestDescription().inputBufferSize + EoDBufferSize,
			TestDescription().outputBufferSize + EscBufferSize)
		{}

	virtual void SetUp()
	{
		TestDescription test;
		m_connection = new iproc::Connection( ep, &m_config);

		m_input.clear();
		m_expected.clear();
		m_input.append( "run\r\n");
		m_expected.append( "OK expecting command\r\n");

		m_input.append( escape( test.content));
		m_expected.append( escape( test.content));

		m_input.append( "\r\n.\r\n");
		m_expected.append( "\r\n.\r\nOK\r\n");
		m_input.append( "quit\r\n");
		m_expected.append( "BYE\r\n");
	}

	virtual void TearDown()
	{
		delete m_connection;
	}
};

typedef ::testing::Types<
	OneLine<200,200>,
	Empty<1,1>,
	OneLine<1,1>,
	OneEmptyLine<1,1>,
	OneOneCharLine<1,1>,
	Random<1,2,2000>,
	Empty<2,2>,
	OneEmptyLine<2,2>,
	OneOneCharLine<2,2>,
	OneLine<2,2>,
	Random<2,2,2000>,
	Empty<3,3>,
	OneEmptyLine<3,3>,
	OneOneCharLine<3,3>,
	OneLine<3,3>,
	Random<3,3,2000>,
	Empty<4,4>,
	OneEmptyLine<4,4>,
	OneOneCharLine<4,4>,
	OneLine<4,4>,
	Random<4,4,8000>,
	Empty<1,2>,
	OneEmptyLine<1,2>,
	OneOneCharLine<1,2>,
	OneLine<1,2>,
	Random<3,2,1000>,
	Empty<2,3>,
	OneEmptyLine<2,3>,
	OneOneCharLine<2,3>,
	OneLine<2,3>,
	Random<2,3,1000>,
	OneEmptyLine<3,4>,
	OneOneCharLine<3,4>,
	OneLine<3,4>,
	Empty<2,1>,
	OneEmptyLine<2,1>,
	OneOneCharLine<2,1>,
	OneLine<2,1>,
	Random<1,3,1000>,
	Empty<3,2>,
	OneEmptyLine<3,2>,
	OneOneCharLine<3,2>,
	OneLine<3,2>,
	Random<3,2,1000>,
	Empty<4,3>,
	OneEmptyLine<4,3>,
	OneLine<4,3>,
	OneOneCharLine<127,127>,
	Random<2,7,4000,32>,
	Random<7,2,4000,32>
> MyTypes;

TYPED_TEST_CASE( IProcHandlerTest, MyTypes);

TYPED_TEST( IProcHandlerTest, ExpectedResult )
{
	std::string output;
	char* itr = const_cast<char*>( this->m_input.c_str());
	EXPECT_EQ( 0, test::runTestIO( itr, itr+this->m_input.size(), output, *this->m_connection));

#define _Wolframe_LOWLEVEL_DEBUG
#ifdef _Wolframe_LOWLEVEL_DEBUG
	unsigned int ii=0,nn=output.size();
	for (;ii<nn && output[ii]==this->m_expected[ii]; ii++);
	if (ii != nn)
	{
		printf( "SIZE R=%lu,E=%lu,DIFF AT %u='%d %d %d %d|%d %d %d %d'\n",
			(unsigned long)output.size(), (unsigned long)this->m_expected.size(), ii,
			output[ii-2],output[ii-1],output[ii-0],output[ii+1],
			this->m_expected[ii-2],this->m_expected[ii-1],this->m_expected[ii-0],this->m_expected[ii+1]);
		boost::this_thread::sleep( boost::posix_time::seconds( 5 ));
	}
#endif
#ifndef _WIN32
	EXPECT_EQ( this->m_expected, output);
#else
	EXPECT_EQ( m_expected, output);
#endif
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	_Wolframe::log::LogBackend::instance().setConsoleLevel( _Wolframe::log::LogLevel::LOGLEVEL_INFO );
	return RUN_ALL_TESTS();
}

