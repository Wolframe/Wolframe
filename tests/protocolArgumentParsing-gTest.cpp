//
// pechoHandler class unit tests using google test framework (gTest)
//


#include "protocol.hpp"
#include <gtest/gtest.h>
#include <stdlib.h>

using namespace _SMERP;

struct TestDescription
{
   const char* in;
   const char* out;
};
static const TestDescription testDescription[20] =
{
   {"bla","bla"},
   {"bla ","bla"},
   {0,0}
};


class ProtocolFixture : public ::testing::Test
{
public:
   std::vector<std::string> output;
   std::vector<std::string> expected;

protected:
   ProtocolFixture() {}

	virtual void SetUp()
	{
      for (unsigned int tt=0; testDescription[tt].in; tt++)
      {
         std::string out;
         char* itr = const_cast<char*>( testDescription[ tt].in);
         protocol::CArgBuffer<std::string> args;
         protocol::Parser::getLine( itr, args);
         unsigned int ii=0,nn=args.size();
         while (ii<nn)
         {
            out.append( args[ii++]);
            if (ii < nn) out.append( "#");
         }
         expected.push_back( testDescription[ tt].out);
         output.push_back( out);
      }
   };
   virtual void TearDown()
   {};
};


// Tests the Version constructors and members
TEST_F( ProtocolFixture, Members )
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

