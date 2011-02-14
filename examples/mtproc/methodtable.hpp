#ifndef _SMERP_METHODTABLE_HPP_INCLUDED
#define _SMERP_METHODTABLE_HPP_INCLUDED
#include "protocol/generator.hpp"

namespace _SMERP {
namespace mtproc {

//method of the processor
struct Method
{
   //defined by the processor instance 
   struct Data;
   
   struct Context
   {
      Data* data;
      protocol::Generator* contentIterator;
      
      Context()                   :data(0),contentIterator(0){}
      void init( Data* p_data=0)  {data=p_data;contentIterator=0;}
   };
   //Method call
   //@return 0, in case of success, errorcode for client in case of error
   typedef int (*Call)( Context* context, unsigned int argc, const char** argv);
   
   const char* name;
   Call call;
};

//current instance with data of the processor
struct Instance
{
   const Method* mt;
   Method::Data* data;

   Instance()                    :mt(0),data(0){}
   Instance( const Instance& o)  :mt(o.mt),data(o.data){}
};

}}//namespace
#endif


