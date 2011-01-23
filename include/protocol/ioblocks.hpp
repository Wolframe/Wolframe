#ifndef _SMERP_PROTOCOL_IO_BLOCKS_HPP_INCLUDED
#define _SMERP_PROTOCOL_IO_BLOCKS_HPP_INCLUDED
#include <cstring>

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
   MemBlock()                                    :m_ptr(0),m_size(0),m_pos(0),m_allocated(false) {};
   MemBlock( unsigned int p_size)                :m_ptr(0),m_size(p_size),m_pos(0),m_allocated(false)
   {
      m_ptr = new unsigned char[ m_size];
      m_allocated = true;
   };
   MemBlock( void* p_ptr, unsigned int p_size)   :m_ptr(p_ptr),m_size(p_size),m_pos(0),m_allocated(false){};

   MemBlock( const MemBlock& o)                  :m_ptr(0),m_size(0),m_pos(0),m_allocated(false) {*this = o;};
   
   MemBlock& operator=( const MemBlock& o)
   {
      if (m_allocated) delete [] (unsigned char*)m_ptr;
      m_size = o.m_size;
      m_pos = o.m_pos;
      m_allocated = o.m_allocated;

      if (o.m_allocated)
      {
         m_ptr = new unsigned char[ m_size];
         memcpy( m_ptr, o.m_ptr, m_size); 
      }
      else
      {
         m_ptr = o.m_ptr;
      }
      return *this;
   };
   
   ~MemBlock()
   {
      if (m_allocated) delete [] (unsigned char*)m_ptr;
   };
   
   void setPos( unsigned int p_pos=0)
   {
      m_pos = p_pos;
   };
   
   void set( void* p_ptr, unsigned int p_size)
   {
      if (m_allocated) delete [] (unsigned char*)m_ptr;
      m_ptr = p_ptr;
      m_size = p_size;
      m_pos = 0;
      m_allocated = false;
   };
   
   void* ptr() const                  {return m_ptr;};
   char* charptr() const              {return (char*)m_ptr;};
   unsigned int size() const          {return m_size;};
   unsigned int pos() const           {return m_pos;};
   
private:
   void* m_ptr;
   unsigned int m_size;
   unsigned int m_pos;
   bool m_allocated;
};

//input memory block to iterate through with an iterator: 
//read as long as you can and throw an exception if you can't because you need more data.
//@remark iterators based on this iterator must be without read ahead. 
// it's illegal to ahead more that you consume. 
class InputBlock :public MemBlock 
{
public:
   InputBlock()                                      {};
   InputBlock( unsigned int p_size)                  :MemBlock(p_size) {};
   InputBlock( void* p_ptr, unsigned int p_size)     :MemBlock(p_ptr,p_size) {};
   InputBlock( const InputBlock& o)                  :MemBlock(o) {};
   
   //exception thrown if there is nothing to read from the input anymore.
   //triggers reading more input from the network
   struct End {};

   //input iterator
   class const_iterator
   {
   public:   
      //skip to the next input character
      void skip()
      {
          if (++m_pos == m_input->pos())
          {
            m_pos = 0;
            throw End();
          }
      };

      //get the current input character
      char cur()
      {         
          if (m_pos == m_input->pos()) throw End();
          return m_input->charptr()[ m_pos];
      };

      const_iterator( InputBlock* input)                    :m_input(input),m_pos(0) {};
      const_iterator()                                      :m_input(0),m_pos(0) {};
      const_iterator( const const_iterator& o)              :m_input(o.m_input),m_pos(o.m_pos) {};
      const_iterator& operator=( const const_iterator& o)   {m_input=o.m_input; return *this;}

      const_iterator& operator++()                          {skip(); return *this;};
      const_iterator operator++(int)                        {const_iterator tmp(*this); skip(); return tmp;};
      char operator*()                                      {return cur();};

      unsigned int getPos() const                           {return m_pos;};
   private:
      friend class InputBlock;
      InputBlock* m_input;
      unsigned int m_pos;        
   };

   const_iterator begin()                                   {const_iterator rt(this); return rt;};
   const_iterator at( unsigned int pos_)                    {const_iterator rt(this); rt.m_pos=(pos_ <= pos())?pos_:pos(); return rt;};
   const_iterator end()                                     {return const_iterator();};
};

//output interface based on a memory block. 
// print as buffer is available and then order to "ship" what you printed.
class OutputBlock :public MemBlock
{
public:
   OutputBlock( unsigned int p_size)                        :MemBlock(p_size) {};
   OutputBlock( void* p_ptr, unsigned int p_size)           :MemBlock(p_ptr,p_size) {};
   OutputBlock( const OutputBlock& o)                  :MemBlock(o) {};
   
   //return true if the buffer is empty
   bool empty() const
   {
      return (pos()==0);
   };
   
   //print one character to the output
   bool print( char ch)
   {
      if (pos() == size()) return false;
      charptr()[ pos()] = ch;
      setPos( pos() + 1);
      return true;
   };

   //forward the output buffer cursor
   bool incPos( unsigned int nn)
   {
      if (pos()+nn >= size()) return false;
      setPos( pos() + nn);
      return true;
   };
   
   //pointer to the rest of the output buffer block
   char* rest() const
   {
      return charptr() + pos();
   }
   
   //size of the rest of the output buffer (how many characters can be written)
   unsigned int restsize() const
   {
      return size() - pos();
   }
   
   //release a written memory block
   void release()
   {
      setPos(0);
   }
};

} // namespace protocol
} // namespace _SMERP

#endif // _SMERP_PROTOCOL_IO_BLOCKS_HPP_INCLUDED

