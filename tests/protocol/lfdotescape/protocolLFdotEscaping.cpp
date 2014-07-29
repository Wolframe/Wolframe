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

#include "ioblocks.hpp"
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

static std::string beautified( const std::string& str)
{
	std::ostringstream rt;
	std::string::const_iterator si = str.begin(), se = str.end();
	for (; si != se; ++si)
	{
		if (*si == '\r')
		{
			rt << 'r';
		}
		else if (*si == '\n')
		{
			rt << 'n';
		}
		else
		{
			rt << *si;
		}
	}
	return rt.str();
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

		protocol::EscapeSTM estm;
		std::string input = randmsg( seed+testno);

		std::cout << "INPUT  {" << beautified(input) << "}" << std::endl;
		char* ib = (char*)std::malloc( ibsize);
		protocol::OutputBlock oib( ib, ibsize);
		char* ob = (char*)std::malloc( obsize);
		std::string mid,output;
		std::size_t obpos;

		estm.setNextBlock( &oib, input.c_str(), input.size());
		while (estm.hasMore())
		{
			estm.process();
			mid.append( oib.charptr(), oib.pos());
			oib.setPos(0);
		}
		std::cout << "INESC  {" << beautified(mid) << "}" << std::endl;
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
		std::cout << "OUTPUT {" << beautified(output) << "}" << std::endl;
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

