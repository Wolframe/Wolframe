#ifndef _SMERP_PROTOCOL_GENERATOR_INTERFACE_HPP_INCLUDED
#define _SMERP_PROTOCOL_GENERATOR_INTERFACE_HPP_INCLUDED

namespace _SMERP {
namespace protocol {

//@section protocolGeneratorInterface
//Defines the building block for generators as iterator concept in interpreted languages 
//having a form of 'yield' instruction (LUA,Python,etc.).

//Behind a generator is everithing hidden that we want to keep away from the processor:
// - different levels of source transformation and filtering (for example XML Path selection and charset mapping)
// - end of content recognition (part of the protocol)


//interface for (non copyable, non STL conform) iterator over input content elements
class Generator
{
   public:            
      //the client of the generator decides when and how he wants to yield according this state.
      enum State {Init, Processing, EndOfBuffer, EndOfInput, Error};

      //generator element
      class Element
      {
         protected:
            const char* m_value;
            unsigned int m_size;
            int m_type;
            
         public:
            Element()                                             :m_value(0),m_size(0),m_type(0){};
            void init( const char* v, unsigned int s, int t)      {m_value=v;m_size=s;m_type=t;};
            const char* value() const                             {return m_value;};
            unsigned int size() const                             {return m_size;};
            int type() const                                      {return m_type;};
      };
      
      Generator()                                              :m_state(Init){};
      
      const Element& operator*() const                         {return m_cur;};
      const Element* operator->() const                        {return &m_cur;};
      State state() const                                      {return m_state;};
      
      virtual ~Generator() {};
      virtual bool skip()=0;
      virtual void feed( void* block, unsigned int blocksize)=0;
      virtual void getRestBlock( void** block, unsigned int* blocksize)=0;
      
   protected:
      Element m_cur;
      State m_state;
};

}}//namespace
#endif



