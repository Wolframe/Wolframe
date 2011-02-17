#ifndef _SMERP_MTPROC_IMPLEMENTATION_HPP_INCLUDED
#define _SMERP_MTPROC_IMPLEMENTATION_HPP_INCLUDED
#include "methodtable.hpp"

namespace _SMERP {
namespace mtproc {

//example mtprocHandler object implementation in C++ish form
struct Method::Data :public Instance
{
   //*methods (context->data points to this)

   //echo the content
   static int echo( Context* context, unsigned int argc, const char** argv);

   //echo the arguments without referencing content
   static int printarg( Context* context, unsigned int argc, const char** argv);

   //data
   Data()
   {
      static Method mt_[3] = {{"echo",&echo},{"parg",&printarg},{0,0}};
      mt = mt_;
      data = this;
   }
};

}}//namespace
#endif


