<?php

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

function userHash( $username )
{
	$seed = str_repeat( 0, 16 );
	for ( $i = 0; $i < 16; $i++ )
		$seed[ $i ] = rand( 0, 255 );
	$hash = hash_hmac( "sha256", $username, $seed, 'true' );

	return '$' . base64_encode( $seed ) . '$' . base64_encode( $hash );
}


function CRAMresponse( $password, $challenge )
{
	if ( $challenge[ 0 ] != '$' )
		return 'invalid challenge format';
	$challenge = ltrim( $challenge, '$' );
	$chlngPart = explode( '$', $challenge );
	if ( count( $chlngPart ) != 2 )
		return 'invalid challenge format';
	$salt = base64_decode( $chlngPart[ 0 ] );
	$chlng = base64_decode( $chlngPart[ 1 ] );
	if ( strlen( $chlng ) != 64 )
		return 'invalid challenge length';
	$passwd = hash_pbkdf2( "sha1", $password, $salt, 10589, 48, 'true' );
	if ( strlen( $passwd ) > 64 )	{
		$response = hash( "sha512", $passwd );
	}
	else	{
		$response = str_repeat( 0x3c, 64 );
		for ( $i = 0; $i < count( $passwd ); $i++ )
			$response[ $i ] = $passwd[ $i ];
	}
	for ( $i = 0; $i < 64; $i++ )
		$response[ $i ] = $response[ $i ] ^ $challenge[ $i ];
	return base64_encode( hash( "sha256", $response ));
}

?>

