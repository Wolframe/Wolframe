//
// xmlIterator.cpp
//

#include "xmlIterator.hpp"
#include "protocol.hpp"
#include "textwolf.hpp"
#include <vector>
#include <cstring>

namespace tw = textwolf;
namespace pt = _SMERP::protocol;

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
   
   template <typename Charset_, typename Scanner_>
   struct ElementIteratorBase
   {
      typedef pt::InputBlock::const_iterator MemBlockIterator;
      typedef pt::TextIterator<MemBlockIterator> ContentIterator;
      typedef Charset_ Charset;
      typedef Scanner_ Scanner;
      typedef typename Scanner::iterator ScannerIterator;
      typedef _SMERP::xml::Source Source;
      
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

   struct Input
   {
      pt::InputBlock m_input;
      typedef pt::InputBlock::const_iterator MemBlockIterator;
      typedef pt::TextIterator< MemBlockIterator> Iterator;
      
      MemBlockIterator m_memBlockIterator;
      Iterator m_iterator;
      
      void defineInput( void* ptr, unsigned int size)
      {
         m_input.set( ptr, size);
      };

      Input()                     :m_memBlockIterator( &m_input),m_iterator( &m_memBlockIterator) {};
            
      Iterator& iterator()        {return m_iterator;};
   };

   template <typename Charset>
   struct XMLElementIterator :public _SMERP::xml::Source::const_iterator
   {
      typedef tw::XMLScanner<Input::Iterator,Charset,tw::charset::UTF8> Scanner;
      typedef ElementIteratorBase<Charset, Scanner> Base;
      typedef _SMERP::xml::Source Source;
      
      Input input;
      Scanner scanner;
      Base base;
      
      XMLElementIterator( char* outbuf, unsigned int outbufsize)
         :_SMERP::xml::Source::const_iterator( outbuf, outbufsize),
          scanner( input.iterator(), outbuf, outbufsize),
          base( outbuf, outbufsize, &scanner) {};

      virtual ~XMLElementIterator() {};
      
      virtual void skip()
      {
         try
         {
            base.m_itr++; 
            if (base.m_itr == scanner.end())
            {
               m_state = Source::EndOfInput;
            }
            else if (base.m_itr->type == Base::Scanner::ErrorOccurred)
            {
               m_state = Source::Error;
            }
            else
            {
               m_state = Source::Processing;
            }
         }
         catch (pt::InputBlock::End)
         {
            m_state = Source::EndOfBuffer;
         };
      };
      
      virtual void feed( char* block, unsigned int blocksize)
      {
         input.defineInput( block, blocksize);
      };
   };


   template <typename Charset>
   struct XMLSelectIterator :public _SMERP::xml::Source::const_iterator
   {
      typedef std::map< const char*,tw::UChar> EntityMap;
      typedef tw::XMLPathSelectAutomaton< tw::charset::UTF8> Automaton;
      typedef tw::XMLPathSelect< Input::Iterator,Charset,tw::charset::UTF8,EntityMap> Scanner;
      typedef ElementIteratorBase< Charset, Scanner> Base;
      typedef _SMERP::xml::Source Source;
      
      Input input;
      Scanner scanner;
      Base base;
      
      XMLSelectIterator( char* outbuf, unsigned int outbufsize, Automaton* automaton, EntityMap* entityMap)
         :Source::const_iterator( outbuf, outbufsize),
         scanner( automaton, input.iterator(), outbuf, outbufsize, entityMap),
         base( outbuf, outbufsize, &scanner) {};       
      
      virtual ~XMLSelectIterator() {};
      
      virtual void skip()
      {
         try
         {
            base.m_itr++; 
            switch (base.m_itr->state)
            {
               case Scanner::iterator::Element::Ok:          m_state = Source::Processing; break;
               case Scanner::iterator::Element::EndOfOutput: m_state = Source::Error; break;
               case Scanner::iterator::Element::EndOfInput:  m_state = Source::EndOfInput; break;
               case Scanner::iterator::Element::ErrorState:  m_state = Source::Error; break;
            }
         }
         catch (pt::InputBlock::End)
         {
            m_state = Source::EndOfBuffer;
         };
      };
      
      virtual void feed( char* block, unsigned int blocksize)
      {
         input.defineInput( block, blocksize);
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
using namespace _SMERP;
using namespace _SMERP::xml;

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
               elem--;
               itr++;
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

Source::const_iterator* Source::createXMLElementIterator( const char* charset, char* outbuf, unsigned int outbufsize)
{
   switch (getCharset(charset))
   {
      case UTF8: return new XMLElementIterator<tw::charset::UTF8>( outbuf, outbufsize);
      case IsoLatin1: return new XMLElementIterator<tw::charset::IsoLatin1>( outbuf, outbufsize);
      case Unknown: return 0;
   }
   return 0;
}

Source::const_iterator* Source::createXMLSelectIterator( const char* charset, char* outbuf, unsigned int outbufsize, const Automaton* atm)
{
   switch (getCharset(charset))
   {
      case UTF8: return new XMLSelectIterator<tw::charset::UTF8>( outbuf, outbufsize, &atm->data->implementation, 0);
      case IsoLatin1: return new XMLSelectIterator<tw::charset::IsoLatin1>( outbuf, outbufsize, &atm->data->implementation, 0);
      case Unknown: return 0;
   }
   return 0;
}


