#include "dispatcher.hpp"
#include "methodtable.hpp"
#include "logger.hpp"

using namespace _SMERP::mtproc;
using namespace _SMERP::protocol;

void CommandHandler::resetCommand()
{
   m_methodIdx = 0;
   if (m_instance)
   {
      m_state = Init;
      m_context.init( m_instance->data);
   }
   else
   {
      m_state = Null;
      m_context.init( m_instance->data);
   }
}

void CommandHandler::init( const char** protocolCmds, Instance* instance)
{
   m_instance = instance;
   m_parser.init();
   resetCommand();
   
   if (protocolCmds)
   {
      for( unsigned int ii=0; protocolCmds[ ii]; ii++)
      {
         m_parser.add( protocolCmds[ ii]);
      }
   }
   if (instance)
   {
      m_context.data = instance->data;
      m_context.contentIterator = 0;

      if (instance->mt)
      {
         for( unsigned int ii=0; instance->mt[ii].call && instance->mt[ii].name; ii++)
         {
            m_parser.add( instance->mt[ii].name);
         }
      }
   }
}

void CommandHandler::protocolInput( protocol::InputBlock& input, protocol::InputBlock::iterator& end)
{
   if (m_state == Running)
   {
      if (m_context.contentIterator)
      {
         m_context.contentIterator->protocolInput( input.ptr(), end-input.begin());
      }
      else
      {
         LOG_ERROR << "content iterator disapeared";
      }
   }
   else
   {
      LOG_ERROR << "illegal state (running but no context)";
      init(0);
   }
}

CommandHandler::~CommandHandler()
{
   if (m_instance) delete m_instance;
}

CommandHandler::Command CommandHandler::getCommand( protocol::InputBlock::iterator& itr, protocol::InputBlock::iterator& eoM)
{
   resetCommand();
   int ci = m_parser.getCommand( itr, eoM, m_cmdBuffer);
   if (ci >= unknown && ci < method)
   {
      return (Command)ci;
   }
   else
   {
      m_methodIdx = (unsigned int)ci - (unsigned int)method;
      m_state = Selected;
      return method;
   }
}

int CommandHandler::call( int argc, const char** argv)
{
   if (m_state == Selected)
   {
      LOG_DEBUG << "call of '" << m_instance->mt[ m_methodIdx].name << "'";
      m_state = Running;
   }
   if (m_state != Running)
   {
      LOG_ERROR << "illegal call in this state (not running)";
      init(0);
      return 0;
   }
   int rt = m_instance->mt[ m_methodIdx].call( &m_context, argc, argv);
   if (rt != 0)
   {
      LOG_ERROR << "error " << rt << " calling '" << m_instance->mt[ m_methodIdx].name << "'";
      resetCommand();
   }
   else
   {
      const char* errmsg;
      switch (m_context.contentIterator->state())
      {
         case protocol::Generator::Init:
         case protocol::Generator::Processing:
            break;

         case protocol::Generator::Error:
            rt = m_context.contentIterator->getError( &errmsg);
            LOG_ERROR << "error " << errmsg << "(" << rt << ") in generator calling '" << m_instance->mt[ m_methodIdx].name << "'";
            resetCommand();
            break;

         case protocol::Generator::EndOfInput:
            resetCommand();
            break;

         case protocol::Generator::EndOfBuffer:
            break;
      }
   }
   return rt;
}



