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
///\file testProtocolLFdotEscaping.cpp
///\brief Test program for testing the escaping of LF DOT sequences in both directions

#include "protocol/ioblocks.hpp"
#include "logger-v1.hpp"
#include "gtest/gtest.h"
#include "wtest/testReport.hpp"
#include "wtest/pseudoRandomGenForTests.hpp"
#include <iostream>
#include <cstdlib>

using namespace _Wolframe;

static wtest::Random g_random;
unsigned int g_random_seed = 0;
bool g_random_seed_set = false;

class LFdotEscapeTest : public ::testing::Test
{
protected:
	LFdotEscapeTest() {}
	virtual ~LFdotEscapeTest() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};

static std::string randmsg( unsigned int seed)
{
	static char chr[] = "\r\nA. ";
	enum {chrsize = 5};
	unsigned int kh = 2654435761U;
	unsigned int ii,nn;
	std::string rt;
	for (ii=0; ii<64; ii++)
	{
		nn = (ii+seed)*kh;
		rt.push_back( chr[ (nn + (nn>>4) + (nn>>8)) %chrsize]);
	}
	return rt;
}

TEST_F( LFdotEscapeTest, tests)
{
	enum {ibarsize=11,obarsize=7,EoDBufferSize=4};
	std::size_t ibar[ibarsize] = {127,2,3,5,7,11,13,17,19,23,41};
	std::size_t obar[obarsize] = {127,4,5,7,11,13,17};
	std::vector<std::string> tests;
	std::size_t testno;

	if (g_random_seed_set)
	{
		g_random.setSeed( g_random_seed);
	}
	unsigned int seed = g_random.seed();

	for (testno=0; testno < 100; ++testno)
	{
		std::size_t ibsize = ibar[ testno % ibarsize];
		std::size_t obsize = obar[ testno % obarsize];

		protocol::EscapeBuffer ebuf;
		std::string input = randmsg( seed+testno);

		char* ib = (char*)std::malloc( ibsize);
		char* ob = (char*)std::malloc( obsize);
		std::string mid,output;
		std::size_t ibpos,obpos;
		for (ibpos=0; ibpos<input.size(); ibpos+=ibsize)
		{
			std::size_t nn = (ibpos+ibsize>input.size())?(input.size()-ibpos):ibsize;
			std::memcpy( ib, input.c_str()+ibpos, nn);
			ebuf.process( ib, ibsize, nn);
			mid.append( ib, nn);
			if (ebuf.hasData())
			{
				nn = 0;
				ebuf.process( ib, ibsize, nn);
				mid.append( ib, nn);
			}
		}
		protocol::InputBlock oblk( ob, obsize, 0);
		void* bb;
		std::size_t bbsize;
		for (obpos=0; obpos<mid.size(); obpos += bbsize)
		{
			if (!oblk.getNetworkMessageRead( bb, bbsize)) throw std::runtime_error( "buffer too small to hold EoD");
			if (obpos+bbsize>mid.size()) bbsize = mid.size()-obpos;
			std::memcpy( bb, mid.c_str()+obpos, bbsize);
			oblk.incr( bbsize);
			oblk.getEoD( oblk.begin());
			if (oblk.gotEoD()) throw std::runtime_error( "got unexpected EoD");
			output.append( oblk.charptr(), oblk.pos());
			oblk.setPos( 0);
		}
		if (!oblk.getNetworkMessageRead( bb, bbsize)) throw std::runtime_error( "buffer too small to hold EoD");
		output.append( oblk.charptr(), oblk.pos());
		EXPECT_EQ( input, output);
		std::free( ib);
		std::free( ob);
	}
}

int main( int argc, char **argv )
{
	WOLFRAME_GTEST_REPORT( argv[0], refpath.string());
	::testing::InitGoogleTest( &argc, argv);
	return RUN_ALL_TESTS();
}

