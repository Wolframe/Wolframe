/************************************************************************
Copyright (C) 2011 - 2013 Project Wolframe.
All rights reserved.

This file is part of Project Wolframe.

Commercial Usage
Licensees holding valid Project Wolframe Commercial licenses may
use this file in accordance with the Project Wolframe
Commercial License Agreement provided with the Software or,
alternatively, in accordance with the terms contained
in a written agreement between the licensee and Project Wolframe.

GNU General Public License Usage
Alternatively, you can redistribute this file and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Wolframe is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///
/// \file xmlIterator.hpp
/// \brief provides XML iterators of arbitrary source and UTF-8 output for processing bindings
///
#ifndef _Wolframe_PROTOCOL_XML_ITERATOR_HPP_INCLUDED
#define _Wolframe_PROTOCOL_XML_ITERATOR_HPP_INCLUDED
#include "protocol/generator.hpp"


namespace _Wolframe {
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
} // namespace _Wolframe

#endif // _Wolframe_PROTOCOL_TEXT_ITERATOR_HPP_INCLUDED

