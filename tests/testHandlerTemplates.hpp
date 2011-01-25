//
// pechoHandler.hpp - simple echo handler example
//

#ifndef _SMERP_TEST_HANDLER_TEMPLATES_HPP_INCLUDED
#define _SMERP_TEST_HANDLER_TEMPLATES_HPP_INCLUDED
#include "logger.hpp"
#include <cstring>
#include <stdio.h>

namespace _SMERP
{

//! THIS YOU CAN CALL A TRICKY (UNMASKING ACCESS RIGHTS BY INTRUSION)
//! ... OR YOU SHOULD BE CAREFUL WITH WHO YOU CALL YOUR FRIEND  :-)
template <typename T>
class connectionBase :public T
{
public:
   connectionBase() :T(T::READ) {};
   const void* data()                    { return T::data(); }
   std::size_t size()                    { return T::size(); }
   typename T::Operation operation()     { return T::operation(); }
};

class NetworkOperation :public connectionBase<Network::NetworkOperation>
{
public:
   NetworkOperation( const Network::NetworkOperation& o)
   {
      memcpy( this, ((char*)&o), sizeof(*this));
   };
};


namespace test
{
   template <class Connection>
   int runTestIO( char* in, std::string& out, Connection& connection)
   {
      enum {NetworkBufSize=8};
      char networkBuf[ NetworkBufSize];
      unsigned int networkBufPos = 0;
      
      for (;;)
      {
         NetworkOperation netop( connection.nextOperation());
         
         switch (netop.operation())
         {
            case NetworkOperation::READ:
            {
               //fprintf( stderr, "network operation is READ\n"); 
               if (networkBufPos == 0)
               {
                  for (;networkBufPos<NetworkBufSize; networkBufPos++)
                  {
                     int ch = *in++;
                     if (ch == 0) break;
                     networkBuf[ networkBufPos] = ch;
                  }
               }
               unsigned int len = (char*)connection.networkInput( networkBuf, networkBufPos)-networkBuf;
               memmove( networkBuf, networkBuf+len, networkBufPos-len);
               networkBufPos -= len;
            }
            break;
            
            case NetworkOperation::WRITE:
            {
               char* data = (char*)netop.data();
               std::size_t ii,size = netop.size();                  
               //fprintf( stderr, "network operation is WRITE '%.8s'[%u]\n", data, size); 
               for (ii=0; ii<size; ii++)
               {
                  out.push_back( data[ ii]);
               }
            }
            break;
            
            case NetworkOperation::CLOSE:
            case NetworkOperation::TERMINATE:
               //fprintf( stderr, "network operation is TERMINATE\n"); 
               return 0;
               
            default:
               continue;
         }
      }
      return 1;      
   }
}}//namespace _SMERP::test
#endif
