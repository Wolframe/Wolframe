#ifndef _SMERP_PROTOCOL_GENERATOR_INTERFACE_HPP_INCLUDED
#define _SMERP_PROTOCOL_GENERATOR_INTERFACE_HPP_INCLUDED

namespace _SMERP {
namespace protocol {

//@section protocolGeneratorInterface
//Defines the building block for generators as iterator concept in interpreted languages 
//having a form of 'yield' instruction (LUA,Python,etc.).

//Behind a generator are things hidden from the processor:
// - different levels of source transformation and filtering (for example XML Path selection and charset mapping)
// - end of content recognition (part of the protocol)
// - state handling for different action of the protocol handler in yield


//interface for (non copyable, non STL conform) iterator over input content elements
class Generator
{
   public:            
      //the client of the generator decides when and how he wants to yield according this state.
      enum State
      {
         Init,           //after initialization
         Processing,     //processing
         EndOfBuffer,    //EWOULDBLK -> have to yield
         EndOfInput,     //EOF
         Error           //an error occurred
      };

      //generator element
      class Element
      {
         protected:
            const char* m_value;
            unsigned int m_size;
            int m_type;
            
         public:
            Element()                                             :m_value(0),m_size(0),m_type(0){}
            void init( const char* v, unsigned int s, int t)      {m_value=v;m_size=s;m_type=t;}
            const char* value() const                             {return m_value;}
            unsigned int size() const                             {return m_size;}
            int type() const                                      {return m_type;}
      };
      
      Generator()                                                 :m_state(Init){}
      
      const Element& operator*() const                            {return m_cur;}
      const Element* operator->() const                           {return &m_cur;}
      State state() const                                         {return m_state;}
      
      virtual ~Generator() {}
      
      //used to go to next element (see example below)
      virtual bool skip()=0;

      //used by the protocol to pass more content to the generator in yield state.
      virtual void protocolInput( void* block, unsigned int blocksize)=0;
      
      //used by functions that transform generators (like XML header to content with different character set)
      virtual void getRestBlock( void** block, unsigned int* blocksize)=0;

      //get the error details
      //@param msg pointer to return bthe error message as string 
      //@return the error code in case of an error state or 0.  
      virtual int getError( char** msg=0)=0;

      //@example generator function in C for the processor:
      //
      //bool nativeGeneratorCall( Generator* g, const char** b, unsigned int* n)
      //{
      //   if (!g->skip()) 
      //   {
      //      if (g->state() == Generator::EndOfInput) return false;
      //      nativeCallYield();
      //   }
      //   *b = g->value();
      //   *n = g->size();
      //   return true;
      //}

   protected:
      Element m_cur;
      State m_state;
};

}}//namespace
#endif



