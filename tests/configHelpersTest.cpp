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
// unit tests for configHelpers
//

#include "configHelpers.hpp"
#include <gtest/gtest.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

using namespace _Wolframe::Configuration;

// The fixture for tests
class ConfigFixture : public ::testing::Test	{
protected:
	// Set-up work for each test here.
	ConfigFixture()	{
		read_info( "configHelpersTest.info", pt );
	}

//	// Clean-up work that doesn't throw exceptions here.
//	virtual ~VersionFixture()	{
//	}


	//   Code here will be called immediately after the constructor (right
	//   before each test).
	virtual void SetUp() {
	}

	//   Code here will be called immediately after each test (right
	//   before the destructor).
	virtual void TearDown() {
	}

	boost::property_tree::ptree	pt;
};


TEST_F( ConfigFixture, bool )	{
	bool	ret, val;

	boost::property_tree::ptree::const_iterator it = pt.get_child( "bool-true" ).begin();
	for ( ; it != pt.get_child( "bool-true" ).end(); it++ )	{
		ret = getBoolValue( it, "Config Helpers bool test - true", val );
		ASSERT_TRUE( ret && val );
	}

	it = pt.get_child( "bool-false" ).begin();
	for ( ; it != pt.get_child( "bool-false" ).end(); it++ )	{
		ret = getBoolValue( it, "Config Helpers bool test - false", val );
		ASSERT_TRUE( ret && !val );
	}

	val = false;
	it = pt.get_child( "bool-wrong" ).begin();
	for ( ; it != pt.get_child( "bool-wrong" ).end(); it++ )	{
		ret = getBoolValue( it, "Config Helpers bool test - wrong", val );
		ASSERT_TRUE( !ret && !val );
	}

	it = pt.get_child( "bool-true" ).begin();
	for ( ; it != pt.get_child( "bool-true" ).end(); it++ )	{
		bool isSet = false;
		val = false;
		ret = getBoolValue( it, "Config Helpers bool test, isSet flag - set", val, isSet );
		ASSERT_TRUE( ret && val && isSet );
	}

	val = true;
	it = pt.get_child( "bool-true" ).begin();
	for ( ; it != pt.get_child( "bool-true" ).end(); it++ )	{
		bool isSet = true;
		ret = getBoolValue( it, "Config Helpers bool test, isSet flag - set", val, isSet );
		ASSERT_TRUE( !ret && val && isSet );
	}
}


TEST_F( ConfigFixture, unsigned_short )	{
	bool		ret;
	unsigned short	val;

	boost::property_tree::ptree::const_iterator it = pt.get_child( "ushortNZ-valid" ).begin();
	for ( ; it != pt.get_child( "ushortNZ-valid" ).end(); it++ )	{
		val = 0;
		ret = getNonZeroIntValue<unsigned short>( it, "Config Helpers unsigned short Test - valid", val );
		ASSERT_TRUE( ret && ( val > 0 ));
	}

	it = pt.get_child( "ushortNZ-invalid" ).begin();
	for ( ; it != pt.get_child( "ushortNZ-invalid" ).end(); it++ )	{
		val = 0;
		ret = getNonZeroIntValue<unsigned short>( it, "Config Helpers unsigned short Test - invalid", val );
		ASSERT_TRUE( !ret && ( val == 0 ));
	}
}


int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
