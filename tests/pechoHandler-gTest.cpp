//
// pechoHandler class unit tests using google test framework (gTest)
//


#include "pechoHandler.hpp"
#include "connectionHandler.hpp"
#include "testHandlerTemplates.hpp"
#include <gtest/gtest.h>
#include <stdlib.h>

using namespace _SMERP;
LogBackend& logBack = _SMERP::LogBackend::instance( );

const char* getRandomAsciiString()
{
   enum {MaxStringSize=4096};
   static char rt[ MaxStringSize+1];

   unsigned int ii=0,nn=rand()%MaxStringSize+1;
   while (ii<nn && rand()%104 != 103)
   {
      rt[ii] = 32+rand()%96;

      //avoid random end of content:
      if (ii>=2 && rt[ii-2]=='\n' && rt[ii-1]=='.' && rt[ii]=='\n')
      {
         continue;
      }
      if (ii>=3 && rt[ii-3]=='\n' && rt[ii-2]=='.' && rt[ii-1]=='\r' && rt[ii]=='\n')
      {
         continue;
      }
      ii++;
   }
   //avoid random end of content:
   while (ii >= 2 && rt[ii-2]=='\n' && rt[ii-1]=='.') ii--;
   if (ii == 1 && rt[ii-1]=='.') ii=0;
   rt[ii] = 0;
   return rt;
}

template <unsigned int InputBufferSize, unsigned int OutputBufferSize>
struct TestDescription
{
   unsigned int inputBufferSize; 
   unsigned int outputBufferSize; 
   std::string content;
   
   TestDescription() :inputBufferSize(InputBufferSize),outputBufferSize(OutputBufferSize) {};
};

template <unsigned int InputBufferSize, unsigned int OutputBufferSize>
struct Empty :public TestDescription<InputBufferSize,OutputBufferSize>
{
   Empty() {};
};
template <unsigned int InputBufferSize, unsigned int OutputBufferSize>
struct OneEmptyLine :public TestDescription<InputBufferSize,OutputBufferSize>
{
   typedef TestDescription<InputBufferSize,OutputBufferSize> ThisTestDescription;
   OneEmptyLine() { ThisTestDescription::content.append("");};
};
template <unsigned int InputBufferSize, unsigned int OutputBufferSize>
struct OneOneCharLine :public TestDescription<InputBufferSize,OutputBufferSize>
{
   typedef TestDescription<InputBufferSize,OutputBufferSize> ThisTestDescription;
   OneOneCharLine() { ThisTestDescription::content.append("?\r\n");};
};
template <unsigned int InputBufferSize, unsigned int OutputBufferSize>
struct OneLine :public TestDescription<InputBufferSize,OutputBufferSize>
{
   typedef TestDescription<InputBufferSize,OutputBufferSize> ThisTestDescription;
   OneLine() { ThisTestDescription::content.append("Hello world!\r\n");};
};
template <unsigned int InputBufferSize, unsigned int OutputBufferSize, unsigned int MaxNofLines>
struct Random :public TestDescription<InputBufferSize,OutputBufferSize>
{
   typedef TestDescription<InputBufferSize,OutputBufferSize> ThisTestDescription;
   Random()
   {
      unsigned int ii=0,nn=rand()%MaxNofLines+1;
      while (ii++<=nn)
      {
         ThisTestDescription::content.append( getRandomAsciiString());
         ThisTestDescription::content.append( "\r\n");
      }
   }
};

static std::string escape( std::string& content)
{
   unsigned int ii,nn;
   std::string rt;
   for (ii=0,nn=content.size(); ii<nn; ii++)
   {
      if (content[ii] == '.' && (ii==0 || content[ii-1] == '\n'))
      {
         continue;
      }
      rt.push_back( content[ii]); 
   }
   return rt;
}


template <class TestDescription>
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
      TestDescription test;
      connection = new pecho::Connection( ep, test.inputBufferSize, test.outputBufferSize);

      input.clear();
      expected.clear();
      input.append( "echo\r\n");
      expected.append( "OK expecting command\r\n");
      expected.append( "OK enter data\r\n\r\n");

      input.append( test.content);
      expected.append( escape( test.content));

      input.append( ".\r\n");
      expected.append( "\r\nOK expecting command\r\n");
      input.append( "quit\r\n");
      expected.append( "BYE\r\n");
	}

   virtual void TearDown()
   {
      delete connection;
   }
};

typedef ::testing::Types<
   Empty<1,1>,
   OneEmptyLine<1,1>,
   OneOneCharLine<1,1>, 
   OneLine<1,1>, 
   Random<1,1,2000>,
   Empty<2,2>,
   OneEmptyLine<2,2>, 
   OneOneCharLine<2,2>, 
   OneLine<2,2>, 
   Random<2,2,2000>,
   Empty<3,3>, 
   OneEmptyLine<3,3>, 
   OneOneCharLine<3,3>, 
   OneLine<3,3>, 
   Random<3,3,2000>,
   Empty<4,4>, 
   OneEmptyLine<4,4>, 
   OneOneCharLine<4,4>, 
   OneLine<4,4>, 
   Random<4,4,8000>,
   Empty<1,2>, 
   OneEmptyLine<1,2>, 
   OneOneCharLine<1,2>, 
   OneLine<1,2>, 
   Random<1,2,1000>,
   Empty<2,3>, 
   OneEmptyLine<2,3>, 
   OneOneCharLine<2,3>, 
   OneLine<2,3>, 
   Random<2,3,1000>,
   Empty<3,4>, 
   OneEmptyLine<3,4>, 
   OneOneCharLine<3,4>, 
   OneLine<3,4>, 
   Random<3,4,24000>,
   Empty<2,1>, 
   OneEmptyLine<2,1>, 
   OneOneCharLine<2,1>, 
   OneLine<2,1>, 
   Random<2,1,1000>,
   Empty<3,2>, 
   OneEmptyLine<3,2>, 
   OneOneCharLine<3,2>, 
   OneLine<3,2>, 
   Random<3,2,1000>,
   Empty<4,3>, 
   OneEmptyLine<4,3>, 
   OneOneCharLine<4,3>, 
   OneLine<4,3>, 
   Random<4,3,24000>
   > MyTypes;
TYPED_TEST_CASE( pechoHandlerFixture, MyTypes);

TYPED_TEST( pechoHandlerFixture, ExpectedResult )
{
   std::string output;
   char* itr = const_cast<char*>( this->input.c_str());
   EXPECT_EQ( 0, test::runTestIO( itr, output, *this->connection));
#ifdef _SMERP_LOWLEVEL_DEBUG   
      unsigned int ii=0,nn=output.size();
      for (;ii<nn && output[ii]==this->expected[ii]; ii++);
      if (ii != nn) printf( "SIZE R=%u,E=%u,DIFF AT %u='%d %d %d %d|%d %d %d %d'\n", output.size(), this->expected.size(), ii, output[ii-2],output[ii-1],output[ii-0],output[ii+1],this->expected[ii-2],this->expected[ii-1],this->expected[ii-0],this->expected[ii+1]);
#endif   
   EXPECT_EQ( output, this->expected);
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}

