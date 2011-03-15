#ifndef _Wolframe_PROTOCOL_GENERATOR_INTERFACE_HPP_INCLUDED
#define _Wolframe_PROTOCOL_GENERATOR_INTERFACE_HPP_INCLUDED

namespace _Wolframe {
namespace protocol {

//@section protocolGeneratorInterface

//interface for (non STL conform) iterator over input content elements
struct Generator
{
   enum State
   {
      Open,           //serving data
      EndOfMessage,   //EWOULDBLK -> have to yield
      Error           //an error occurred
   };

   enum ElementType
   {
      OpenTag,         //Open new hierarchy level
      Attribute,       //Attribute Name
      Value,           //Content or attribute Value
      CloseTag         //Close current hierarchy level
   };

   //Get next element call
   typedef bool (*GetNext)( Generator* this_, ElementType* type, void* buffer, unsigned int buffersize, unsigned int* bufferpos);

   bool getNext( ElementType* type, void* buffer, unsigned int buffersize, unsigned int* bufferpos)
   {
      return m_getNext( this, type, buffer, buffersize, bufferpos);
   }

   State state() const
   {
      return m_state;
   }

   void protocolInput( void* data, unsigned int datasize, bool eoD)
   {
      m_gotEoD = eoD;
      m_ptr = data;
      m_size = datasize;
      m_pos = 0;
   }

   Generator& operator = (const Generator& o)
   {
      m_ptr = o.m_ptr;
      m_pos = o.m_pos;
      m_size = o.m_size;
      m_gotEoD = o.m_gotEoD;
      m_state = o.m_state;
      m_errorCode = o.m_errorCode;
      return *this;
   }

   Generator( const GetNext& gn) :m_ptr(0),m_pos(0),m_size(0),m_gotEoD(false),m_state(Open),m_errorCode(0),m_getNext(gn){}

   int getError() const              {return m_errorCode;}
   bool gotEoD() const               {return m_gotEoD;}
   void* ptr() const                 {return(void*)((char*)m_ptr+m_pos);}
   unsigned int size() const         {return (m_pos<m_size)?(m_size-m_pos):0;}
   void skip( unsigned int n)        {if ((m_pos+n)>=m_size) m_pos=m_size; else m_pos+=n;}
   void setState( State s, int e=0)  {m_state=s;m_errorCode=e;}

private:
   void* m_ptr;
   unsigned int m_pos;
   unsigned int m_size;
   bool m_gotEoD;
   State m_state;
   int m_errorCode;
   GetNext m_getNext;
};


struct FormatOutput
{
   //Print next element call
   typedef bool (*Print)( FormatOutput* this_, int type, void* element, unsigned int elementsize);

   FormatOutput( const Print& op) :m_ptr(0),m_pos(0),m_size(0),m_print(op){}

   FormatOutput& operator = (const FormatOutput& o)
   {
      m_ptr = o.m_ptr;
      m_pos = o.m_pos;
      m_size = o.m_size;
      return *this;
   }

   void init( void* data, unsigned int datasize)
   {
      m_ptr = data;
      m_size = datasize;
      m_pos = 0;
   }

   void init()
   {
      init( 0, 0);
   }

   void* cur() const                 {return (void*)((char*)m_ptr+m_pos);}
   unsigned int restsize() const     {return (m_pos<m_size)?(m_size-m_pos):0;}
   unsigned int pos() const          {return m_pos;}
   unsigned int size() const         {return m_size;}
   void* ptr() const                 {return m_ptr;}
   void incr( unsigned int n)        {if ((m_pos+n)>=m_size) m_pos=m_size; else m_pos+=n;}

   bool print( int type, void* element, unsigned int elementsize)
   {
      return m_print( this, type, element, elementsize);
   }

private:
   void* m_ptr;
   unsigned int m_pos;
   unsigned int m_size;
   Print m_print;
};
}}//namespace
#endif



