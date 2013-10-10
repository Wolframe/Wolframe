/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
// pechoHandler class unit tests using google test framework (gTest)
//


#include "pechoHandler.hpp"
#include "system/connectionHandler.hpp"
#include "wtest/testHandlerTemplates.hpp"
#include "gtest/gtest.h"
#include <stdlib.h>
#include <boost/thread/thread.hpp>

using namespace _Wolframe;

static const char* getRandomAsciiString()
{
	enum {MaxStringSize=4096};
	static char rt[ MaxStringSize+1];

	unsigned int ii=0,nn=rand()%MaxStringSize+1;
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
	OneEmptyLine() { ThisTestDescription::content.append("");};
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
template <unsigned int InputBufferSize, unsigned int OutputBufferSize, unsigned int MaxNofLines>
struct Random :public TestDescription<InputBufferSize,OutputBufferSize>
{
	typedef TestDescription<InputBufferSize,OutputBufferSize> ThisTestDescription;
	Random()
	{
		unsigned int ii=0,nn=rand()%MaxNofLines+1;
		while (ii++<=nn)
		{
			ThisTestDescription::content.append( getRandomAsciiString());
			ThisTestDescription::content.append( "\r\n\r\n");
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
class pechoHandlerFixture : public ::testing::Test
{
public:
	std::string input;
	std::string expected;
	net::LocalTCPendpoint ep;
	pecho::Connection* connection;
	enum
	{
		EoDBufferSize=4,
		EscBufferSize=1
	};

protected:
	pechoHandlerFixture() :ep( "127.0.0.1", 12345),connection(0) {}

	virtual void SetUp()
	{
		TestDescription test;
		connection = new pecho::Connection( ep, test.inputBufferSize+EoDBufferSize, test.outputBufferSize+EscBufferSize);

		input.clear();
		expected.clear();
		input.append( "echo\r\n");
		expected.append( "OK expecting command\r\n");
		expected.append( "OK enter data\r\n\r\n");

		input.append( test.content);
		expected.append( escape( test.content));

		input.append( "\r\n.\r\n");
		expected.append( "OK expecting command\r\n");
		input.append( "quit\r\n");
		expected.append( "BYE\r\n");
	}

	virtual void TearDown()
	{
		delete connection;
	}
};

typedef ::testing::Types<
	OneEmptyLine<3,3>,
	OneEmptyLine<2,2>,
	Empty<1,1>,
	OneEmptyLine<1,1>,
	OneOneCharLine<1,1>,
	OneLine<1,1>,
	Random<1,1,2000>,
	Empty<2,2>,
	OneOneCharLine<2,2>,
	OneLine<2,2>,
	Random<2,2,2000>,
	Empty<3,3>,
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
	Empty<3,4>,
	OneEmptyLine<3,4>,
	OneOneCharLine<3,4>,
	OneLine<3,4>,
	Random<3,4,24000>,
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
	OneOneCharLine<4,3>,
	OneLine<4,3>,
	Random<4,3,24000>
	> MyTypes;
TYPED_TEST_CASE( pechoHandlerFixture, MyTypes);

TYPED_TEST( pechoHandlerFixture, ExpectedResult )
{
	std::string output;
	EXPECT_EQ( 0, test::runTestIO( this->input, output, *this->connection));

	unsigned int ii=0,nn=output.size();
	for (;ii<nn && output[ii]==this->expected[ii]; ii++);
	if (ii != nn)
	{
		printf( "SIZE R=%lu,E=%lu,DIFF AT %u='%d %d %d %d|%d %d %d %d'\n",
			(unsigned long)output.size(), (unsigned long)this->expected.size(), ii, output[ii-2],output[ii-1],output[ii-0],output[ii+1],this->expected[ii-2],this->expected[ii-1],this->expected[ii-0],this->expected[ii+1]);
		boost::this_thread::sleep( boost::posix_time::seconds( 5 ));
	}
#ifndef _WIN32
	EXPECT_EQ( this->expected, output);
#else
	EXPECT_EQ( expected, output);
#endif
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}

