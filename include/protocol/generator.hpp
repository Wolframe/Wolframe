#ifndef _SMERP_PROTOCOL_GENERATOR_INTERFACE_HPP_INCLUDED
#define _SMERP_PROTOCOL_GENERATOR_INTERFACE_HPP_INCLUDED

namespace _SMERP {
namespace protocol {

//@section protocolGeneratorInterface
//Defines the building block for generators as iterator concept in interpreted languages 
//having a form of 'yield' instruction (LUA,Python,etc.).
//the protocol passes the content to process to the generator.

//Behind a generator are things hidden from the processor:
// - different levels of source transformation and filtering (for example XML Path selection and charset mapping)
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
         EndOfMessage,   //EWOULDBLK -> have to yield
         EndOfInput,     //EOF
         Error           //an error occurred
      };

      Generator()                                                 :m_state(Init){}      
      State state() const                                         {return m_state;}
      //set the got end of data flag for the generator
      void setEoD()                                               {m_gotEoD=true;}
      
      virtual ~Generator() {}
      
      //used to go to next element (see example below)
      virtual bool next( const void** elem, unsigned int* elemsize)=0;

      //used by the protocol to pass more content to the generator in yield state.
      virtual void protocolInput( const void* block, unsigned int blocksize)=0;

      //used by functions that transform generators (like XML header to content with different character set)
      virtual void getRestBlock( const void** block, unsigned int* blocksize)=0;

      //get the error details
      //@param msg pointer to return bthe error message as string 
      //@return the error code in case of an error state or 0.  
      virtual int getError( const char** msg=0) const {if (msg) *msg=0; return 0;}

      //@example generator function in C for the processor:
      //
      //bool nativeGeneratorCall( Generator* g, const void** b, unsigned int* n)
      //{
      //   if (!g->next( b, n)) 
      //   {
      //      if (g->state() == Generator::EndOfInput) return false;
      //      nativeCallYield();
      //   }
      //   return true;
      //}

   protected:
      State m_state;
      bool m_gotEoD;
};

}}//namespace
#endif



