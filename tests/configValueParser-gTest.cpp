///
/// \brief configuration lexem parser unit tests using google test framework (gTest)
///

#include "config/valueParser.hpp"
#include <gtest/gtest.h>
#include <stdlib.h>

using namespace _Wolframe;
using namespace config;

enum TestType {undefined_,bool_,short_,int_,int_1_10_,enum_};

struct TestDescription
{
	TestType type;
	const char* in;
	const char* out;
};
static const TestDescription testDescription[] =
{
	{bool_,"true","1"},
	{bool_,"false","0"},
	{int_,"-123","-123"},
	{short_,"323333","!error"},
	{int_1_10_,"1","1"},
	{int_1_10_,"2","2"},
	{int_1_10_,"10","10"},
	{int_1_10_,"11","!error"},
	{int_1_10_,"0","!error"},
	{undefined_,0,0}
};


class ProtocolArgumentParsingFixture : public ::testing::Test
{
public:
		std::vector<std::string> output;
		std::vector<std::string> expected;

protected:
		ProtocolArgumentParsingFixture() {}

		virtual void SetUp()
		{
			for (unsigned int tt=0; testDescription[tt].in; tt++)
			{
				bool success = false;
				const std::string in(testDescription[tt].in);
				std::string out;
				std::string name;
				name.append( "cfg");
				name.append( boost::lexical_cast<std::string>(tt));

				switch (testDescription[tt].type)
				{
					case undefined_:
					break;
					case bool_:
					{
						static const char* booleanValues[] = {"false","true"};
						Parser::EnumDomain booleanDomain( 2, booleanValues);

						bool value = false;
						success = Parser::getValue( name.c_str(), in, value, booleanDomain);
						out = boost::lexical_cast<std::string>( value);
					}
					break;
					case int_1_10_:
					{
						Parser::RangeDomain<int> range_1_10( 1,10);

						int value = -1;
						success = Parser::getValue( name.c_str(), in, value, range_1_10);
						out = boost::lexical_cast<std::string>( value);
					}
					break;
					case int_:
					{
						int value = -1;
						success = Parser::getValue( name.c_str(), in, value);
						out = boost::lexical_cast<std::string>( value);
					}
					break;
					case short_:
					{
						short value = -1;
						success = Parser::getValue( name.c_str(), in, value);
						out = boost::lexical_cast<std::string>( value);
					}
					break;
					case enum_:
					break;
				}//switch(..)

				if (!success)
				{
					out = "!error";
				}
				expected.push_back( testDescription[ tt].out);
				output.push_back( out);
			}//for (;;)
		}

		virtual void TearDown()
		{}
};


// Tests the Version constructors and members
TEST_F( ProtocolArgumentParsingFixture, Members )
{
	for (unsigned int tt=0; testDescription[tt].in; tt++)
	{
		ASSERT_EQ( output[tt], expected[tt]);
	}
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}

