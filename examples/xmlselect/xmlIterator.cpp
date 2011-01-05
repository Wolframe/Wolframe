//
// xmlIterator.cpp
//

#include "xmlIterator.hpp"
#include "protocol.hpp"
#include "textwolf.hpp"
#include <vector>

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
               Count
            };
            Type type;
            
            Element()                                                           :type(Tag) {};
            Element( Type p_type)                                               :type(p_type) {};
            Element( Type p_type, const char* p_name, unsigned int p_namesize)  :name(p_name,p_namesize),type(p_type) {};
            Element( const Element& o)                                          :name(o.name),type(o.type) {};
         };
      private:
         const char* input;
         std::vector<Element> ar;
         unsigned int pos;
         bool valid;
         
         void skipB()
         {
            const char* name = input+pos;
            unsigned int ii;
            for (ii=0; name[ii] != '\0' && name[ii]<=' '; ii++);
            if (name[ii] == '\0') throw Error();
            pos += ii;
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
         void parseString( Element::Type type, char eb)
         {
            const char* name = input+pos;
            unsigned int ii;
            for (ii=0; name[ii] != eb && name[ii] != '\0'; ii++);
            if (name[ii] == '\0') throw Error();
            ar.push_back( Element( type, name, ii));
            pos += ii+1;
            skipB();
         };
         void parseElement()
         {
            skipB();
            if (input[pos] == '@')
            {
               pos++;
               parseName( Element::AttributeName);
               if (input[pos] == '=')
               {
                  pos++;
                  if (input[pos] == '\'' || input[pos] == '\"')     
                  {
                     pos++;
                     parseString( Element::AttributeValue, input[pos-1]);
                  }
                  else
                  {
                     parseName( Element::AttributeValue);
                  }
               }
            }
            else
            {
               parseNumber( Element::Count);
            }
            if (input[pos] != ']') throw Error();
            pos++;
            skipB();
         };
         void parse()
         {
            while (input[pos] != '\0')
            {
               if (input[pos] == '/')
               {
                  pos++;
                  if (input[pos] == '/')
                  {
                     ar.push_back( Element( Element::Follow));
                  }
                  else
                  {
                     parseName( Element::Tag);
                  }
               }
               else if (input[pos] == '@')
               {
                  parseName( Element::AttributeName);
               }
               else if (input[pos] == '[')
               {
                  parseElement();
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
            
            case XPathExpression::Element::Count:
            {
               elem < atoi( itr->name.c_str());
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


