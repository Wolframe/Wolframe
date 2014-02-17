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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file testStorageDataType.cpp
///\brief Test for storageDataType.hpp
#include "types/storageDataType.hpp"
#include "gtest/gtest.h"
#include <iostream>
#include <string>
#include <limits>
#include <cstdlib>
#include <boost/cstdint.hpp>

using namespace _Wolframe;

class StorageDataTypeDescriptionTest
	:public ::testing::Test
{
protected:
	StorageDataTypeDescriptionTest()
	{
		srand(123);
	}
	virtual ~StorageDataTypeDescriptionTest() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};

template <typename T>
struct NumericTestData
{
	T val1;
	T val2;
	T minimum;
	T maximum;

	NumericTestData() :val1((T)rand()),val2((T)rand()),minimum(std::numeric_limits<T>::min()),maximum(std::numeric_limits<T>::max()){}
};

template <typename T>
static void testReadWrite( const T& data, const char* typeName)
{
	std::cerr << "Testing type '" << typeName << "'" << std::endl;
	types::StorageDataType value;
	value.init( data);
}

template <typename T>
static void testNumericReadWrite( const NumericTestData<T>& testdata, const char* typeName)
{
	std::cerr << "Testing type '" << typeName << "'" << std::endl;
	types::StorageDataType value;
	value.init( testdata.val1);
	value.init( testdata.val2);
	value.init( testdata.minimum);
	value.init( testdata.maximum);
}

TEST_F( StorageDataTypeDescriptionTest, tests)
{
	std::wstring wstring_( L"This is a string!");
	std::string string_( "This is a string!");
	float float_ = 3.1415927;
	double double_ = 3.1415926535897931;
	NumericTestData<boost::int64_t> testInt64;
	NumericTestData<boost::uint64_t> testUInt64;
	NumericTestData<boost::int32_t> testInt32;
	NumericTestData<boost::uint32_t> testUInt32;
	NumericTestData<boost::int16_t> testInt16;
	NumericTestData<boost::uint16_t> testUInt16;
	NumericTestData<char> testChar;
	NumericTestData<unsigned char> testUChar;
	types::StorageDataType::DateStruct date_(1968,11,18);
	types::StorageDataType::DateTimeStruct datetime_(1968,11,18,04,11,56);
	types::StorageDataType::DateTimeStruct_mcs datetime_mcs_(1968,11,18,04,11,56,867,615);

	testReadWrite( wstring_, "std::wstring");
	testReadWrite( string_, "std::string");
	testReadWrite( float_, "float");
	testReadWrite( double_, "double");
	testNumericReadWrite( testInt64, "int64_t");
	testNumericReadWrite( testUInt64, "uint64_t");
	testNumericReadWrite( testInt32, "int32_t");
	testNumericReadWrite( testUInt32, "uint32_t");
	testNumericReadWrite( testInt16, "int16_t");
	testNumericReadWrite( testUInt16, "uint16_t");
	testNumericReadWrite( testChar, "char");
	testNumericReadWrite( testUChar, "unsigned char");
	testReadWrite( date_, "types::StorageDataType::DateStruct");
	testReadWrite( datetime_, "types::StorageDataType::DateTimeStruct");
	testReadWrite( datetime_mcs_, "types::StorageDataType::DateTimeStruct_mcs");
}

int main( int argc, char **argv)
{
	::testing::InitGoogleTest( &argc, argv);
	return RUN_ALL_TESTS();
}


