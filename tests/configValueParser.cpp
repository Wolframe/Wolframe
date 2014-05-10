///\file configValueParser.cpp
///\brief configuration lexem parser unit tests using google test framework (gTest)

#include "config/valueParser.hpp"
#include "gtest/gtest.h"
#include "wtest/testReport.hpp"
#include <boost/property_tree/xml_parser.hpp>
#include <iostream>

using namespace _Wolframe;
using namespace config;

enum TestType {undefined_,xml_,bool_,short_,int_,int_1_10_,enum_};

struct TestDescription
{
	TestType type;
	const char* in;
	const char* out;
};

enum {NofTestEnumElements=3};
static const char* testEnumElements[ NofTestEnumElements] = {"Hello", "world", "!"};

static const TestDescription testDescription[] =
{
	{enum_,"Hello","0"},
	{enum_,"World","1"},
	{enum_,"!","2"},
	{enum_,"!!","!error"},
	{bool_,"true","1"},
	{bool_,"false","0"},
	{bool_,"ON","1"},
	{bool_,"off","0"},
	{bool_,"YES","1"},
	{bool_,"nO","0"},
	{bool_,"0","0"},
	{bool_,"1","1"},
	{int_,"-123","-123"},
	{short_,"323333","!error"},
	{xml_,"<cfg>text</cfg>","text"},
	{xml_,"<cfg><val>1</val></cfg>","!error"},
	{xml_,"<cfg><val>true</val></cfg>","!error"},
	{int_1_10_,"2","2"},
	{int_1_10_,"10","10"},
	{int_1_10_,"11","!error"},
	{int_1_10_,"0","!error"},
	{undefined_,0,0}
};

typedef std::pair<const std::string,const boost::property_tree::ptree> PtNode;

static const PtNode getPropertyTreeDef( const std::string& name, const std::string& content)
{
	std::istringstream in( content);
	boost::property_tree::ptree pt;
	read_xml( in, pt, boost::property_tree::xml_parser::no_comments);
	boost::property_tree::ptree::const_iterator it = pt.begin();
	return PtNode( name, it->second);
}

class ConfigValueParserFixture : public ::testing::Test
{
public:
		std::vector<std::string> output;
		std::vector<std::string> expected;

protected:
		ConfigValueParserFixture() {}

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
					case xml_:
					{
						std::string value;
						success = Parser::getValue( "", getPropertyTreeDef( name, in), value);
						out = value;
					}
					break;
					case bool_:
					{
						bool value = false;
						success = Parser::getValue( "", name.c_str(), in, value, Parser::BoolDomain());
						out = boost::lexical_cast<std::string>( value);
					}
					break;
					case int_1_10_:
					{
						int value = -1;
						success = Parser::getValue( "", name.c_str(), in, value, Parser::RangeDomain<int>( 1,10));
						out = boost::lexical_cast<std::string>( value);
					}
					break;
					case int_:
					{
						int value = -1;
						success = Parser::getValue( "", name.c_str(), in, value);
						out = boost::lexical_cast<std::string>( value);
					}
					break;
					case short_:
					{
						short value = -1;
						success = Parser::getValue( "", name.c_str(), in, value);
						out = boost::lexical_cast<std::string>( value);
					}
					break;
					case enum_:
					{
						Parser::EnumDomain enumDomain( NofTestEnumElements, testEnumElements);
						unsigned int value = 0;
						success = Parser::getValue( "", name.c_str(), in, value, enumDomain);
						out = boost::lexical_cast<std::string>( value);
					}
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
TEST_F( ConfigValueParserFixture, Members )
{
	for (unsigned int tt=0; testDescription[tt].in; tt++)
	{
		ASSERT_EQ( output[tt], expected[tt]);
	}
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	WOLFRAME_GTEST_REPORT( argv[0], refpath.string());
	return RUN_ALL_TESTS();
}

