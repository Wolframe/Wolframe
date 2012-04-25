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
// byte <-> ASCII hex conversion tests

#include "gtest/gtest.h"
#include "byte2hex.h"

TEST( Bytes_2_Hex, SHA224 )
{
	unsigned char	bytes[16];
	char		output[33];
	int		i;

	for ( i = 0; i < 16; i++ )
		bytes[i] = 0;
	ASSERT_STREQ( byte2hex( bytes, 11, output, 33 ), "0000000000000000000000" );

	for ( i = 0; i < 16; i++ )
		bytes[i] = 0xff;
	ASSERT_STREQ( byte2hex( bytes, 8, output, 33 ), "FFFFFFFFFFFFFFFF" );

	ASSERT_TRUE( byte2hex( bytes, 78, output, 33 ) == NULL );

	ASSERT_TRUE( byte2hex( bytes, 0, output, 0 ) == NULL );
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}

