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
   template<typename Input>
   int getc( Input&){char a[sizeof(Input)!=0]; return 0;}//not defined!
   
   template<>
   int getc( char*& in)
   {
      return *in++;
   }

   template<>
   int getc( FILE*& in)
   {
      return ::getc( in);
   }

   template<typename Output>
   void putc( char, Output*){char a[sizeof(Output)!=0];}//not defined!

   template<>
   void putc( char ch, std::string* out)
   {
      out->push_back( ch);
   }

   template<>
   void putc( char ch, FILE* out)
   {
      ::putc( ch, out);
   }

   //template for a test of a connection handler using FILE I/O
   //@return 0, if ok, some errorcode as main would else
   template<typename Input, typename Output, class Connection>
   int runTestIO( Input*& in, Output* out, Connection& connection)
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
               }
            }
            break;
            
            case Network::NetworkOperation::TERMINATE:
               //fprintf( stderr, "network operation is TERMINATE\n"); 
               return 0;
         }
      }
      return 1;      
   }
}}//namespace _SMERP::test
#endif
