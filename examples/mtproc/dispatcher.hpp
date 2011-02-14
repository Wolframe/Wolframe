#ifndef _SMERP_METHODTABLE_DISPATCHER_PRIVATE_HPP_INCLUDED
#define _SMERP_METHODTABLE_DISPATCHER_PRIVATE_HPP_INCLUDED
#include "protocol.hpp"
#include "methodtable.hpp"

namespace _SMERP {
namespace mtproc {

struct CommandHandler
{
private:
   enum State {Null,Init,Selected,Running};
   
   protocol::ProtocolParser m_parser;       //< parser for the commands
   Instance* m_instance;                    //< method table and data
   Method::Context m_context;               //< context of current method executed
   unsigned int m_methodIdx;                //< index of currently executed method or -1
   State m_state;                           //< command handler state

   //set the current command to unknown without affecting the processed data object
   void resetCommand();
   
   //initialize the processed data object with its methods and set the current command to unknown
   void init( const char** protocolCmds, Instance* instance);

   //pass a protocol data input chunk to the processors generator function
   void processorInput( Input& input, Input::iterator& end);
   
public:
   //initialize the prcessed data object with all its methods and some default protocol commands
   enum Command {unknown=-1, empty=0, caps, quit, method};      
   void init( Instance* instance)
   {
      static const char* cmd[4] = {"","caps","quit", 0};
      init( cmd, instance);
   }
   
   CommandHandler( Instance* instance)
   {
      init( instance);
   }

   CommandHandler()
   {
      init( 0);
   }

   Command getCommand();
   int call( int argc, const char** argv);

   template <class Buffer>
   void writeCaps( Buffer& buf)
   {
      unsigned int ii;
      if (m_instance && m_instance->mt)
      {
         for( unsigned int ii=0; instance->mt[ii].call && instance->mt[ii].name; ii++)
         {
            if (ii>0)
            {
               buf.push_back( ',');
               buf.push_back( ' ');
            }
            buf.append( instance->mt[ii].name);
         }
      }
   }      
};

}}//namespace
#endif


