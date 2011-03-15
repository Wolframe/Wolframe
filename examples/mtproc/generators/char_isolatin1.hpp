#ifndef _Wolframe_PROTOCOL_GENERATOR_CHAR_ISOLATIN1_HPP_INCLUDED
#define _Wolframe_PROTOCOL_GENERATOR_CHAR_ISOLATIN1_HPP_INCLUDED
#include "protocol/generator.hpp"
#include <cstring>

namespace _Wolframe {
namespace mtproc {

namespace generator {

struct CharIsoLatin1
{
   typedef protocol::Generator Generator;
   typedef protocol::FormatOutput FormatOutput;

   enum ErrorCodes {Ok=0,ErrBufferTooSmall=1};
   static bool GetNext( Generator* this_, Generator::ElementType* type, void* buffer, unsigned int buffersize, unsigned int* bufferpos)
   {
      char* in = (char*)this_->ptr();
      unsigned int nn = this_->size();
      *type = Generator::Value;

      if (buffersize == *bufferpos)
      {
         this_->setState( Generator::Error, ErrBufferTooSmall);
         return false;
      }
      else if (nn == 0)
      {
         if (this_->gotEoD())
         {
            this_->setState( Generator::Open);
            return false;
         }
         else
         { 
            this_->setState( Generator::EndOfMessage);
            return false;
         }
      }
      else
      {
         this_->setState( Generator::Open);
         ((char*)buffer)[*bufferpos] = *in;
         this_->skip( 1);
         *bufferpos += 1;
         return true;
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


