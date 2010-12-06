//
// pechoHandler class unit tests using google test framework (gTest)
//


#include "pechoHandler.hpp"
#include "connectionHandler.hpp"
#include "testHandlerTemplates.hpp"
#include <gtest/gtest.h>
#include <stdlib.h>

using namespace _SMERP;
LogBackend logBack;

const char* getRandomAsciiString()
{
   enum {MaxStringSize=2048};
   static char rt[ MaxStringSize+1];

   unsigned int ii=0,nn=rand()%MaxStringSize+1;
   while (ii<nn) rt[ii++] = 32+rand()%96;
   rt[ii] = 0;
   return rt;
}

struct Empty :public std::string {};
struct OneEmptyLine :public std::string {OneEmptyLine(){ this->append("");};};
struct OneOneCharLine :public std::string {OneOneCharLine(){ this->append("?\r\n");};};
struct FakeEOF :public std::string {FakeEOF(){ this->append("\r\n#.\r\n");};};
struct OneLine :public std::string  {OneLine(){ this->append("Hello world!\r\n");};};
struct Random :public std::string
{
   Random()
   {
      enum {MaxNofLines=128};
      unsigned int ii=0,nn=rand()%MaxNofLines+1;
      while (ii++<=nn)
      {
         this->append( getRandomAsciiString());
         this->append( "\r\n");
      }
   }
};

static void escape( std::string& content)
{
   unsigned int ii,nn;
   for (ii=0,nn=content.size(); ii<nn; ii++)
   {
      if (content[ii] == '.' && (ii==0 || content[ii-1] == '\n'))
      {
         content[ii] = '\r';
      }
   }
}


template <class Input>
class pechoHandlerFixture : public ::testing::Test
{
public:
   std::string input;
   std::string expected;
   Network::LocalTCPendpoint ep;
   pecho::Connection* connection;

protected:
   pechoHandlerFixture() :ep( "127.0.0.1", 12345),connection(0) {}

	virtual void SetUp()
	{
      connection = new pecho::Connection( ep);

      input.clear();
      expected.clear();
      input.append( "echo\r\n");
      expected.append( "OK expecting command\r\n");
      expected.append( "OK enter data\r\n\r\n");

      Input content;
      input.append( content);
      escape( content);
      expected.append( content);

      input.append( ".\r\n");
      expected.append( "\r\r\nOK expecting command\r\n");
      input.append( "quit\r\n");
      expected.append( "BYE\r\n");
	}

   virtual void TearDown()
   {
      delete connection;
   }
};

typedef ::testing::Types<Empty, OneEmptyLine, OneOneCharLine, FakeEOF, OneLine, Random> MyTypes;
TYPED_TEST_CASE( pechoHandlerFixture, MyTypes);

TYPED_TEST( pechoHandlerFixture, ExpectedResult )
{
   std::string output;
   char* itr = const_cast<char*>( this->input.c_str());
   EXPECT_EQ( 0, test::runTestIO( itr, &output, *this->connection));
   EXPECT_EQ( output.size(), this->expected.size());
   unsigned int ii,nn;
   for (ii=0,nn=output.size(); ii<nn; ii++)
   {
      if (output[ii] != this->expected[ii]) break;
   }
   EXPECT_EQ( ii, nn);
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}

