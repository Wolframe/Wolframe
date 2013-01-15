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
#include "xmlIterator.hpp"
#include "protocol/buffer.hpp"
#include "textwolf.hpp"
#include <vector>
#include <cstring>

namespace tw = textwolf;
namespace pt = _Wolframe::protocol;
namespace xx = _Wolframe::xml;

namespace
{
   struct XPathExpression
   {
      public:
         struct Error {};
         
         struct Element
         {
            std::string name;
            enum Type
            {
               Tag,
               Follow,
               AttributeName,
               AttributeValue,
               RangeStart,
               RangeEnd
            };
            Type type;
            
            Element()                                                           :type(Tag) {};
            Element( Type p_type)                                               :type(p_type) {};
            Element( Type p_type, const char* p_name, unsigned int p_namesize)  :name(p_name,p_namesize),type(p_type) {};
            Element( Type p_type, const std::string& p_name)                    :name(p_name),type(p_type) {};
            Element( const Element& o)                                          :name(o.name),type(o.type) {};
         };
      private:
         const char* input;
         std::vector<Element> ar;
         unsigned int pos;
         bool valid;
         
         void skipB()
         {
            while (input[pos] != '\0' && input[pos]<=' ') pos++;
         };
         void skip( unsigned int nn=1)
         {
            while (nn>0 && input[pos+nn]) --nn;
            skipB();
         };
         char cur() const
         {
            return input[pos];
         };
         bool match( const char* str) const
         {
            unsigned int ii=0;
            while (input[pos+ii] != '\0' && input[pos+ii] == str[ii]) ii++;
            return (str[ii] == '\0');
         };
         bool match( char ch) const
         {
            return cur()==ch;
         };
         
         void parseNumber( Element::Type type)
         {
            const char* name = input+pos;
            unsigned int ii;
            for (ii=0; name[ii] >= '0' && name[ii] <= '9'; ii++);
            if (ii == 0) throw Error();
            ar.push_back( Element( type, name, ii));
            pos += ii;
            skipB();
         };

         void parseRange()
         {
            if (match( ".."))
            {
               skip(2);
               parseNumber( Element::RangeEnd);
            }
            else
            {
               parseNumber( Element::RangeStart);
               if (match( ".."))
               {
                  skip(2);
                  if (cur() != ']')
                  {
                     parseNumber( Element::RangeEnd);
                  }
               }
               else if (cur() == ']')
               {
                  ar.push_back( Element( Element::RangeEnd, ar[ ar.size()-1].name));
               }
               else
               {
                  throw Error();
               }
            }
            skipB();
         };

         void parseName( Element::Type type)
         {
            const char* name = input+pos;
            unsigned int ii;
            for (ii=0; name[ii] != '/' && name[ii] != '[' && name[ii] != '@' && name[ii] != '=' && name[ii] != '\0'; ii++);
            if (ii == 0) throw Error();
            ar.push_back( Element( type, name, ii));
            pos += ii;
            skipB();
         };

         void parseString( Element::Type type)
         {
            char eb = input[pos++];
            const char* name = input+pos;
            unsigned int ii;
            for (ii=0; name[ii] != eb && name[ii] != '\0'; ii++);
            if (name[ii] == '\0') throw Error();
            ar.push_back( Element( type, name, ii));
            pos += ii+1;
            skipB();
         };

         void parseCondition()
         {
            if (match( '@'))
            {
               skip();
               parseName( Element::AttributeName);
               if (match( '='))
               {
                  skip();
                  if (match( '\"') || match( '\''))     
                  {
                     parseString( Element::AttributeValue);
                  }
                  else
                  {
                     parseName( Element::AttributeValue);
                  }
               }
            }
            else
            {
               parseRange();
            }
            if (cur() != ']') throw Error();
            skip();
         };

         void parse()
         {
            while (cur() != '\0')
            {
               if (match("//"))
               {
                  ar.push_back( Element( Element::Follow));
                  skip(2);
               }
               else if (match('/'))
               {
                  skip();
               }
               else if (match('@'))
               {
                  parseName( Element::AttributeName);
               }
               else if (match('['))
               {
                  parseCondition();
               }
               else
               {
                  parseName( Element::Tag);
               }
            }
         };

      public:
         XPathExpression( const char* p_input)      :input(p_input),pos(0),valid(true)
         {
            try
            {
               parse();
            }
            catch (Error)
            {
               valid = false;
            };
         };
         
         int errorPos()
         {
            return (valid)?-1:(int)pos;
         };
         
         typedef std::vector<Element>::iterator iterator;
         
         iterator begin()  {return ar.begin();};
         iterator end()    {return ar.end();};
   };


   struct Input  :public pt::Generator
   {
      pt::InputBlock m_block;
      typedef pt::InputBlock::const_iterator MemBlockIterator;
      typedef pt::TextIterator< MemBlockIterator> Iterator;

      MemBlockIterator m_memBlockIterator;
      Iterator m_iterator;

