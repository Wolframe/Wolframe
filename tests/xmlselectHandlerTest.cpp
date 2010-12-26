//
// tests a simple protocol handler (pechoHandler) std input

#include "xmlselectHandler.hpp"
#include "testHandlerTemplates.hpp"
#include <stdio.h>

using namespace _SMERP;

LogBackend& logBack = _SMERP::LogBackend::instance( );

int main( int, const char**)
{
   Network::LocalTCPendpoint ep( "127.0.0.1", 12345);
   xmlselect::Connection connection( ep);
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




