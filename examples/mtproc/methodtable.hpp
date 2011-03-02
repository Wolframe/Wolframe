#ifndef _Wolframe_METHODTABLE_HPP_INCLUDED
#define _Wolframe_METHODTABLE_HPP_INCLUDED
#include "protocol/generator.hpp"

namespace _Wolframe {
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
      protocol::FormatOutput* output;
      
      Context()                   :data(0),contentIterator(0),output(0){}
      void init( Data* d=0)       {data=d;contentIterator=0;output=0;}

      typedef Data* (*DataConstructor)();
      typedef void (*DataDestructor)( Data* context);
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
   const Method* m_mt;
   Method::Data* m_data;
   Method::Context::DataConstructor m_createData;
   Method::Context::DataDestructor m_destroyData;

   Instance()                    :m_mt(0),m_data(0),m_createData(0),m_destroyData(0){}
   Instance( const Instance& o)  :m_mt(o.m_mt),m_data(o.m_data),m_createData(o.m_createData),m_destroyData(o.m_destroyData){}
};

}}//namespace
#endif