      Input()                                            :m_memBlockIterator( &m_block),m_iterator( &m_memBlockIterator) {};

      Iterator& iterator()                               {return m_iterator;};

      virtual void feed( void* block, unsigned int blocksize)
      {
         m_block.set( block, blocksize);
      };

      virtual void getRestBlock( void** block, unsigned int* blocksize)
      {
         *block = m_block.charptr() + m_block.pos();
         *blocksize = m_block.size() - m_block.pos();
      };

      virtual bool skip() {return false;};
   };


   template <typename Charset_, typename Scanner_>
   struct ElementIteratorBase
   {
      typedef pt::InputBlock::const_iterator MemBlockIterator;
      typedef pt::TextIterator<MemBlockIterator> ContentIterator;
      typedef Charset_ Charset;
      typedef Scanner_ Scanner;
      typedef typename Scanner::iterator ScannerIterator;
      typedef xx::Source_UTF8 Source;
      
      char* m_outbuf;
      unsigned int m_outbufsize;
      Scanner* m_scanner;
      ScannerIterator m_itr;
      
      ElementIteratorBase( char* outbuf, unsigned int outbufsize, Scanner* p_scanner) 
            :m_outbuf( outbuf),
             m_outbufsize( outbufsize),
             m_scanner( p_scanner),
             m_itr( m_scanner->begin()) {};
   };


   template <typename Charset>
   struct XMLElementIterator  :public Input
   {
      typedef tw::XMLScanner<Input::Iterator,Charset,tw::charset::UTF8> Scanner;
      typedef typename Scanner::ElementType ScannerElementType;
      typedef ElementIteratorBase<Charset, Scanner> Base;
      typedef xx::Source_UTF8 Source;
      
      Scanner scanner;
      Base base;
      
      XMLElementIterator( char* outbuf, unsigned int outbufsize)
            :scanner( iterator(), outbuf, outbufsize),
             base( outbuf, outbufsize, &scanner) {};

      virtual ~XMLElementIterator() {};
      
      struct ElementTypeMap
      {
         xx::ElementType ar[ Scanner::NofElementTypes];
         ElementTypeMap()
         {
            ar[ Scanner::None] = xx::Unknown;
            ar[ Scanner::ErrorOccurred] = xx::Unknown;
            ar[ Scanner::HeaderAttribName] = xx::HeaderAttribName;
            ar[ Scanner::HeaderAttribValue] = xx::HeaderAttribValue;
            ar[ Scanner::HeaderEnd] = xx::HeaderEnd;
            ar[ Scanner::TagAttribName] = xx::TagAttribName;
            ar[ Scanner::TagAttribValue] = xx::TagAttribValue;
            ar[ Scanner::OpenTag] = xx::OpenTag;
            ar[ Scanner::CloseTag] = xx::CloseTag;
            ar[ Scanner::CloseTagIm] = xx::CloseTagIm;
            ar[ Scanner::Content] = xx::Content;
            ar[ Scanner::Exit] = xx::Unknown;
         };
         xx::ElementType operator[]( ScannerElementType e) const {return ar[e];};
      };
      
      virtual bool skip()
      {
         static ElementTypeMap etm;
         try
         {
            base.m_itr++;
            m_cur.init( base.m_itr->content, base.m_itr->size, etm[ base.m_itr->type]);
                        
            if (base.m_itr->type == Scanner::None || base.m_itr->type == Scanner::ErrorOccurred)
            {
               m_state = pt::Generator::Error;
               return false;
            }
            else if (base.m_itr->type == Scanner::Exit)
            {
               m_state = pt::Generator::EndOfInput;
               return false;
            }
            else
            {
               m_state = pt::Generator::Processing;
               return true;
            }
         }
         catch (pt::InputBlock::End)
         {
            m_state = pt::Generator::EndOfBuffer;
         };
         return false;
      };
   };


   template <typename Charset>
   struct XMLSelectIterator  :public Input
   {
      typedef std::map< const char*,tw::UChar> EntityMap;
      typedef tw::XMLPathSelectAutomaton< tw::charset::UTF8> Automaton;
      typedef tw::XMLPathSelect< Input::Iterator,Charset,tw::charset::UTF8,EntityMap> Scanner;
      typedef ElementIteratorBase< Charset, Scanner> Base;
      typedef xx::Source_UTF8 Source;
      
      Scanner scanner;
      Base base;
      
      XMLSelectIterator( char* outbuf, unsigned int outbufsize, Automaton* automaton, EntityMap* entityMap)
            :scanner( automaton, iterator(), outbuf, outbufsize, entityMap),
             base( outbuf, outbufsize, &scanner) {};       
      
      virtual ~XMLSelectIterator() {};
      
