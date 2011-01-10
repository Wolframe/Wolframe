#ifndef _SMERP_PROTOCOL_XML_ITERATOR_HPP_INCLUDED
#define _SMERP_PROTOCOL_XML_ITERATOR_HPP_INCLUDED
#include "protocol.hpp"

//@section xmlContentIterators
//provides a uniform interface to 2 types of XML iterators of arbitrary source and 
//UTF-8 output:
// [1] element iterators iterate over the basi xml elements like tag name etc.
// [2] select iterators provide a selection by an automaton of abbreviated syntax of XPath

namespace _SMERP {
namespace xml {
   
class Automaton
{
public:
   Automaton();
   bool defineExpression( int type, const char* expression, int* errorPos=0);

private:
   struct Private;
   Private* data;
};

/* TODO
class Source
{
public:
   Source() :block(""),blocksize(0) {};
     
   initSourceBlock( const char* p_block, unsigned int p_blocksize)
   {
      block = p_block;
      blocksize = p_blocksize;
   };
   
   class iterator
   {
   public:
      enum State {Processing, EndOfBuffer, EndOfInput};
            
      class Element
      {
      protected:
         friend class Source;
         char* buf;
         unsigned int bufsize;
         unsigned int m_size;
         int m_type;
         Element( unsigned int p_bufsize)                        :buf(0),bufsize(p_bufsize),m_type(0) {buf=new char[bufsize?bufsize:1]; buf[0]=0;};
         Element( const Element& o)                              :bufsize(o.bufsize),m_size(o.m_size),m_type(o.m_type) {buf=new char[bufsize]; memcpy(buf,o.buf,bufsize);};
         
      public:
         const char* value() {return buf;};
         unsigned int size() {return m_size;};
         int type() {return m_type;};
      };

      iterator( const iterator& o)                               :src(o.src),cur(o.cur),m_state(o.m_state){};
      iterator( Source* p_src, unsigned int bufsize)             :src(p_src),cur(bufsize){};
      iterator()                                                 :src(0),cur("",0){};
      const Element& operator*() const                           {return cur;};
      const Element* operator->() const                          {return &cur;};
      iterator& operator++()                                     {skip(); return *this;};
      iterator& operator++(int)                                  {iterator rt=*this; skip(); return rt;};
      
      virtual ~iterator() {};
      virtual void skip()=0;
      virtual unsigned int pos() const=0;
      State state() const                                        {return m_state;};

   private:
      Source* src;
      Element cur;
      State m_state;
   };
   enum Charset {UTF8,IsoLatin1};
   
   iterator elementIterator( Charset charset);
   iterator selectIterator( Charset charset, Automaton* atm);

private:
   const char* block;
   unsigned int blocksize;
};

*/

} // namespace xml
} // namespace _SMERP

#endif // _SMERP_PROTOCOL_TEXT_ITERATOR_HPP_INCLUDED

