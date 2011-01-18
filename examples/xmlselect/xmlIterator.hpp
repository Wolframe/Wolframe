#ifndef _SMERP_PROTOCOL_XML_ITERATOR_HPP_INCLUDED
#define _SMERP_PROTOCOL_XML_ITERATOR_HPP_INCLUDED
#include "protocol/generator.hpp" 

//@section xmlContentIterators
//provides 2 types of XML iterators of arbitrary source and UTF-8 output for processing bindings
// [1] generator for iterating over the xml elements like tag name etc.
// [2] generator for iterating over a selection by an automaton of a subset of abbreviated syntax of XPath

namespace _SMERP {
namespace xml {

class Automaton
{
public:
   Automaton();
   bool defineExpression( int type, const char* expression, int* errorPos=0);

private:
   friend class Source_UTF8;
   struct Private;
   Private* data;
};

//values of the generator elements type := Element::type() 
//for the generators 'XMLHeaderIterator' and 'XMLElementIterator'
enum ElementType
{
   Unknown=0,           //< unknown element -> means have a look at the state. 
   HeaderAttribName,    //< name of an attribute in the XML header
   HeaderAttribValue,   //< value of an attribute in the XML header
   HeaderEnd,           //< end of the XML header. can be used to switch the iterator
   TagAttribName,       //< name of a tag attribute
   TagAttribValue,      //< name of a tag attribute
   OpenTag,             //< name of a tag in an open tag
   CloseTag,            //< close of a tag (without name !)
   CloseTagIm,          //< close of a tag with />
   Content              //< content element - they are sperated by spaces
};

class Source_UTF8
{
public:
   //create iterator over the XML header
   //@param outbuf buffer for the elements produced by the generator
   //@param outbufsize allocation size of outbuf in bytes
   //@param prev generator to inherit the input context from (if generators are switched)
   protocol::Generator* createXMLHeaderIterator( char* outbuf, unsigned int outbufsize, protocol::Generator* prev=0);

   //create iterator over the XML elements
   //@param charset character set encoding of the input processed by the generator as string (output is UTF-8)
   //@param outbuf buffer for the elements produced by the generator
   //@param outbufsize allocation size of outbuf in bytes
   //@param prev generator to inherit the input context from (if generators are switched)
   protocol::Generator* createXMLElementIterator( const char* charset, char* outbuf, unsigned int outbufsize, protocol::Generator* prev=0);

   //create iterator over the XPath selected elements
   //@param charset character set encoding of the input processed by the generator as string (output is UTF-8)
   //@param outbuf buffer for the elements produced by the generator
   //@param outbufsize allocation size of outbuf in bytes
   //@param prev generator to inherit the input context from (if generators are switched)
   protocol::Generator* createXMLSelectIterator( const char* charset, char* outbuf, unsigned int outbufsize, const Automaton* atm, protocol::Generator* prev=0);
};

} // namespace xml
} // namespace _SMERP

#endif // _SMERP_PROTOCOL_TEXT_ITERATOR_HPP_INCLUDED

