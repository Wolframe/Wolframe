<?php
namespace Wolframe;
if (version_compare(PHP_VERSION, '5.5.0', '<')) {
	require 'pbkdf2.php';
}

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

class WolframeCram
{
	public static function userHash( $username )
	{
		$seed = str_repeat( 0, 16 );
		for ( $i = 0; $i < 16; $i++ )
			$seed[ $i ] = rand( 0, 255 );
		$hash = hash_hmac( "sha256", $username, $seed, true );
	
		return '$' . base64_encode( $seed ) . '$' . base64_encode( $hash );
	}
	
	public static function seededUserHash( $seed, $username )
	{
		$seed = base64_decode( $seed );
	
		$hash = hash_hmac( "sha256", $username, $seed, true );
	
		return '$' . base64_encode( $seed ) . '$' . base64_encode( $hash );
	}
	
	public static function CRAMresponse( $password, $challenge )
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
		if ( count( $passwd ) > 64 )	{
			$response = hash( "sha512", $passwd, true );
		}
		else	{
			$respArray = array_fill( 0, 64, 0x3c );
			$response = implode( array_map( "chr", $respArray ));
			for ( $i = 0; $i < strlen( $passwd ); $i++ )
				$response[ $i ] = $passwd[ $i ];
		}
		for ( $i = 0; $i < 64; $i++ )
			$response[ $i ] = $response[ $i ] ^ $chlng[ $i ];
	
		return base64_encode( hash( "sha256", $response, true ));
	}

	function passwordChangeMessage( $oldPassword, $challenge, $newPassword )
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
	
		$passwd = hash_pbkdf2( "sha1", $oldPassword, $salt, 10589, 48, 'true' );
		if ( count( $passwd ) > 64 )	{
			$response = hash( "sha512", $passwd, true );
		}
		else	{
			$respArray = array_fill( 0, 64, 0x3c );
			$response = implode( array_map( "chr", $respArray ));
			for ( $i = 0; $i < strlen( $passwd ); $i++ )
				$response[ $i ] = $passwd[ $i ];
		}
		for ( $i = 0; $i < 64; $i++ )
			$response[ $i ] = $response[ $i ] ^ $chlng[ $i ];
		$key = hash( "sha256", $response, true );
	
		$pwdArray = array_fill( 0, 48, 0x00 );
		$pwdMessage = implode( array_map( "chr", $pwdArray ));
		$pwdLen = strlen( $newPassword );
		if ( $pwdLen > 47 )
			$pwdLen = 47;
		for ( $i = 0; $i < $pwdLen; $i++ )
			$pwdMessage[ $i + 1 ] = $newPassword[ $i ];
		$pwdMessage[ 0 ] = pack( "C", $pwdLen );
		$pwdHash = hash( "md5", $pwdMessage, true );
		return base64_encode( mcrypt_encrypt( MCRYPT_RIJNDAEL_128, $key,
					 ( $pwdMessage . $pwdHash ), MCRYPT_MODE_CBC, $salt ));
	}
} // class WolframeCram
?>

