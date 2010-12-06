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
//@remark iterators based on this iterator must be without read ahead (believe me, one byte is too much). it's illegal to
//read over more that you can consume. you can read your EOF but detecting this by reading more that belongs to you is illegal.
//but you can buffer what ever you want if it is yours. but do not read over one single byte that you cannot consume.
//TODO make iterator more general, not only char iterator but covering arbitrary data types and binary data.
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
   
   //exception thrown if there is nothing to read from the input anymore: triggers reading more input from the network
   struct End {};

   //STL conform input iterator
   //TODO tag it correctly according STL and do not rely on the default properties (input iterator)
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


//output interface based on a memory block. print as long a you can and the order to "ship" what you printed.
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
//TODO make it really ascii and not a superset of it
//TODO simplify multi argument parsing (C argc,argv model)
//@remark end of lines or trailing spaces are not implicitely consumed, because this raises problems (like was it consumed or not).
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
      typedef unsigned long long ValueType;
      unsigned int pos;
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
   //so we can be called again and continue with context in case of an input exception
   template <typename IteratorType>
   int get( IteratorType& src, Context& ct) const
   {
      char ch;
      while ((ch=*src) > 32)
      {
         if (!next( ct, ch)) return -1;
         ++src;
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
  
   //go to the end of line and write it to buffer, starting with the first non space.
   //after call the iterator is pointing to the end of line char or at the end of content char
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
   enum {MaxNofCommands=16,MaxCommandLen=(sizeof(Context::ValueType)/6)};
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




//iterator over a content that terminates with [CR]LF dot [CR]LF 
template <class Iterator>
class TextIterator
{
private:
   Iterator* src_;
   Iterator& src() const                  {return *src_;};
   unsigned int state;
   
public:
   TextIterator()                         :state(0) {};
   TextIterator( Iterator* p_src)         :src_(p_src),state(0) {};

   //return the current character
   //  or a '\r' for the '.' after EOL
   //  or 0 if we reached the EOF state
   char cur()
   {
      char ch = *src();
      if (state < 2) return ch;
      if (state == 2) return (ch == '.')?'\r':ch;
      if (state < 5) return ch;
      return 0; /*EOF*/
   };
   
   //skip one character and adapt the EOF sequence detection state
   void skip()
   {
      if (state == 0)
      {
         char ch = *src(); 
         ++src();
         if (ch < 32)
         {
            if (ch == '\r')
            {
               state = 1;
            }
            else if (ch == '\n')
            {
               state = 2;
            }
         }
      }
      else if (state == 1)
      {
         char ch = *src();
         ++src();
         if (ch != '\n')
         {
            if (ch != '\r') state = 0;
         }
         else
         {
            state = 2;
         }
      }
      else if (state == 2)
      {
         char ch = *src();
         ++src();
         if (ch == '.')
         {
            state = 3;         
         }
         else if (ch == '\r')
         {
            state = 1;
         }
         else if (ch == '\n')
         {
            //state = 2;
         }
         else
         {
            state = 0;
         }
      }
      else if (state == 3)
      {
         char ch = *src();
         ++src();
         if (ch == '\r')
         {
            state = 4;
         } 
         else if (ch == '\n')
         {
            state = 5;
         }
         else
         {
            state = 0;
         }
      }
      else if (state == 4)
      {
         char ch = *src();
         if (ch == '\n')
         {
            state = 5;
         }
         else
         {
            state = 0;
         }
      }
      else
      {
         state = 0;
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

