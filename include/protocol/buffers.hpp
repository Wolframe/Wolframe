#ifndef _SMERP_PROTOCOL_BUFFERS_HPP_INCLUDED
#define _SMERP_PROTOCOL_BUFFERS_HPP_INCLUDED

#include <boost/cstdint.hpp>

namespace _SMERP {
namespace protocol {

//@section protocolBuffers
//defines some buffers that can be used by 'protocol::Parser::getLine' to prefilter and 
// buffer data in the form needed for processing in the current context of the protocol.
// all buffers in this module have a minimal subset of std::string interface
// required by the 'BufferType' argument of 'getLine'

//* constant size buffer of single byte characters
template <unsigned int SIZE=128>
class Buffer
{
private:
   enum {Size=SIZE};
   unsigned int pos;
   char buf[ Size+1];
   
public:
   Buffer()                     :pos(0){};
   void init()                  {pos=0;};
   void push_back( char ch)     {if (pos<Size) buf[pos++]=ch;};
   unsigned int size() const    {return pos;};
   const char* c_str()          {buf[pos]=0; return buf;}; 
};

//buffer for the currently parsed command. 
struct CmdBuffer
{
   typedef int_least64_t ValueType;    //< stores the command name with a maximum of 10 characters (6 bit per character = case insensitive alpha or digit)
   unsigned int pos;               //< current position
   ValueType value;
   enum 
   {
      MaxCommandLen=(sizeof(ValueType)/6)
   };
   
   CmdBuffer()                     :pos(0),value(0) {};
   CmdBuffer( const CmdBuffer& o)  :pos(o.pos),value(o.value) {}; 
   void init()                     {pos=0;value=0;};
  
   //feed context with the next input character (case insensitive)
   void push_back( char ch)
   {
      if (pos >= MaxCommandLen) {init(); return;}
      if (ch >= 'a' && ch <= 'z') {value = (value << 6) | (ch-'a'); return;} 
      if (ch >= 'A' && ch <= 'Z') {value = (value << 6) | (ch-'A'); return;}
      if (ch >= '0' && ch <= '9') {value = (value << 6) | (ch+26-'0'); return;}
      init();
   };
   
   unsigned int size() const {return pos;};
   operator ValueType() const {return value;};
   ValueType operator*() const {return value;};
};


//* buffer for multi argument parsing (fixed array of null terminated byte character strings)
// beside splitting the input by blanks it parses escaping and quoted strings:
//  -escaping is done with backslash, strings can be single or double quoted
//  -the maximum number of arguments parsed is fix (16). more arguments are appended to the 16th argument
template <class Buffer>
class CArgBuffer
{
private:
   enum {Size=16};
   enum State {EndToken,Empty,Content,ContentEsc,SQContent,SQContentEsc,DQContent,DQContentEsc};
   unsigned int pos;
   unsigned int buf[ Size];
   State state;
   Buffer content;
   
   void openArg()                      {if (pos<Size) buf[pos++]=content.size();};
   
   public:
   CArgBuffer()                        :pos(0),state(Empty) {buf[0]=0;};
   void init()                         {pos=0;buf[0]=0;content.init();};
   void push_back( char ch)
   {
      switch (state)
      {
         case EndToken:
            if (ch == ' ')
            {
               content.push_back(0); 
               state = Empty;
               break;
            }
            //no break here!
            
         case Empty:
            switch (ch)
            {
               case '\'': state = SQContent; openArg(); break;
               case '\"': state = DQContent; openArg(); break;
               case '\\': state = ContentEsc; openArg(); break;
               case ' ': break;
               default: state = Content; openArg(); content.push_back(ch); break;
            }
            break;
         
         case Content:
            switch (ch)
            {
               case '\'': state = SQContent; break;
               case '\"': state = DQContent; break;
               case '\\': state = ContentEsc; break;
               case ' ':  content.push_back(0); state = Empty; break;
               default:   content.push_back(ch); break;
            }
            break;
            
         case ContentEsc:
            state = Content;
            content.push_back(ch); 
            break;
            
         case SQContent:
            switch (ch)
            {
               case '\'': state = EndToken; break;
               case '\\': state = SQContentEsc; break;
               default:   content.push_back(ch); break;
            }
            break;
            
         case SQContentEsc:
            state = SQContent;
            content.push_back(ch); 
            break;

         case DQContent:
            switch (ch)
            {
               case '\"': state = EndToken; break;
               case '\\': state = DQContentEsc; break;
               default:   content.push_back(ch); break;
            }
            break;
            
         case DQContentEsc:
            state = DQContent;
            content.push_back(ch); 
            break;
      }
   };
   unsigned int size() const                          {return pos;};
   const char* operator[]( unsigned int idx) const    {return (idx<pos)?content.c_str()+buf[idx]:0;};
};
   
} // namespace protocol
} // namespace _SMERP

#endif // _SMERP_PROTOCOL_BUFFERS_HPP_INCLUDED

