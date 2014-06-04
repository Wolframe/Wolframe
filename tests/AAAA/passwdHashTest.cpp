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
//
// paswword hash manipulation tests
//

#include "logger-v1.hpp"
#include "gtest/gtest.h"
#include "system/globalRngGen.hpp"
#include "AAAA/passwordHash.hpp"
#include "crypto/HMAC.hpp"

using namespace _Wolframe::AAAA;
using namespace _Wolframe::crypto;
using namespace std;

// The fixture for testing class _Wolframe::module
class PasswdHashFixture : public ::testing::Test
{
protected:
	PasswdHashFixture( )
	{
		_Wolframe::GlobalRandomGenerator::instance( "" );
	}
};


// Note that although the parametrs have the same value, they are not
// computed here, they are computed outside in order to catch more errors
static void testSalt( const unsigned char saltData[], const char* base64,
		      const char* BCD )
{
	PasswordHash::Salt	salt0( saltData, PASSWORD_SALT_SIZE );
	PasswordHash::Salt	salt1( base64 );

	EXPECT_EQ( salt0.size(), PASSWORD_SALT_SIZE );
	EXPECT_EQ( salt1.size(), PASSWORD_SALT_SIZE );

	EXPECT_STRCASEEQ( base64, salt0.toString().c_str() );
	EXPECT_STRCASEEQ( BCD, salt0.toBCD().c_str() );
	EXPECT_TRUE( salt0 == salt1 );
}

TEST_F( PasswdHashFixture, PasswordSaltConstructors )
{
	unsigned char		saltData[ PASSWORD_SALT_SIZE ];
	PasswordHash::Salt	salt0;

	// default constructor, should initialize salt to empty
	EXPECT_EQ( salt0.size(), 0lu );
	EXPECT_STREQ( "", salt0.toBCD().c_str() );
	EXPECT_STREQ( "", salt0.toString().c_str() );

	for ( size_t i = 0; i < PASSWORD_SALT_SIZE; i++ )
		saltData[ i ] = 0;
	testSalt( saltData, "AAAAAAAAAAAAAAAAAAAAAA",
		  "00000000000000000000000000000000" );

	saltData[ 0 ] = 0x80, saltData[ PASSWORD_SALT_SIZE - 1 ] = 0x01;
	testSalt( saltData, "gAAAAAAAAAAAAAAAAAAAAQ",
		  "80000000000000000000000000000001" );

	for ( size_t i = 0; i < PASSWORD_SALT_SIZE; i++ )
		saltData[ i ] = i;
	testSalt( saltData, "AAECAwQFBgcICQoLDA0ODw",
		  "000102030405060708090a0b0c0d0e0f" );

	for ( size_t i = 0; i < PASSWORD_SALT_SIZE; i++ )
		saltData[ i ] = 0xff;
	testSalt( saltData, "/////////////////////w",
		  "ffffffffffffffffffffffffffffffff" );

	saltData[ 0 ] = 0x7f, saltData[ PASSWORD_SALT_SIZE - 1 ] = 0xfe;
	testSalt( saltData, "f////////////////////g",
		  "7ffffffffffffffffffffffffffffffe" );

	EXPECT_THROW( PasswordHash::Salt salt1( "AAAAAAAAAAAAAAAAAAAAAAA" ), std::runtime_error );

	PasswordHash::Salt	salt2( "AAAAAAAAAAAAAAAAAA" );
	EXPECT_TRUE( salt2.size() < PASSWORD_SALT_SIZE );
}


// Note that although the parametrs have the same value, they are not
// computed here, they are computed outside in order to catch more errors
static void testHash( const unsigned char hashData[], const char* base64,
		      const char* BCD )
{
	PasswordHash::Hash	hash0( hashData, PASSWORD_HASH_SIZE );
	PasswordHash::Hash	hash1( base64 );

	EXPECT_EQ( hash0.size(), PASSWORD_HASH_SIZE );
	EXPECT_EQ( hash1.size(), PASSWORD_HASH_SIZE );

	EXPECT_STRCASEEQ( base64, hash0.toString().c_str() );
	EXPECT_STRCASEEQ( BCD, hash0.toBCD().c_str() );
	EXPECT_TRUE( hash0 == hash1 );
}

TEST_F( PasswdHashFixture, PasswordHashDataConstructors )
{
	unsigned char		hashData[ PASSWORD_HASH_SIZE ];
	PasswordHash::Salt	salt0;

	// default constructor, should initialize salt to empty
	EXPECT_EQ( salt0.size(), 0lu );
	EXPECT_STREQ( "", salt0.toBCD().c_str() );
	EXPECT_STREQ( "", salt0.toString().c_str() );

	for ( size_t i = 0; i < PASSWORD_HASH_SIZE; i++ )
		hashData[ i ] = 0;
	testHash( hashData, "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
		  "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" );

	hashData[ 0 ] = 0x80, hashData[ PASSWORD_HASH_SIZE - 1 ] = 0x01;
	testHash( hashData, "gAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB",
		  "800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001" );

	for ( size_t i = 0; i < PASSWORD_HASH_SIZE; i++ )
		hashData[ i ] = i;
	testHash( hashData, "AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8gISIjJCUmJygpKissLS4v",
		  "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f" );

	for ( size_t i = 0; i < PASSWORD_HASH_SIZE; i++ )
		hashData[ i ] = 0xff;
	testHash( hashData, "////////////////////////////////////////////////////////////////",
		  "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff" );

	hashData[ 0 ] = 0x7f, hashData[ PASSWORD_HASH_SIZE - 1 ] = 0xfe;
	testHash( hashData, "f//////////////////////////////////////////////////////////////+",
		  "7ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffe" );

	EXPECT_THROW( PasswordHash::Hash hash1( "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" ),
		      std::runtime_error );
	EXPECT_THROW( PasswordHash::Hash hash2( "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" ),
		      std::runtime_error );
}

TEST_F( PasswdHashFixture, PasswordHashTest )
{
	PasswordHash	hash0( "$gAAAAAAAAAAAAAAAAAAAAQ$gAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB");

	EXPECT_STREQ( "80000000000000000000000000000001", hash0.salt().toBCD().c_str() );
	EXPECT_STREQ( "800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001"
		      , hash0.hash().toBCD().c_str() );
	std::cout << "Password hash: " << hash0.toString() << std::endl;
}

//****************************************************************************

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
