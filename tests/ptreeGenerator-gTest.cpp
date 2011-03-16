//
// pechoHandler class unit tests using google test framework (gTest)
//


#include "generators/propertyTree.hpp"
#include <boost/property_tree/xml_parser.hpp>
#include <gtest/gtest.h>
#include <stdlib.h>

using namespace _Wolframe;
using namespace mtproc;

struct TestDescription
{
	const char* in;
	const char* out;
};
static const TestDescription testDescription[] =
{
	{"<x>a</x>","0 x\n2 a\n3 \n"},
	{0,0}
};


class PropertyTreeGeneratorFixture : public ::testing::Test
{
public:
	std::vector<std::string> output;
	std::vector<std::string> expected;

protected:
	PropertyTreeGeneratorFixture() {}

	virtual void SetUp()
	{
		for (unsigned int tt=0; testDescription[tt].in; tt++)
		{
			std::stringstream in( testDescription[ tt].in, std::ios_base::in);
			std::stringstream out( std::ios_base::out);
			boost::property_tree::ptree pt;
			boost::property_tree::xml_parser::read_xml( in, pt);
			PropertyTreeGenerator gg( &pt);
			protocol::Generator::ElementType type;
			char buf[ 256];
			protocol::Generator::size_type nn = 0;
			while (gg.getNext( &type, buf, sizeof(buf), &nn))
			{
				buf[nn] = 0;
				out << (int)type << " " << buf << "\n";
				nn = 0;
			}
			expected.push_back( testDescription[ tt].out);
			output.push_back( out.str());
		}
	};
	virtual void TearDown()
	{};
};


// Tests the Version constructors and members
TEST_F( PropertyTreeGeneratorFixture, Members )
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

