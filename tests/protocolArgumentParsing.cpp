#include "protocol/parser.hpp"
#include "protocol/buffers.hpp"
#include "gtest/gtest.h"
#include "wtest/testReport.hpp"
#include <stdlib.h>

using namespace _Wolframe;

struct TestDescription
{
   const char* in;
   const char* out;
};
static const TestDescription testDescription[40] =
{
   {" bla","bla"},
   {" bla ","bla"},
   {"  bla ","bla"},
   {"bla","bla"},
   {"bla ","bla"},
   {"b a","b#a"},
   {"b  a","b#a"},
   {"b\ta","b#a"},
   {"b\t\ta","b#a"},
   {"b  acd","b#acd"},
   {"b  acd e","b#acd#e"},
   {"b  acd   e","b#acd#e"},
   {"b  acd   e  ","b#acd#e"},
   {"b'la'","bla"},
   {"b'l'","bl"},
   {"b''","b"},
   {"b 'la'","b#la"},
   {"b 'l'","b#l"},
   {"b ''","b#"},
   {"b 'la","b#la"},
   {"b 'l","b#l"},
   {"b '","b#"},
   {"b 'la''e'","b#lae"},
   {"b 'l''e'","b#le"},
   {"b '''e'","b#e"},
   {"b 'la' 'e'","b#la#e"},
   {"b 'l' 'e'","b#l#e"},
   {"b '' 'e'","b##e"},
   {"b 'l a'","b#l a"},
   {"b 'l '","b#l "},
   {"b ' '","b# "},
   {"bl\\a","bla"},
   {"bl\\'a ","bl'a"},
   {"b\\ a","b a"},
   {0,0}
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
         std::string out;
         char* itr = const_cast<char*>( testDescription[ tt].in);
         char* end = strchr( itr, '\0');
         std::string argbuf;
         protocol::CArgBuffer<std::string> args( &argbuf);
         protocol::Parser::getLine( itr, end, args);
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
      WOLFRAME_GTEST_REPORT( argv[0], refpath.string());
  return RUN_ALL_TESTS();
}

