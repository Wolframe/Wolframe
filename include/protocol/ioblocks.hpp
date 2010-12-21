#ifndef _SMERP_PROTOCOL_IO_BLOCKS_HPP_INCLUDED
#define _SMERP_PROTOCOL_IO_BLOCKS_HPP_INCLUDED

namespace _SMERP {
namespace protocol {

//@section protocolIOBlocks
//defines the processed blocks for output from and input to the processor 
// that reads the input through iterators of the input blocks and prints to the
// output blocks.

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

} // namespace protocol
} // namespace _SMERP

#endif // _SMERP_PROTOCOL_IO_BLOCKS_HPP_INCLUDED

