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
// CRAM tests
//
#include "gtest/gtest.h"
#include <stdexcept>
#include "AAAA/CRAM.hpp"
#include "globalRngGen.hpp"

TEST( CRAM, UninitializedRandomGenerator )
{
	EXPECT_THROW( _Wolframe::AAAA::CRAMchallenge	challenge, std::logic_error );
}

TEST( CRAM, Challenge )
{
	_Wolframe::RandomGenerator::instance( "" );

	const unsigned char* pwdHash = (const unsigned char*)"1841bac2def7cf53a978f0414aa8d5c3e7c4618899709c84fedcdcd6";
	_Wolframe::AAAA::CRAMchallenge	challenge;
	std::cout << challenge.toBCD();
	_Wolframe::AAAA::CRAMresponse	resp1( challenge, pwdHash, 224 / 8 );
	_Wolframe::AAAA::CRAMresponse	resp2( challenge.toBCD(), pwdHash, 224 / 8 );
	EXPECT_TRUE( resp1 == resp2 );
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}

