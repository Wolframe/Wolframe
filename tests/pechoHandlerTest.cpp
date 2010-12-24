//
// tests a simple protocol handler (pechoHandler) std input

#include "pechoHandler.hpp"
#include "testHandlerTemplates.hpp"
#include <cstdio>

using namespace _SMERP;

LogBackend logBack;

int main( int, const char**)
{
   Network::LocalTCPendpoint ep( "127.0.0.1", 12345);
   pecho::Connection connection( ep);
   try
   {
      return test::runTestIO( stdin, stdout, connection);
   }
   catch (std::exception e)
   {
      fprintf( stderr, "exception %s\n", e.what());
   }
   catch (...)
   {
      fprintf( stderr, "an unexpected exception (this is a test program)\n"); 
   }
   return 1;
}
