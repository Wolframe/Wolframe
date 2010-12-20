//
// protocol helper
//

#ifndef _SMERP_PROTOCOL_HPP_INCLUDED
#define _SMERP_PROTOCOL_HPP_INCLUDED
#include <string.h>
#include <string>
#include <iostream>

namespace _SMERP {
namespace protocol {
  
//memory block for network messages
struct MemBlock
{
   void* ptr;
   unsigned int size;
   unsigned int filled;
   bool lastblock;
  
   void setEOF() {lastblock=true;};
   
   MemBlock( unsigned int p_size) :ptr(0),size(p_size),filled(0),lastblock(false)
   {
      ptr = (unsigned char*)new unsigned char[ size];
   };
   ~MemBlock()
   {
      delete [] (unsigned char*)ptr;
   };
   void init()
   {
      lastblock = false;
      filled = 0;
   };
    
private:
   MemBlock( const MemBlock&) {};
   MemBlock& operator=( const MemBlock&);
};


//input memory block to iterate through with an iterator: 
//read as long as you can and throw an exception if you can't because you need more data.
//@remark iterators based on this iterator must be without read ahead. 
// it's illegal to ahead more that you consume. 
class InputBlock 
{
private:
   MemBlock mem;
public:
   unsigned char* content() const       {return (unsigned char*)mem.ptr;};
   unsigned int size() const            {return mem.size;};
   unsigned int endpos() const          {return mem.filled;};
  
public:
   InputBlock( unsigned int p_size)     :mem(p_size) {};

   MemBlock* operator->()               {return &mem;};
  
   void init()
   {
      mem.init();
   };
   
   //exception thrown if there is nothing to read from the input anymore.
   //triggers reading more input from the network
   struct End {};

   //input iterator
   struct iterator
   {
      InputBlock* input;
      unsigned int pos;
      
      //skip to the next input character
      void skip()
      {
          if (++pos == input->endpos())
          {
            pos = 0;
            input->mem.filled = 0;
            throw End();
          }
      };
      
      //get the current input character
      char cur()
      {         
          if (pos == input->endpos()) throw End();
          return input->content()[ pos];
      };
      
      iterator( InputBlock* p_input)          :input(p_input),pos(0) {};
      iterator()                              :input(0),pos(0) {};
      iterator( const iterator& o)            :input(o.input),pos(o.pos) {};
      iterator& operator=( const iterator& o) {input=o.input; return *this;}
      
      iterator& operator++()                  {skip(); return *this;};
      iterator operator++(int)                {iterator tmp(*this); skip(); return tmp;};
      char operator*()                        {return cur();};
   };
   iterator begin()                           {iterator rt(this); return rt;};
   iterator end()                             {return iterator();};
};


//output interface based on a memory block. 
// print as buffer is available and then order to "ship" what you printed.
class OutputBlock
{
private:
   MemBlock mem;
  
public:
   OutputBlock( unsigned int p_size)     :mem(p_size) {};

   MemBlock* operator->()                {return &mem;};

   void init()
   {
      mem.init();
   };

   //return true if the buffer is empty
   bool empty() const
   {
      return mem.filled==0;
   };
   
   //print one character to the output
   bool print( char ch)
   {
      if (mem.filled == mem.size) return false;
      ((char*)mem.ptr)[ mem.filled++] = ch;
      return true;
   };

   //forward the output buffer cursor
   bool shift( unsigned int nn)
   {
      if (mem.filled+nn >= mem.size) return false;
      mem.filled += nn;
      return true;
   };
   
   //pointer to the rest of the output buffer block
   char* rest() const
   {
      return ((char*)mem.ptr) + mem.filled;
   }
   
   //size of the rest of the output buffer (how many characters can be written)
   unsigned int restsize() const
   {
      return mem.size - mem.filled;
   }
   
   //release a written memory block
   void release()
   {
      mem.filled = 0;
   }
};


//case insensitive ascii protocol parser
//@remark end of lines or trailing spaces are not implicitely consumed, 
// because this leads to intermediate states that cannot be handled
class Parser
{      
public:
   //exception for illegal parser definition (too many commands). thrown only in definition (hopefully static) not run time!
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

   //define a protocol parser command
   Parser& operator[]( const char* cmd)
   {
      add( cmd);
      return *this;
   };
  
   //get the next command of the parser. buffer the visited command start in context.
   // so it can be called again and continue with context in case of an missing input
   // exception
   template <typename IteratorType>
   int get( IteratorType& src, Context& ct) const
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

