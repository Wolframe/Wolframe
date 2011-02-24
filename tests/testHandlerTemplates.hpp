//
// pechoHandler.hpp - simple echo handler example
//

#ifndef _SMERP_TEST_HANDLER_TEMPLATES_HPP_INCLUDED
#define _SMERP_TEST_HANDLER_TEMPLATES_HPP_INCLUDED
#include "logger.hpp"
#include <cstring>
#include <iostream>

namespace _SMERP
{

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
      std::memcpy( this, ((char*)&o), sizeof(*this));
   };
};


namespace test
{
   template <class Connection>
   int runTestIO( char* in, std::string& out, Connection& connection)
   {
      for (;;)
      {
         NetworkOperation netop( connection.nextOperation());
         
         switch (netop.operation())
         {
            case NetworkOperation::READ:
            {
               char* data = const_cast<char*>((char*)netop.data());
               unsigned int size = netop.size();
               unsigned int ii;
               for (ii=0; ii<size && *in; ii++,in++) data[ii]=*in;

               connection.networkInput( (void*)data, ii);
            }
            break;
            
            case NetworkOperation::WRITE:
            {
               char* data = (char*)netop.data();
               std::size_t ii,size = netop.size();                  
               for (ii=0; ii<size; ii++)
               {
                  out.push_back( data[ ii]);
               }
            }
            break;
            
            case NetworkOperation::CLOSE:
               return 0;
               
            default:
               continue;
         }
      }
      return 1;      
   }
}}//namespace _SMERP::test
#endif
