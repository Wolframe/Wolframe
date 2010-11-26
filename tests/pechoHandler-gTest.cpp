//
// pechoHandler class unit tests using google test framework (gTest)
//


#include "pechoHandler.hpp"
#include "testHandlerTemplates.hpp"
#include <gtest/gtest.h>


// The fixture for testing class SMERP::Version
class pechoHandlerFixture : public ::testing::Test
{
private:
   std::string input;
   std::string expected;
   Network::LocalTCPendpoint ep;
   pecho::Connection connection;

protected:
   pechoHandlerFixture(){};

	virtual void SetUp()
	{
      Network::LocalTCPendpoint ep( "127.0.0.1", 12345);
      pecho::Connection connection( ep);

      input.clear();
      expected.clear();
      input.append( "echo tolower\r\n");
      expected.append( "OK expecting command\r\n");
      expected.append( "OK enter data\r\n");

      //TODO generated data here
      const char* content =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ\r\n"
      "ABCDEFGHIJKLMNOPQRSTUVWXY\r\n"
      "ABCDEFGHIJKLMNOPQRSTUVWX\r\n"
      "ABCDEFGHIJKLMNOPQRSTUVW\r\n"
      "ABCDEFGHIJKLMNOPQRSTUV\r\n"
      "ABCDEFGHIJKLMNOPQRSTU\r\n"
      "ABCDEFGHIJKLMNOPQRST\r\n"
      "ABCDEFGHIJKLMNOPQRS\r\n"
      "ABCDEFGHIJKLMNOPQR\r\n"
      "ABCDEFGHIJKLMNOPQ\r\n"
      "ABCDEFGHIJKLMNOP\r\n"
      "ABCDEFGHIJKLMNO\r\n"
      "ABCDEFGHIJKLMN\r\n"
      "ABCDEFGHIJKLM\r\n"
      "ABCDEFGHIJKL\r\n"
      "ABCDEFGHIJK\r\n"
      "ABCDEFGHIJ\r\n"
      "ABCDEFGHI\r\n"
      "ABCDEFGH\r\n"
      "ABCDEFG\r\n"
      "ABCDEF\r\n"
      "ABCDE\r\n"
      "ABCD\r\n"
      "ABC\r\n"
      "AB\r\n"
      "A\r\n";

      input.append( content);
      expected.append( content);
      input.append( ".\r\n");
      expected.append( "OK expecting command\r\n");
      input.append( "quit\r\n");
      expected.append( "BYE\r\n");
	};
};


TEST_F( pechoHandlerFixture, ExpectedResult )
{
   std::string output;
   EXPECT_EQ( 0, test::runTestIO( input.c_str(), output, connection));
   EXPECT_EQ( output, expected);
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}

