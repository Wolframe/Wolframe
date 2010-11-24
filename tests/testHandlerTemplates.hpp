//
// pechoHandler.hpp - simple echo handler example
//

#ifndef _SMERP_TEST_HANDLER_TEMPLATES_HPP_INCLUDED
#define _SMERP_TEST_HANDLER_TEMPLATES_HPP_INCLUDED
#include "logger.hpp"
#include <stdio.h>

namespace _SMERP
{
namespace test
{
   //template for a test of a connection handler using FILE I/O
   //@return 0, if ok, some errorcode as main would else
   template<class Connection>
   int runTestFileIO( FILE* in, FILE* out, Connection& connection)
   {
      try
      {
         for (;;)
         {
            Network::NetworkOperation netop( connection.nextOperation());
            
            switch (netop.operation())
            {
               case Network::NetworkOperation::READ:
               {
                  //fprintf( stderr, "network operation is READ\n"); 
                  char* data = (char*)netop.data();
                  std::size_t ii,size = netop.size();
                  for (ii=0; ii<size; ii++)
                  {
                     int ch = getc( in);
                     if (ch == EOF) break;
                     data[ ii] = ch;
                  }
                  connection.parseInput( netop.data(), ii);
               }
               break;
               
               case Network::NetworkOperation::WRITE:
               {
                  char* data = (char*)netop.data();
                  std::size_t ii,size = netop.size();                  
                  //fprintf( stderr, "network operation is WRITE '%.8s'[%u]\n", data, size); 
                  for (ii=0; ii<size; ii++)
                  {
                     putc( data[ ii], out);
                     fflush( out);
                  }
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
      catch (...)
      {
         fprintf( stderr, "an unexpected exception (this is a test program)\n"); 
      };
      return 1;      
   }
}}//namespace _SMERP::test
#endif
