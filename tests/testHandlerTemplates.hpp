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
   struct FileInput
   {
      FILE* file;
      FileInput( const FileInput& o) :file(o.file) {};
      FileInput( FILE* p_file) :file(p_file) {};
      int get() {return getc(file);};
   };
   struct FileOutput
   {
      FILE* file;
      FileOutput( const FileOutput& o) :file(o.file) {};
      FileOutput( FILE* p_file) :file(p_file) {};
      void put( char ch) {putc(ch,file);};
   };
   struct StringInput
   {
      char* itr;
      StringInput( const StringInput& o) :itr(o.itr) {};
      StringInput( char* p_itr) :itr(p_itr) {};
      int get() {ch = *itr++; return (ch)?ch:EOF};
   };
   struct StringOutput
   {
      std::string* buf;
      StringOutput( const StringOutput& o) :buf(o.buf) {};
      StringOutput( std::string* p_buf) :buf(p_buf) {};
      void put( char ch) {buf->push_back(ch);};
   };
   template<typename Input, typename Output, class Connection>
   int runTestIO_( Input in, Output out, Connection& connection)
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
                  int ch = in.get();
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
                  out.put( data[ ii]);
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
   
   template<class Connection>
   int runTestIO( char* input, std::string* output, Connection& connection)
   {
      StringInput in(input);
      StringOutput out(output);
      return runTestIO_( in, out, connection);
   }
   template<class Connection>
   int runTestIO( FILE* input, FILE* output, Connection& connection)
   {
      FileInput in(input);
      FileOutput out(output);
      return runTestIO_( in, out, connection);
   }
}}//namespace _SMERP::test
#endif
