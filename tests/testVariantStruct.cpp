#include "types/variant.hpp"
#include "types/variantStruct.hpp"
#include "types/variantStructDescription.hpp"
#include "gtest/gtest.h"
#include "wtest/testReport.hpp"
#include <iostream>

using namespace _Wolframe;

class VariantStructDescriptionStableTest : public ::testing::Test
{
protected:
	VariantStructDescriptionStableTest() {}
	virtual ~VariantStructDescriptionStableTest() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};

class VariantStructDescriptionTest : public ::testing::Test
{
protected:
	VariantStructDescriptionTest() {}
	virtual ~VariantStructDescriptionTest() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};

TEST_F( VariantStructDescriptionStableTest, tests)
{
	types::VariantStructDescription main;
	types::VariantStructDescription outer;
	types::VariantStructDescription inner;
	inner.addAttribute( "hi", types::Variant( 12), 0);
	inner.addAttribute( "ho", types::Variant( 1.0), 0);
	inner.addAtom( "hu", types::Variant( "hihohu"), 0);
	outer.addAtom( "bu", types::Variant( "buhu"), 0);
	outer.addStructure( "chld", inner);
	main.addStructure( "data", outer);
}

int main( int argc, char **argv)
{
	WOLFRAME_GTEST_REPORT( argv[0], refpath.string());
	::testing::InitGoogleTest( &argc, argv);
	return RUN_ALL_TESTS();
}


