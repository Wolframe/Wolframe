#ifndef _SMERP_PROTOCOL_PARSER_HPP_INCLUDED
#define _SMERP_PROTOCOL_PARSER_HPP_INCLUDED

namespace _SMERP {
namespace protocol {

//parser for ascii protocol commands with helper functions to parse the command arguments
class Parser
{      
public:
   //exception for illegal parser definition (too many commands). 
   struct Bad {};
   
   Parser() :size(0){};
   ~Parser() {};

   Parser( const Parser& o) :size(o.size)
   {
      for (unsigned int ii=0; ii<size; ii++) elem[ii]=o.elem[ii];
   };

   Parser& operator=( const Parser& o)
   {
      size = o.size;
      for (unsigned int ii=0; ii<size; ii++) elem[ii]=o.elem[ii];
      return *this;
   };
   
   //define a parser with a null terminated array of protocol parser commands
   Parser( const char** cmd)
   {
      for (unsigned int ii=0; cmd[ii]; ii++) add(cmd[ii]);
   };
   
   //define a command
   Parser& operator[]( const char* cmd)
   {
      add( cmd);
      return *this;
   };
   
   //context of the currently parsed command. like "ech" of the "echo" command if the whole command could 
   //   not be read because the network message ended in the middle.
   //remind to keep this as part of the parser state variables and NOT as local (!)
   struct Context
   {
      typedef unsigned long long ValueType; //< stores the command name with a maximum of 10 characters (6 bit per character = case insensitive alpha or digit)
      unsigned int pos;                     //< current position
      ValueType value;
      
      Context()                   :pos(0),value(0) {};
      Context( const Context& o)  :pos(o.pos),value(o.value) {}; 
      void init()                 {pos=0;value=0;};
   };

   //get the next command of the parser. buffer the visited command start in context.
   // so it can be called again and continue with context in case of an missing input
   // exception
   template <typename IteratorType>
   int getCommand( IteratorType& src, Context& ct) const
   {
      char ch;
      while ((ch=*src) > 32)
      {
         ++src;
         if (!next( ct, ch)) return -1;
      }

      Context::ValueType vt = ct.value;
      ct.init();
      
      for (unsigned int ii=0; ii<size; ii++)
      {
         if (elem[ii] == vt) return (int)ii;
      }
      return -1;
   }
  
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
  
private:
   enum {MaxNofCommands=32,MaxCommandLen=(sizeof(Context::ValueType)/6)};
   Context::ValueType elem[ MaxNofCommands];
   unsigned int size;

   //feed context with the next input character (case insensitive)
   bool next( Context& ct, char ch) const
   {
      if (ct.pos >= MaxCommandLen) {ct.init(); return false;}
      if (ch >= 'a' && ch <= 'z') {ct.value = (ct.value << 6) | (ch-'a'); return true;} 
      if (ch >= 'A' && ch <= 'Z') {ct.value = (ct.value << 6) | (ch-'A'); return true;}
      if (ch >= '0' && ch <= '9') {ct.value = (ct.value << 6) | (ch+26-'0'); return true;}
      ct.init();
      return false;
   };

   //add a command to the protocol parser (case insensitive)
   void add( const char* cmd)
   {
      Context ct;
      for (unsigned int ii=0; cmd[ii] && next(ct,cmd[ii]); ii++);
      if (size == MaxNofCommands) throw Bad();
      elem[size++] = ct.value; 
   };
};
   
} // namespace protocol
} // namespace _SMERP

#endif // _SMERP_PROTOCOL_PARSER_HPP_INCLUDED

