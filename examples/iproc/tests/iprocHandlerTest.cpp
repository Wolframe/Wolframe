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
#include "langbind/luaConfig.hpp"
#include "handlerConfig.hpp"
#include "testHandlerTemplates.hpp"
#include <gtest/gtest.h>
#include <stdlib.h>
#include <boost/lexical_cast.hpp>

using namespace _Wolframe;
using namespace iproc;

struct TestConfiguration :public lua::Configuration
{
	TestConfiguration()
		:lua::Configuration( "iproc", "test-iproc"){}
	TestConfiguration( const TestConfiguration& o) :lua::Configuration(o){}
	TestConfiguration( int bufferSizeInput, int bufferSizeOutput)
		:lua::Configuration( "iproc", "test-iproc")
	{
		boost::property_tree::ptree pt;
		pt.put("main", "test.lua");
		pt.put("input_buffer", boost::lexical_cast<std::string>( bufferSizeOutput));
		pt.put("output_buffer", boost::lexical_cast<std::string>( bufferSizeInput));
		setCanonicalPathes( ".");
		if (!parse( pt, "test")) throw std::logic_error( "Bad Configuration");
		setCanonicalPathes( ".");
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

	TestDescription() :inputBufferSize(InputBufferSize),outputBufferSize(OutputBufferSize) {};
};

template <unsigned int InputBufferSize, unsigned int OutputBufferSize>
struct Empty :public TestDescription<InputBufferSize,OutputBufferSize>
{
	Empty() {};
};
template <unsigned int InputBufferSize, unsigned int OutputBufferSize>
struct OneEmptyLine :public TestDescription<InputBufferSize,OutputBufferSize>
{
	typedef TestDescription<InputBufferSize,OutputBufferSize> ThisTestDescription;
	OneEmptyLine() { ThisTestDescription::content.append("\r\n");};
};
template <unsigned int InputBufferSize, unsigned int OutputBufferSize>
struct OneOneCharLine :public TestDescription<InputBufferSize,OutputBufferSize>
{
	typedef TestDescription<InputBufferSize,OutputBufferSize> ThisTestDescription;
	OneOneCharLine() { ThisTestDescription::content.append("?\r\n");};
};
template <unsigned int InputBufferSize, unsigned int OutputBufferSize>
struct OneLine :public TestDescription<InputBufferSize,OutputBufferSize>
{
	typedef TestDescription<InputBufferSize,OutputBufferSize> ThisTestDescription;
	OneLine() { ThisTestDescription::content.append("Hello world!\r\n");};
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
			continue;
		}
		rt.push_back( content[ii]);
	}
	return rt;
}


template <class TestDescription>
class iprocHandlerFixture : public ::testing::Test
{
public:
	std::string input;
	std::string expected;
	net::LocalTCPendpoint ep;
	iproc::Connection* connection;
	TestConfiguration config;
protected:
	iprocHandlerFixture() :ep( "127.0.0.1", 12345),connection(0) {}

	virtual void SetUp()
	{
		TestDescription test;
		config = TestConfiguration( test.inputBufferSize, test.outputBufferSize);
		connection = new iproc::Connection( ep, &config);

		input.clear();
		expected.clear();
		input.append( "run\r\n");
		expected.append( "OK expecting command\r\n\r\n");

		input.append( test.content);
		expected.append( escape( test.content));

		input.append( ".\r\n");
		expected.append( "\r\n\r\n.\r\nOK\r\n");
		input.append( "quit\r\n");
		expected.append( "BYE\r\n");
	}

	virtual void TearDown()
	{
		delete connection;
	}
};

typedef ::testing::Types<
	OneLine<200,200>,
	Empty<1,1>,
	OneEmptyLine<1,1>,
	OneOneCharLine<1,1>,
	OneLine<1,1>,
	Random<1,1,2000>,
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
	Random<1,2,1000>,
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
	Random<2,1,1000>,
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

TYPED_TEST_CASE( iprocHandlerFixture, MyTypes);

TYPED_TEST( iprocHandlerFixture, ExpectedResult )
{
	std::string output;
	char* itr = const_cast<char*>( this->input.c_str());
	EXPECT_EQ( 0, test::runTestIO( itr, output, *this->connection));
#define _Wolframe_LOWLEVEL_DEBUG
#ifdef _Wolframe_LOWLEVEL_DEBUG
	unsigned int ii=0,nn=output.size();
	for (;ii<nn && output[ii]==this->expected[ii]; ii++);
	if (ii != nn)
	{
		printf( "SIZE R=%lu,E=%lu,DIFF AT %u='%d %d %d %d|%d %d %d %d'\n",
			(unsigned long)output.size(), (unsigned long)this->expected.size(), ii,
			output[ii-2],output[ii-1],output[ii-0],output[ii+1],
			this->expected[ii-2],this->expected[ii-1],this->expected[ii-0],this->expected[ii+1]);
	}
#endif
	EXPECT_EQ( output, this->expected);
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	_Wolframe::log::LogBackend::instance().setConsoleLevel( _Wolframe::log::LogLevel::LOGLEVEL_INFO );
	return RUN_ALL_TESTS();
}

