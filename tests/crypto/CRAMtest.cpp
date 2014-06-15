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
// CRAM tests
//
#include "gtest/gtest.h"
#include "wtest/testReport.hpp"
#include <stdexcept>
#include "AAAA/CRAM.hpp"
#include "system/globalRngGen.hpp"

using namespace _Wolframe::AAAA;

TEST( CRAM, Challenge )
{
	_Wolframe::GlobalRandomGenerator& rnd = _Wolframe::GlobalRandomGenerator::instance( "" );

	CRAMchallenge challenge( rnd );
	std::cout << "Challenge: " << challenge.toBCD() << std::endl;
	CRAMresponse resp1( challenge.toString( PasswordHash::Salt( "GoodSalt" )), "password" );
	CRAMresponse resp2( challenge, PasswordHash( "GoodSalt", "password" ));
	EXPECT_TRUE( resp1 == resp2 );
	std::cout << "Response : " << resp1.toBCD() << std::endl;
}

int main( int argc, char **argv )
{
	WOLFRAME_GTEST_REPORT( argv[0], refpath.string());
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}

