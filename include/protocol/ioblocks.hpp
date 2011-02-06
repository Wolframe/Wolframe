#ifndef _SMERP_PROTOCOL_IO_BLOCKS_HPP_INCLUDED
#define _SMERP_PROTOCOL_IO_BLOCKS_HPP_INCLUDED
#include "iterators.hpp"
#include <stdexcept>

namespace _SMERP {
namespace protocol {

//@section protocolIOBlocks
//defines the processed blocks for output from and input to the processor 
//  that reads the input through iterators of the input blocks and prints via the output blocks.


//memory block for network messages
class MemBlock
{  
public:
   MemBlock();
   MemBlock( unsigned int p_size);
   MemBlock( void* p_ptr, unsigned int p_size);
   MemBlock( const MemBlock& o);
   ~MemBlock();

   MemBlock& operator=( const MemBlock& o);

   void setPos( unsigned int p_pos=0)            {m_pos = p_pos;};
   void set( void* p_ptr, unsigned int p_size);
   void* ptr()                                   {return m_ptr;};
   const void* ptr() const                       {return m_ptr;};
   char* charptr()                               {return (char*)m_ptr;};
   const char* charptr() const                   {return (const char*)m_ptr;};
   unsigned int size() const                     {return m_size;};
   unsigned int pos() const                      {return m_pos;};
   
   //access violation exceptions
   struct ArrayBoundReadError                    :public std::logic_error {ArrayBoundReadError():std::logic_error("ABR"){};};
   struct ArrayBoundWriteError                   :public std::logic_error {ArrayBoundWriteError():std::logic_error("ABR"){};};
   
   //element typedefs
   typedef char value_type;
   typedef unsigned int size_type;
   
   //random access operators
   char operator[]( size_type idx) const         {if (idx>=m_pos) throw ArrayBoundReadError(); return charptr()[idx];};
   char& operator[]( size_type idx)              {if (idx>=m_pos) throw ArrayBoundWriteError(); return charptr()[idx];};
  
private:
   void* m_ptr;
   unsigned int m_size;
   unsigned int m_pos;
   bool m_allocated;
};




//input memory block to iterate through
//  read as long as you can and throw an exception if you can't because you need more data.
class InputBlock  :public MemBlock
{
public:
   struct EoD
   {
      enum State {SRC,LF,LF_DOT,LF_DOT_CR,LF_DOT_CR_LF};
   };
   
   InputBlock()                                         :m_eodState(EoD::SRC){};
   InputBlock( unsigned int p_size)                     :MemBlock(p_size),m_eodState(EoD::SRC){};
   InputBlock( void* p_ptr, unsigned int p_size)        :MemBlock(p_ptr,p_size),m_eodState(EoD::SRC){};
   InputBlock( const InputBlock& o)                     :MemBlock(o),m_eodState(o.m_eodState){};

   //random access iterators
   typedef array::iterator_t<const InputBlock,size_type,char,char,const char*> const_iterator;
   typedef array::iterator_t<InputBlock,size_type,char,char&,char*> iterator;
   
   const_iterator begin() const                         {const_iterator rt(this); return rt;};
   iterator begin()                                     {iterator rt(this); return rt;};
   const_iterator at( unsigned int pos_) const          {const_iterator rt(this); return rt+pos_;};
   iterator at( unsigned int pos_)                      {iterator rt(this); return rt+pos_;};
   const_iterator end() const                           {return const_iterator(this)+size();};
   iterator end()                                       {return iterator(this)+size();};
   
   //end of data calculation and markup
   const_iterator getEoD( const_iterator start);
   void resetEoD()                                      {m_eodState=EoD::SRC;};

private:
   EoD::State m_eodState;
};




//output interface based on a memory block. 
// print as buffer is available and then order to "ship" what you printed.
class OutputBlock :public MemBlock
{
public:
   OutputBlock( unsigned int p_size)                    :MemBlock(p_size) {};
   OutputBlock( void* p_ptr, unsigned int p_size)       :MemBlock(p_ptr,p_size) {};
   OutputBlock( const OutputBlock& o)                   :MemBlock(o) {};
   
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
   const char* rest() const                            {return charptr() + pos();};
   char* rest()                                        {return charptr() + pos();};

   //size of the rest of the output buffer (how many characters can be written)
   unsigned int restsize() const                       {return size()-pos();};

   //release a written memory block
   void release()                                      {setPos(0);};
};

} // namespace protocol
} // namespace _SMERP

#endif // _SMERP_PROTOCOL_IO_BLOCKS_HPP_INCLUDED