      virtual bool skip()
      {
         try
         {
            base.m_itr++; 
            m_cur.init( base.m_itr->content, base.m_itr->size, base.m_itr->type);
            
            switch (base.m_itr->state)
            {
               case Scanner::iterator::Element::Ok:          m_state = pt::Generator::Processing; break;
               case Scanner::iterator::Element::EndOfOutput: m_state = pt::Generator::Error; break;
               case Scanner::iterator::Element::EndOfInput:  m_state = pt::Generator::EndOfInput; break;
               case Scanner::iterator::Element::ErrorState:  m_state = pt::Generator::Error; break;
            }
         }
         catch (pt::InputBlock::End)
         {
            m_state = pt::Generator::EndOfBuffer;
         };
         return (m_state == pt::Generator::Processing);
      };
   };


   enum Charset {Unknown,IsoLatin1, UTF8};
   static Charset getCharset( const char* charset)
   {
      std::string cc;
      for (unsigned int ii=0; charset[ii]; ii++)
      {
         if (charset[ii] != '-') cc.push_back( tolower(charset[ii]));
      }
      if (strstr( "utf8", cc.c_str()))
      {
         return UTF8;
      }
      else if (strstr( "isolatin", cc.c_str()))
      {
         return IsoLatin1;
      }
      else if (strstr( "8859", cc.c_str()))
      {
         return IsoLatin1;
      }
      return Unknown;
   }
}//anonymous namespace



//public implementation
using namespace _Wolframe;
using namespace _Wolframe::xml;

struct Automaton::Private
{
   friend class Source;
   typedef tw::XMLPathSelectAutomaton<tw::charset::UTF8> ThisAutomaton;
   ThisAutomaton implementation;
   
   bool defineExpression( int type, const char* expression, int* errorpos)
   {
      XPathExpression xpe( expression);
      int ee = xpe.errorPos();
      if (errorpos) *errorpos = ee;
      if (ee >= 0) return false;
      
      XPathExpression::iterator itr = xpe.begin(), end = xpe.end(); 
      XPathExpression::Element attribute;
      ThisAutomaton::PathElement elem( &implementation);
      
      while (itr != end)
      {
         switch (itr->type)
         {
            case XPathExpression::Element::Tag:
            {
               elem[ itr->name.c_str()];
               itr++;
            }
            break;
            
            case XPathExpression::Element::Follow:
            {
               elem--;
               itr++;
            }  
            break;

            case XPathExpression::Element::AttributeName:
            {
               XPathExpression::iterator aa = itr++;
               if (itr != end && itr->type == XPathExpression::Element::AttributeValue)
               {
                  elem( aa->name.c_str(), itr->name.c_str());
               }
               else
               {
                  elem( aa->name.c_str());
               }
            }
            break;

            case XPathExpression::Element::AttributeValue:
            {
               elem( 0, itr->name.c_str());
               itr++;
            }
            break;

            case XPathExpression::Element::RangeStart:
            {
               elem.FROM( atoi( itr->name.c_str()));
               itr++;
            }
            break;

            case XPathExpression::Element::RangeEnd:
            {
               elem.TO( atoi( itr->name.c_str()));
               itr++;
            }
            break;

            default: return false;
         }
      }
      elem = type;
      return true;
   };
};

Automaton::Automaton()
{
   data = new Private();
}

bool Automaton::defineExpression( int type, const char* expression, int* errorpos)
{
   return data->defineExpression( type, expression, errorpos);
}

static void inheritBuffer( protocol::Generator* to, protocol::Generator* from)
{
   if (to && from)
   {
      void* buf = 0;
      unsigned int bufsize = 0;
      from->getRestBlock( &buf, &bufsize);
      to->feed( buf, bufsize);
   }
}

pt::Generator* Source_UTF8::createXMLElementIterator( const char* charset, char* outbuf, unsigned int outbufsize, pt::Generator* prev)
{
   protocol::Generator* rt = 0;
   switch (getCharset(charset))
   {
      case UTF8:      rt = new XMLElementIterator<tw::charset::UTF8>( outbuf, outbufsize);
      case IsoLatin1: rt = new XMLElementIterator<tw::charset::IsoLatin1>( outbuf, outbufsize);
      case Unknown:   rt = 0;
   }
   inheritBuffer( rt, prev);
   return rt;
}

pt::Generator* Source_UTF8::createXMLSelectIterator( const char* charset, char* outbuf, unsigned int outbufsize, const Automaton* atm, pt::Generator* prev)
{
   protocol::Generator* rt = 0;
   switch (getCharset(charset))
   {
      case UTF8:      rt = new XMLSelectIterator<tw::charset::UTF8>( outbuf, outbufsize, &atm->data->implementation, 0);
      case IsoLatin1: rt = new XMLSelectIterator<tw::charset::IsoLatin1>( outbuf, outbufsize, &atm->data->implementation, 0);
      case Unknown:   rt = 0;
   }
   inheritBuffer( rt, prev);
   return rt;
}

pt::Generator* Source_UTF8::createXMLHeaderIterator( char* outbuf, unsigned int outbufsize, pt::Generator* prev)
{
   protocol::Generator* rt = new XMLElementIterator<tw::charset::IsoLatin1>( outbuf, outbufsize);
   inheritBuffer( rt, prev);
   return rt;
}


