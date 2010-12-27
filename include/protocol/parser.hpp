#ifndef _SMERP_PROTOCOL_PARSER_HPP_INCLUDED
#define _SMERP_PROTOCOL_PARSER_HPP_INCLUDED
#include <exception>

namespace _SMERP {
namespace protocol {

//helper functions for protocol parsing
class Parser
{
public:
   //go to the next non space
   template <typename IteratorType>
   static void skipSpaces( IteratorType& src)
   {
      char ch;
      while ((ch=*src) == ' ' || ch == '\t') 
      {
         ++src;
      }
   }
   
   template <typename IteratorType>
   static bool isEOLN( IteratorType& src)
   {
      return (*src == '\n' || *src == '\r');
   }
   
   template <typename IteratorType>
   static void consumeEOLN( IteratorType& src)
   {
      if (*src == '\r') ++src;
      if (*src == '\n') ++src;
   }
   
   //* go to the end of line and write it to buffer, starting with the first non space.
   // after call the iterator is pointing to the end of line char or at the EOF char.
   //@remark end of lines or trailing spaces are not implicitely consumed, 
   // because this leads to intermediate states that cannot be handled
   //@remark control characters are mapped to space (ascii 32) before writing them to 'buf'
   //@param src input iterator
   //@param buf output buffer with a minimal subset of std::string interface
   template <typename IteratorType, typename BufferType>
   static void getLine( IteratorType& src, BufferType& buf)
   {
      char ch;
      if (buf.size() == 0) skipSpaces( src);
      
      while ((ch=*src) != '\n')
      {
         if (ch == '\r') {++src; continue;}
         if (ch <= ' ') {buf.push_back(' '); ++src; continue;}
         buf.push_back(ch);
         ++src;
      }
   }   
};

//parser for ascii protocol commands
template <typename CmdBufferType>
class CmdParser :public Parser
{      
public:
   //exception for illegal parser definition (too many commands). 
   struct Bad :public std::exception {Bad() {};};
   
   CmdParser() :size(0){};
   ~CmdParser() {};

   CmdParser( const CmdParser& o) :size(o.size)
   {
      for (unsigned int ii=0; ii<size; ii++) elem[ii]=o.elem[ii];
   };

   CmdParser& operator=( const CmdParser& o)
   {
      size = o.size;
      for (unsigned int ii=0; ii<size; ii++) elem[ii]=o.elem[ii];
      return *this;
   };
   
   //define a parser with a null terminated array of protocol parser commands
   CmdParser( const char** cmd)
   {
      for (unsigned int ii=0; cmd[ii]; ii++) add(cmd[ii]);
   };
   
   //define a command
   CmdParser& operator[]( const char* cmd)
   {
      add( cmd);
      return *this;
   };
   
   //parse the next command.
   template <typename IteratorType>
   int getCommand( IteratorType& src, CmdBufferType& buf) const
   {
      char ch;
      while ((ch=*src) > 32)
      {
         buf.push_back( ch);
         ++src;
      }
      if (*buf == -1)
      {
         buf.init();
         return -1;
      }
      for (unsigned int ii=0; ii<size; ii++)
      {
         if (elem[ii] == *buf)
         {
            buf.init();
            return (int)ii;
         }
      }
      buf.init();
      return -1;
   }
  
private:
   enum {MaxNofCommands=32};
   typedef typename CmdBufferType::ValueType CmdValueType;
   CmdValueType elem[ MaxNofCommands];
   unsigned int size;

   //add a command to the protocol parser (case insensitive)
   void add( const char* cmd)
   {
      CmdBufferType ct;
      for (unsigned int ii=0; cmd[ii]; ii++) ct.push_back(cmd[ii]);
      if (size == MaxNofCommands) throw Bad();
      elem[size++] = *ct; 
   };
};
   
} // namespace protocol
} // namespace _SMERP

#endif // _SMERP_PROTOCOL_PARSER_HPP_INCLUDED

