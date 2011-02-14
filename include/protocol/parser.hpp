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
   static bool skipSpaces( IteratorType& src, IteratorType& end)
   {
      while (src < end && (*src == ' ' || *src == '\t')) ++src;
      return (src<end);
   }

   template <typename IteratorType>
   static bool skipLine( IteratorType& src, IteratorType& end)
   {
      while (src < end && *src != '\r' && *src != '\n') ++src;
      return (src<end);
   }

   //consume the end of line
   template <typename IteratorType>
   static bool consumeEOLN( IteratorType& src, IteratorType& end)
   {
      if (src == end) return false;
      if (*src == '\r') src++;
      if (src == end) return false;
      if (*src != '\n') return false;
      src++;
      return true;
   }
   
   template <typename IteratorType>
   static bool isEOLN( IteratorType& src)
   {
      return (*src == '\r' || *src == '\n');
   }

   //* go to the end of line and write it to buffer, starting with the first non space.
   // after call the iterator is pointing to the end of line char or at the EOF char.
   //@remark end of lines or trailing spaces are not implicitely consumed, 
   // because this leads to intermediate states that cannot be handled
   //@remark control characters are mapped to space (ascii 32) before writing them to 'buf'
   //@param src input iterator
   //@param buf output buffer with a minimal subset of std::string interface
   //@return true, if the line was consumed completely
   template <typename IteratorType, typename BufferType>
   static bool getLine( IteratorType& src, IteratorType& end, BufferType& buf)
   {
      if (buf.size() == 0) if (!skipSpaces( src)) return false;
      
      while (src < end && *src != '\n' && *src != '\r')
      {
         if (*src <= ' ') {buf.push_back(' '); ++src; continue;}
         buf.push_back(*src);
         ++src;
      }
      return (src < end);
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

   void init()
   {
      size = 0;
   };

   //add a command to the protocol parser (case insensitive)
   void add( const char* cmd)
   {
      CmdBufferType ct;
      for (unsigned int ii=0; cmd[ii]; ii++) ct.push_back(cmd[ii]);
      if (size == MaxNofCommands) throw Bad();
      elem[size++] = *ct; 
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
   //@return -1, if the command could not be parsed
   template <typename IteratorType>
   int getCommand( IteratorType& src, IteratorType& end, CmdBufferType& buf) const
   {
      while (src < end && *src > 32)
      {
         buf.push_back( *src);
         ++src;
      }
      if (src == end) return -1;

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
};
   
} // namespace protocol
} // namespace _SMERP

#endif // _SMERP_PROTOCOL_PARSER_HPP_INCLUDED

   
