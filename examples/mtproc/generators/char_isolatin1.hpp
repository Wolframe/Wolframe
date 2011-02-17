#ifndef _SMERP_PROTOCOL_GENERATOR_CHAR_ISOLATIN1_HPP_INCLUDED
#define _SMERP_PROTOCOL_GENERATOR_CHAR_ISOLATIN1_HPP_INCLUDED
#include "protocol/generator.hpp"

namespace _SMERP {
namespace mtproc {

namespace isolatin1 {

class Generator :public protocol::Generator
{
public:
   Generator() :m_ar(0),m_pos(0),m_size(0){}
   
   virtual ~Generator() {}
   
   //used to go to next element (see example below)
   virtual bool next( const void** elem, unsigned int* size)
   {
      if (m_pos >= m_size) return false;
      m_pos++;
      *elem = (const void*)(m_ar+m_pos);
      *size = 1;
      return true;
   }

   virtual void protocolInput( const void* block, unsigned int blocksize)
   {
      m_ar = (const char*)block;
      m_pos = 0;
      m_size = blocksize;
   }
   
   virtual void getRestBlock( const void** block, unsigned int* blocksize)
   {
      *block = m_ar + m_pos;
      *blocksize = (m_pos >= m_size)?0:(m_size-m_pos);
   }

private:
   const char* m_ar;
   unsigned int m_pos;
   unsigned int m_size;
};

}//namespace isolatin1
}}//namespace
#endif