   //constant size buffer for printing messages and reading the 
   // arguments of a protocol command with a subset of std::string interface
   // that implements a minumum implemnting the 'getLine'::BufferType interface
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

   //buffer for multi argument parsing that can be used for getLine
   // escaping is done with backslash, strings can be single or double quoted
   // the maximum number of arguments parsed is fix (16).
   // the template argument is a Buffer as used for getLine (subset of std::string).
   template <class Buffer>
   class CargBuffer
   {
   private:
      enum {Size=16};
      enum State {Empty,Content,ContentEsc,SQContent,SQContentEsc,DQContent,DQContentEsc};
      unsigned int pos;
      unsigned int buf[ Size];
      State state;
      Buffer content;
      void openArg()                      {if (pos<Size) buf[pos++]=content.size();};
      
    public:
      CargBuffer()                        :pos(0){buf[0]=0;};
      void init()                         {pos=0;buf[0]=0;content.init();};
      void push_back( char ch)
      {
         switch (state)
         {
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
                  case '\'': state = SQContent; openArg(); break;
                  case '\"': state = DQContent; openArg(); break;
                  case '\\': state = ContentEsc; break;
                  case ' ':  state = Empty; break;
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
                  case '\'': state = Empty; break;
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
                  case '\"': state = Empty; break;
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

   //go to the end of line and write it to buffer, starting with the first non space.
   // after call the iterator is pointing to the end of line char or at the end of 
   // content char
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




//iterator over a content that returns 0 (end of data) with [CR]LF dot(".") [CR]LF reached
//when 0 is returned the iterator for the protocol can take over again
template <class Iterator>
class TextIterator
{
private:
   Iterator* src_;
   Iterator& src() const                  {return *src_;};
   enum State {SRC,CR,CR_LF,CR_LF_DOT,CR_LF_DOT_CR,CR_LF_DOT_CR_LF};
   State state;
   unsigned int nn;
   
public:
   TextIterator()                         :state(SRC),nn(0) {};
   TextIterator( Iterator* p_src)         :src_(p_src),state(SRC),nn(0) {};

   //return the current character
   //  or a '\r' for the '.' after EOL
   //  or 0 if we reached end of data marked by [CR]LF dot(".") [CR]LF
   char cur()
   {
      char ch = *src();
      if (state < CR_LF) return ch;
      if (state == CR_LF) return (ch == '.')?'\r':ch;
      if (state < CR_LF_DOT_CR_LF) return ch;
      return 0; /*EOF*/
   };
   
   //skip one character and adapt the EOF sequence detection state
   // its a cascaded if instead of a switch because the states are ordered 
   // ascending according their probability
   void skip()
   {
      if (state == SRC)
      {
         char ch = *src(); 
         if (ch < 32)
         {
            if (ch == '\r')
            {
               state = CR;
            }
            else if (ch == '\n')
            {
               state = CR_LF;
            }
         }
         ++src();++nn;
      }
      else if (state == CR)
      {
         char ch = *src();
         if (ch == '\n')
         {
            state = CR_LF;
         }
         else if (ch == '\r')
         {
            //state = CR;
         }
         else
         {
            state = SRC;
         }
         ++src();++nn;
      }
      else if (state == CR_LF)
      {
         char ch = *src();
         if (ch == '.')
         {
            state = CR_LF_DOT;         
         }
         else if (ch == '\r')
         {
            state = CR;
         }
         else if (ch == '\n')
         {
            //state = CR_LF;
         }
         else
         {
            state = SRC;
         }
         ++src();++nn;
      }
      else if (state == CR_LF_DOT)
      {
         char ch = *src();
         if (ch == '\r')
         {
            state = CR_LF_DOT_CR;
         } 
         else if (ch == '\n')
         {
            state = CR_LF_DOT_CR_LF;
         }
         else
         {
            state = SRC;
         }
         ++src();++nn;
      }
      else if (state == CR_LF_DOT_CR)
      {
         char ch = *src();
         if (ch == '\n')
         {
            state = CR_LF_DOT_CR_LF;
         }
         else
         {
            state = SRC;
         }
      }
      else
      {
         state = SRC;
      }
   };

   TextIterator& operator++()                                  {skip(); return *this;};
   TextIterator operator++(int)                                {TextIterator tmp(*this); skip(); return tmp;};
   char operator*()                                            {return cur();};
   TextIterator& operator=( Iterator* p_src)                   {src_=p_src; return *this;};
};
   
} // namespace protocol
} // namespace _SMERP

#endif // _SMERP_PROTOCOL_HPP_INCLUDED

