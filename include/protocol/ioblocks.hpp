#ifndef _SMERP_PROTOCOL_IO_BLOCKS_HPP_INCLUDED
#define _SMERP_PROTOCOL_IO_BLOCKS_HPP_INCLUDED

namespace _SMERP {
namespace protocol {

//@section protocolIOBlocks
//defines the processed blocks for output from and input to the processor 
// that reads the input through iterators of the input blocks and prints to the
// output blocks.

//memory block for network messages
class MemBlock
{  
public:
   friend class InputBlock;
   friend class OutputBlock;
   
   MemBlock( unsigned int p_size) :m_ptr(0),m_size(p_size),m_filled(0),m_allocated(false)
   {
      m_ptr = new unsigned char[ m_size];
      m_allocated = true;
   };
   MemBlock( void* p_ptr, unsigned int p_size) :m_ptr(p_ptr),m_size(p_size),m_filled(0),m_allocated(false)
   {};
   
   ~MemBlock()
   {
      if (m_allocated) delete [] (unsigned char*)m_ptr;
   };
   
   void init()
   {
      m_filled = 0;
   };
   
   void define( void* p_ptr, unsigned int p_size)
   {
      if (m_allocated) delete [] (unsigned char*)m_ptr;
      m_ptr = p_ptr;
      m_size = p_size;
      m_filled = 0;
      m_allocated = false;
   };
   
   void* ptr() const                  {return m_ptr;};
   unsigned int size() const          {return m_size;};
   unsigned int filled() const        {return m_filled;};
   
private:
   void* m_ptr;
   unsigned int m_size;
   unsigned int m_filled;
   bool m_allocated;
   
   MemBlock( const MemBlock&) {};
   MemBlock& operator=( const MemBlock&);
};

//input memory block to iterate through with an iterator: 
//read as long as you can and throw an exception if you can't because you need more data.
//@remark iterators based on this iterator must be without read ahead. 
// it's illegal to ahead more that you consume. 
class InputBlock 
{
public:
   MemBlock mem;
   unsigned char* content() const                    {return (unsigned char*)mem.ptr();};
   unsigned int size() const                         {return mem.size();};
   unsigned int endpos() const                       {return mem.filled();};
  
public:
   InputBlock( unsigned int p_size)                  :mem(p_size) {};
   InputBlock( void* p_ptr, unsigned int p_size)     :mem(p_ptr,p_size) {};
   
   MemBlock* operator->()                            {return &mem;};
  
   void init()
   {
      mem.init();
   };
   
   void setFilled( unsigned int nn)
   {
      mem.m_filled = nn;
   };
   
   //exception thrown if there is nothing to read from the input anymore.
   //triggers reading more input from the network
   struct End {};

   //input iterator
   class iterator
   {
   public:   
      //skip to the next input character
      void skip()
      {
          if (++pos == input->endpos())
          {
            pos = 0;
            input->mem.init();
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

   private:
      InputBlock* input;
      unsigned int pos;        
   };
   iterator begin()                           {iterator rt(this); return rt;};
   iterator end()                             {return iterator();};
   
   //TODO make a real const iterator
   typedef iterator const_iterator;
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
      return mem.filled()==0;
   };
   
   //print one character to the output
   bool print( char ch)
   {
      if (mem.m_filled == mem.m_size) return false;
      ((char*)mem.m_ptr)[ mem.m_filled++] = ch;
      return true;
   };

   //forward the output buffer cursor
   //TODO rename function
   bool shift( unsigned int nn)
   {
      if (mem.m_filled+nn >= mem.m_size) return false;
      mem.m_filled += nn;
      return true;
   };
   
   //pointer to the rest of the output buffer block
   char* rest() const
   {
      return ((char*)mem.m_ptr) + mem.m_filled;
   }
   
   //size of the rest of the output buffer (how many characters can be written)
   unsigned int restsize() const
   {
      return mem.m_size - mem.m_filled;
   }
   
   //release a written memory block
   void release()
   {
      mem.m_filled = 0;
   }
};

} // namespace protocol
} // namespace _SMERP

#endif // _SMERP_PROTOCOL_IO_BLOCKS_HPP_INCLUDED

