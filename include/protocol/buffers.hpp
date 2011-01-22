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
   unsigned int m_pos;
   char m_buf[ Size+1];
   
public:
   Buffer()                     :m_pos(0){};
   void init()                  {m_pos=0;};
   void push_back( char ch)     {if (m_pos<Size) m_buf[m_pos++]=ch;};
   void append( const char* cc) {unsigned int ii=0; while(m_pos<Size && cc[ii]) m_buf[m_pos++]=cc[ii++];};
   unsigned int size() const    {return m_pos;};
   const char* c_str()          {m_buf[m_pos]=0; return m_buf;}; 
};

//buffer for the currently parsed command. 
struct CmdBuffer
{
   typedef boost::int_least64_t ValueType;    //< stores the command name with a maximum of 10 characters (6 bit per character = case insensitive alpha or digit)
   unsigned int m_pos;                        //< current position
   ValueType m_value;
   enum 
   {
      MaxCommandLen=(sizeof(ValueType)/6)
   };
   
   CmdBuffer()                     :m_pos(0),m_value(0) {};
   CmdBuffer( const CmdBuffer& o)  :m_pos(o.m_pos),m_value(o.m_value) {}; 
   void init()                     {m_pos=0;m_value=0;};
  
   //feed context with the next input character (case insensitive)
   void push_back( char ch)
   {
      if (m_pos >= MaxCommandLen) {init(); return;}
      if (ch >= 'a' && ch <= 'z') {m_value = (m_value << 6) | (ch-'a'); return;} 
      if (ch >= 'A' && ch <= 'Z') {m_value = (m_value << 6) | (ch-'A'); return;}
      if (ch >= '0' && ch <= '9') {m_value = (m_value << 6) | (ch+26-'0'); return;}
      init();
   };
   
   unsigned int size() const {return m_pos;};
   operator ValueType() const {return m_value;};
   ValueType operator*() const {return m_value;};
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
   unsigned int m_pos;
   unsigned int m_buf[ Size];
   const char* m_sbuf[ Size];
   State m_state;
   Buffer* m_content;
   
   void openArg()                      {if (m_pos<Size) m_buf[m_pos++]=m_content->size();};
   
public:
   CArgBuffer( Buffer* c)              :m_content(c),m_pos(0),m_state(Empty) {m_buf[0]=0;m_sbuf[0]=0;};
   void init()                         {m_pos=0;m_buf[0]=0;m_sbuf[0]=0;m_content->init();};
   void push_back( char ch)
   {
      switch (m_state)
      {
         case EndToken:
            switch (ch)
            {
               case '\'': m_state = SQContent; break;
               case '\"': m_state = DQContent; break;
               case '\\': m_state = ContentEsc; break;
               case ' ': m_content->push_back(0); m_state = Empty; break;
               default: m_state = Content; m_content->push_back(ch); break;
            }
            break;
            
         case Empty:
            switch (ch)
            {
               case '\'': m_state = SQContent; openArg(); break;
               case '\"': m_state = DQContent; openArg(); break;
               case '\\': m_state = ContentEsc; openArg(); break;
               case ' ': break;
               default: m_state = Content; openArg(); m_content->push_back(ch); break;
            }
            break;
         
         case Content:
            switch (ch)
            {
               case '\'': m_state = SQContent; break;
               case '\"': m_state = DQContent; break;
               case '\\': m_state = ContentEsc; break;
               case ' ':  m_content->push_back(0); m_state = Empty; break;
               default:   m_content->push_back(ch); break;
            }
            break;
            
         case ContentEsc:
            m_state = Content;
            m_content->push_back(ch); 
            break;
            
         case SQContent:
            switch (ch)
            {
               case '\'': m_state = EndToken; break;
               case '\\': m_state = SQContentEsc; break;
               default:   m_content->push_back(ch); break;
            }
            break;
            
         case SQContentEsc:
            m_state = SQContent;
            m_content->push_back(ch); 
            break;

         case DQContent:
            switch (ch)
            {
               case '\"': m_state = EndToken; break;
               case '\\': m_state = DQContentEsc; break;
               default:   m_content->push_back(ch); break;
            }
            break;
            
         case DQContentEsc:
            m_state = DQContent;
            m_content->push_back(ch); 
            break;
      }
   };
   unsigned int size() const                          {return m_pos;};
   const char* operator[]( unsigned int idx) const    {return (idx<m_pos)?m_content->c_str()+m_buf[idx]:0;};
   
   //return an array of 0-terminated strings as convenient in C for program arguments
   //@remark the return value of this function may not be valid anymore after a new call of push_back
   const char** argv()
   {
      if (m_sbuf[0]==0)
      {
         unsigned int ii;
         for (ii=0; ii<m_pos; ii++) m_sbuf[ii]=m_content->c_str()+m_buf[ii]; 
         if (ii<m_pos) m_sbuf[ii]=0;
      }
      return m_sbuf;
   };
   unsigned int argc() const                          {return m_pos;};
};
   
} // namespace protocol
} // namespace _SMERP

#endif // _SMERP_PROTOCOL_BUFFERS_HPP_INCLUDED

