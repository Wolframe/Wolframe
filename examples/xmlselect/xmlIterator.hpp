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
   friend class Source;
   struct Private;
   Private* data;
};


class Source
{
public:
   enum State {Init, Processing, EndOfBuffer, EndOfInput, Error};

   //interface for iterator over input content elements
   class const_iterator
   {
   public:            
      class Element
      {
      protected:
         friend class Source;
         char* m_buf;
         unsigned int m_bufsize;
         unsigned int m_size;
         int m_type;
         
         Element( char* buf, unsigned int bufsize)               :m_buf(buf),m_bufsize(bufsize),m_type(0){};
         
      public:
         const char* value() const                               {return m_buf;};
         unsigned int size() const                               {return m_size;};
         int type() const                                        {return m_type;};
      };

      const_iterator( char* outbuf, unsigned int outbufsize)     :m_cur(outbuf,outbufsize),m_state(Init){};
      const_iterator()                                           :m_cur(0,0),m_state(EndOfInput){};
      const Element& operator*() const                           {return m_cur;};
      const Element* operator->() const                          {return &m_cur;};
      const_iterator& operator++()                               {skip(); return *this;};
      bool operator==( const const_iterator& o) const            {return m_state!=Processing && m_state==o.m_state;};
      bool operator!=( const const_iterator& o) const            {return m_state==Processing || m_state!=o.m_state;};
      
      virtual ~const_iterator() {};
      virtual void skip()=0;
      virtual void feed( char* block, unsigned int blocksize)=0;
      State state() const                                        {return m_state;};

   protected:
      Element m_cur;
      State m_state;
   };

   //create iterator over the XML header
   const_iterator* createXMLHeaderIterator( char* outbuf, unsigned int outbufsize);
   //create iterator over the XML elements
   const_iterator* createXMLElementIterator( const char* charset, char* outbuf, unsigned int outbufsize);
   //create iterator over the XPath selected elements
   const_iterator* createXMLSelectIterator( const char* charset, char* outbuf, unsigned int outbufsize, const Automaton* atm);
};


} // namespace xml
} // namespace _SMERP

#endif // _SMERP_PROTOCOL_TEXT_ITERATOR_HPP_INCLUDED

