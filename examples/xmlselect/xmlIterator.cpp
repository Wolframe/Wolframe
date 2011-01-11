//
// xmlIterator.cpp
//

#include "xmlIterator.hpp"
#include "protocol.hpp"
#include "textwolf.hpp"
#include <vector>
#include <cstring>

namespace tw = textwolf;

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
   
}//anonymous namespace

using namespace _SMERP;
using namespace _SMERP::xml;

struct Automaton::Private
{
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

template <typename Charset_>
struct ElementIteratorBase
{
   typedef protocol::InputBlock::const_iterator MemBlockIterator;
   typedef protocol::TextIterator<MemBlockIterator> ContentIterator;
   typedef Charset_ Charset;
   typedef tw::XMLScanner<ContentIterator,Charset,tw::charset::UTF8> Scanner;
   typedef typename Scanner::iterator ScannerIterator;
   
   char* m_outbuf;
   unsigned int m_outbufsize;
   protocol::InputBlock input;
   MemBlockIterator memBlockIterator;
   ContentIterator contentIterator;
   Scanner scanner;
   ScannerIterator itr;
   
   ElementIteratorBase( char* outbuf, unsigned int outbufsize) 
           :m_outbuf( outbuf),
            m_outbufsize( outbufsize),
            input(0,0),
            memBlockIterator( &input),
            contentIterator( &memBlockIterator),
            scanner( contentIterator, outbuf, outbufsize),
            itr( scanner.begin()) {};
               
   void defineInput( void* ptr, unsigned int size)
   {
      input.mem.define( ptr,size);
   };
   
   Source::State next()
   {
      try
      {
         itr++; 
         if (itr == scanner.end())
         {
            return Source::EndOfInput;
         }
         else if (itr->type == Scanner::ErrorOccurred)
         {
            return Source::Error;
         }
         else
         {
            return Source::Processing;
         }
      }
      catch (protocol::InputBlock::End)
      {
         return Source::EndOfBuffer;
      };
   };
};

template <typename Charset>
struct ElementIterator
      :public Source::const_iterator, 
       public ElementIteratorBase<Charset>
{
   ElementIterator( char* outbuf, unsigned int outbufsize)
       :Source::const_iterator( outbuf, outbufsize),
        ElementIteratorBase<Charset>( outbuf, outbufsize) {};

   virtual ~ElementIterator() {};
   
   virtual void skip()
   {
      m_state = ElementIteratorBase<Charset>::next();
   };
   
   virtual void feed( char* block, unsigned int blocksize)
   {
      ElementIteratorBase<Charset>::input.mem.define( block, blocksize);
   };
};

Source::const_iterator* Source::createElementIterator( const char* charset, char* outbuf, unsigned int outbufsize)
{
   unsigned int ii;
   std::string cc;
   for (ii=0; charset[ii]; ii++)
   {
      if (charset[ii] != '-') cc.push_back( tolower(charset[ii]));
   }
   if (strstr( "utf8", cc.c_str()))
   {
      return new ElementIterator<tw::charset::UTF8>( outbuf, outbufsize);
   }
   else if (strstr( "isolatin", cc.c_str()))
   {
      return new ElementIterator<tw::charset::IsoLatin1>( outbuf, outbufsize);
   }
   return 0;
}

/* TODO
const_iterator Automaton::selectIterator( Charset charset, Automaton* atm)
{
}
*/




