#ifndef _SMERP_PROTOCOL_GENERATOR_CHAR_ISOLATIN1_HPP_INCLUDED
#define _SMERP_PROTOCOL_GENERATOR_CHAR_ISOLATIN1_HPP_INCLUDED
#include "protocol/generator.hpp"
#include <cstring>

namespace _SMERP {
namespace mtproc {

namespace generator {

struct CharIsoLatin1
{
   typedef protocol::Generator Generator;
   typedef protocol::FormatOutput FormatOutput;

   enum ErrorCodes {Ok=0,ErrBufferTooSmall=1};
   static int GetNext( Generator* this_, void* buffer, unsigned int buffersize)
   {
      char* in = (char*)this_->ptr();
      unsigned int nn = this_->size();

      if (buffersize == 0)
      {
         this_->setState( Generator::Error, ErrBufferTooSmall);
         return 0;
      }
      else if (nn == 0)
      {
         this_->setState( this_->gotEoD()?Generator::EndOfInput:Generator::EndOfMessage, Ok);
         return 0;
      }
      else
      {
         this_->setState( Generator::Processing, Ok);
         *(char*)buffer = *in;
         this_->skip( 1);
         return 1;
      }
   }

   static bool Print( FormatOutput* this_, int, void* element, unsigned int elementsize)
   {
      char* out = (char*)this_->cur();
      unsigned int nn = this_->restsize();

      if (elementsize > nn)
      {
         return false;
      }
      else
      {
         std::memcpy( out, element, elementsize);
         this_->incr( elementsize);
         return true;
      }
   }
};

}//namespace generator1
}}//namespace
#endif


