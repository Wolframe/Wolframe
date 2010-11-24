//
// tests a protocol (pechoHandler) std input

//compile: g++ -c -o pechoHandlerTest.o -g -fstrict-aliasing -pedantic -Wall -Wunused -Wno-import -Wformat -Wformat-y2k -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wswitch-enum -Wunknown-pragmas -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Wwrite-strings -Wmissing-noreturn -Wno-multichar -Wparentheses -Wredundant-decls -Winline -Wdisabled-optimization -Wno-long-long -Werror -Wfatal-errors pechoHandlerTest.cpp
//compile: g++ -c -o ../src/pechoHandler.o -DMODULE_TEST -g -fstrict-aliasing -pedantic -Wall -Wunused -Wno-import -Wformat -Wformat-y2k -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wswitch-enum -Wunknown-pragmas -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Wwrite-strings -Wmissing-noreturn -Wno-multichar -Wparentheses -Wredundant-decls -Winline -Wdisabled-optimization -Wno-long-long -Werror -Wfatal-errors ../src/pechoHandler.cpp
//link     g++ -lc -o pechoHandlerTest pechoHandlerTest.o pechoHandler.o

#include "pechoHandler.hpp"
#include "logger.hpp"
#include <stdio.h>
#include <string.h>

using namespace _SMERP;

LogBackend   logBack;

struct Bad {};

int main( int, const char**)
{
   Network::LocalTCPendpoint ep( "127.0.0.1", 12345);
   pecho::Connection test( ep);

   try
   {
      for (;;)
      {
          Network::NetworkOperation netop( test.nextOperation());

          switch (netop.operation())
          {
              case Network::NetworkOperation::READ:
              {
                  //fprintf( stderr, "network operation is READ\n"); 
                  char* data = (char*)netop.data();
                  std::size_t size = netop.size();
                  std::size_t nn = fread( data, 1, size, stdin);
                  test.parseInput( netop.data(), nn);
              }
              break;
              
              case Network::NetworkOperation::WRITE:
              {
                  char* data = (char*)netop.data();
                  std::size_t size = netop.size();                  
                  //fprintf( stderr, "network operation is WRITE '%.8s'[%u]\n", data, size); 
                  fwrite( data, 1, size, stdout);
                  fflush( stdout);
              }
              break;
              
              case Network::NetworkOperation::TERMINATE:
                  //fprintf( stderr, "network operation is TERMINATE\n"); 
                  return 0;
              break;
          }
      }
   }
   catch (std::exception e)
   {
       fprintf( stderr, "exception %s\n", e.what()); 
   }
   catch (Bad)
   {
       fprintf( stderr, "a bad exception (this is a test program)\n"); 
   }
   catch (...)
   {
       fprintf( stderr, "an unexpected exception (this is a test program)\n"); 
   };
   return 1;
}


