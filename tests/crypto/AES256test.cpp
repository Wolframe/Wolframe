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
// AES-256 ECB tests
//

#include <string>
#include <map>
#include <fstream>
#include <boost/algorithm/string.hpp>

#include "gtest/gtest.h"
#include "wtest/testReport.hpp"

#include "types/byte2hex.h"
#include "crypto/AES256.h"


static const std::size_t AES256_KEY_SIZE = 256 / 8;
static const std::size_t AES256_TEXT_SIZE = 128 / 8;
static const std::size_t AES256_CIPHER_SIZE = 128 / 8;

static bool readValues( std::ifstream& infile, std::map< std::string, std::string >& values )
{
	bool	hasValues = false;
	bool	hasLine;
	std::string line;
	std::size_t pos;

	while (( hasLine = std::getline( infile, line )))	{
		if (( pos = line.find( "#" )) != std::string::npos )
			line.erase( pos );
		boost::trim( line );
		if ( ! line.empty() )
			break;
	}

	while ( hasLine )	{
		hasValues = true;
		if (( pos = line.find( "#" )) != std::string::npos )
			line.erase( pos );
		boost::trim( line );
		if ( line.empty() )
			return true;

		pos = line.find( "=" );
		if ( pos != std::string::npos )
			values.insert( std::pair< std::string, std::string >( boost::trim_copy( line.substr( 0, pos )),
									      boost::trim_copy( line.substr( pos + 1 )) ));
		else	{
			std::string msg = "Invalid (key, value) string: '" + line + "'";
			throw( std::runtime_error( msg ));
		}
		hasLine = std::getline( infile, line );
	}
	return hasValues;
}


//AES-256.vectors/ECBdecode256.rsp
//AES-256.vectors/ECBencode256.rsp
//AES-256.vectors/ECBGFSbox256.rsp
//AES-256.vectors/ECBKeySbox256.rsp
//AES-256.vectors/ECBtable256.rsp
//AES-256.vectors/ECBvarKey256.rsp
//AES-256.vectors/ECBVarKey256.rsp
//AES-256.vectors/ECBvarTxt256.rsp
//AES-256.vectors/ECBVarTxt256.rsp

TEST( AES256, TestVectors )
{
	std::ifstream infile("AES-256.vectors/ECBGFSbox256.rsp");

	bool hasValues = true;
	do	{
		std::map< std::string, std::string > values;
		hasValues = readValues( infile, values );
//		for( std::map< std::string, std::string >::const_iterator it = values.begin();
//									it != values.end(); it++ )	{
//			std::cout << "Values: ( " << it->first << ", " << it->second << " )" << std::endl;
//		}
		if ( hasValues )	{
			std::cout << "Count    : " << values[ "COUNT" ] << std::endl;
			std::cout << "Key      : " << values[ "KEY" ] << std::endl;
			std::cout << "Plain    : " << values[ "PLAINTEXT" ] << std::endl;
			std::cout << "Encrypted: " << values[ "CIPHERTEXT" ] << std::endl;
			std::cout << "\n";

//			unsigned int count = boost::lexical_cast< unsigned int >( values[ "COUNT" ] );
			unsigned char key[ AES256_KEY_SIZE ];
			hex2byte( values[ "KEY" ].c_str(), key, AES256_KEY_SIZE );
			unsigned char text[ AES256_TEXT_SIZE ];
			hex2byte( values[ "PLAINTEXT" ].c_str(), text, AES256_TEXT_SIZE );

			AES256_context	ctx;
			AES256_init( &ctx, key );
			AES256_encrypt_ECB( &ctx, text );
			AES256_done( &ctx );

			char output[65];
			byte2hex( ctx.enckey, 32, output, 65 );
			std::cout << "Enckey: " << output << std::endl;

			byte2hex( ctx.deckey, 32, output, 65 );
			std::cout << "Deckey: " << output << std::endl;
		}
	} while ( hasValues );
}

int main( int argc, char **argv )
{
	WOLFRAME_GTEST_REPORT( argv[0], refpath.string());
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
