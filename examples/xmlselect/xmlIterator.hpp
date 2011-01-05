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
   Source( protocol::buffers::InputBlock* inputBlock);
     
   class iterator
   {
      virtual bool next( int* type, char* buf, unsigned int bufsize)=0;
   };
   enum Charset {UTF8,IsoLatin1};
   
   iterator elementIterator( const Source& src, const iterator& itr, Charset charset);
   iterator selectIterator( const Source& src, const iterator& itr, Charset charset, Automaton* atm);

private:
   struct Private;
   Private* data;
};
*/

} // namespace xml
} // namespace _SMERP

#endif // _SMERP_PROTOCOL_TEXT_ITERATOR_HPP_INCLUDED

