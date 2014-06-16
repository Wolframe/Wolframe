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
#include <fstream>
#include <boost/algorithm/string.hpp>

#include "gtest/gtest.h"
#include "wtest/testReport.hpp"
#include "crypto/AES256.h"

/*
static bool readValues( std::ifstream infile, const char* label1, std::string& value1,
					      const char* label2, std::string& value2,
					      const char* label3, std::string& value3,
					      const char* label4, std::string& value4 )
{
	bool	hasVal1, hasVal2, hasVal3, hasVal4;
	hasVal1 = hasVal2 = hasVal3 = hasVal4 = false;

	std::string line;
	while ( std::getline( infile, line ))	{
		std::string cleanLine = line.substr( 0, line.find( "#" ));
		boost::trim( cleanLine );
		if ( cleanLine.empty() )
			continue;
		if ( boost::starts_with( cleanLine, label1 ))	{
			if ( cleanLine[ strlen( label1 )] == '=' )	{
				if ( hasVal1 )	{
					std::string msg;
					msg = "DATA ERROR: '" + label1 + "' has already been defined";
					throw( msg );
				}
				value1 = cleanLine.substr( strlen( label1 ));
				hasVal1 = true;
			}
		}
	}
}
*/

TEST( DISABLED_AES256, TestVectors )
{
}

int main( int argc, char **argv )
{
	WOLFRAME_GTEST_REPORT( argv[0], refpath.string());
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}


