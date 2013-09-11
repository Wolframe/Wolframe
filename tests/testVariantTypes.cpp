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
// Tests portability of variant_types and boost lexical casts
//

#include "gtest/gtest.h"
#include "types/variant.hpp"
#include <string>
#include <sstream>
#include <boost/cstdint.hpp>

using namespace _Wolframe;
using namespace types;
using namespace std;

TEST( variantTypeFixture, prerequisites )
{
#ifdef BOOST_NO_INT64_T
	// if this fails either we really have no 64-bit support or
	// boost cstdint has not been correctly ported to that platform!
	FAIL( ) << "BOOST_NO_INT64_T is not defined, variant type 64-bit support is broken!";
#else
	SUCCEED( );
#endif
}

TEST( variantTypeFixture, casts )
{
	string s = "47";
	Variant v( s );
	v.convert( Variant::string_ );
	ASSERT_EQ( "47", v.tostring( ) );
	v.convert( Variant::uint_ );
	ASSERT_EQ( 47, v.touint( ) );
}

TEST( variantTypeFixture, uint32 )
{
	unsigned int i = 47;
	Variant v( i );
	ASSERT_EQ( i, v.toint( ) );
	v.convert( Variant::string_ );
	ASSERT_EQ( "47", v.tostring( ) );
}

TEST( variantTypeFixture, uint64 )
{
	boost::uint64_t i = UINT64_C( 18446744073709551615 ); // 2^64-1
	Variant v( i );
	ASSERT_EQ( i, v.touint( ) );
	v.convert( Variant::string_ );
	ASSERT_EQ( "18446744073709551615", v.tostring( ) );	
}

TEST( variantTypeFixture, initialized )
{
	Variant v;

	ASSERT_FALSE( v.initialized( ) );
}

TEST( variantTypeFixture, output )
{
	Variant v1( 47 );
	Variant v2( 0.0 );
	Variant v3( "hello" );

	ostringstream os;
	os << v1 << " " << v2 << " " << v3;

	ASSERT_EQ( os.str( ), "47 0 hello" );
}

TEST( variantTypeFixture, const_ )
{
	Variant v1( 47 );
	VariantConst v2( v1 );

	ASSERT_EQ( v1, v2 );
	ASSERT_EQ( v1.toint( ), 47 );

	ASSERT_FALSE( v1.constant( ) );
	ASSERT_TRUE( v2.constant( ) );

	v2 = "hello";
	// why?
	ASSERT_EQ( v2.tostring( ), "hello" );
	v1 = "hello";

	ASSERT_EQ( v1.tostring( ), "hello" );
	ASSERT_EQ( v2.tostring( ), "hello" );
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}