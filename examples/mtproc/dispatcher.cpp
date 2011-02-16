#include "dispatcher.hpp"
#include "methodtable.hpp"
#include "logger.hpp"

using namespace _SMERP::mtproc;
using namespace _SMERP::protocol;

void CommandHandler::resetCommand()
{
   m_methodIdx = 0;
   m_lineBuffer.init();
   m_argBuffer.init();
   m_cmdBuffer.init();

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

void CommandHandler::protocolInput( protocol::InputBlock::iterator& start, protocol::InputBlock::iterator& end)
{
   if (m_state == WaitForInput)
   {
      LOG_DATA << "command handler got input";
      m_state = Running;
   }
   else
   {
      LOG_ERROR << "illegal state (unexpected protocol input)";
      throw (IllegalState());
   }
   if (m_context.contentIterator)
   {
      m_context.contentIterator->protocolInput( &start[0], end-input.begin());
   }
   else
   {
      LOG_ERROR << "illegal state: got input without recipient (content iterator disapeared)";
      throw (IllegalState());
   }
}

CommandHandler::CommandHandler( Instance* instance=0)
   :m_argBuffer( &m_lineBuffer);
{
   init( instance);
}

CommandHandler::~CommandHandler()
{
   if (m_instance) delete m_instance;
}

CommandHandler::Command CommandHandler::getCommand( protocol::InputBlock::iterator& itr, protocol::InputBlock::iterator& eoM)
{
   switch (m_state)
   {
      case Running:
      case WaitForInput:
      case Null:
      case Init:
      {
         resetCommand();
         int ci = m_parser.getCommand( itr, eoM, m_cmdBuffer);
         if (ci >= unknown && ci < method)
         {
            return (Command)ci;
         }
         m_methodIdx = (unsigned int)ci - (unsigned int)method;
         m_state = Selected;
         //no break here !
      }
      case Selected:
      {
         if (!getLine( itr, eoM, m_argBuffer)) return unknown;
         //no break here !
      }
      case ArgumentsParsed:
      {
         return method;
      }
   }
   LOG_ERROR << "illegal state (end of getCommand)";
   throw (IllegalState());
}


IOState CommandHandler::call( int& returnCode)
{
   switch (m_state)
   {
      case Null:
      case Init:
      case Selected:
      { 
         LOG_ERROR << "illegal call in this state (not running)";
         throw (IllegalState());
      }
      case ArgumentsParsed:
      {
         LOG_DEBUG << "call of '" << m_instance->mt[ m_methodIdx].name << "'";
         m_state = Running;
         //no break here !
      }
      case WaitForInput:
         LOG_ERROR << "called without input";
         //no break here !
         
      case Running:
      {
         returnCode = m_instance->mt[ m_methodIdx].call( &m_context, m_argBuffer.argc(), m_argBuffer.argv());
         if (returnCode != 0)
         {
            LOG_ERROR << "error " << rt << " calling '" << m_instance->mt[ m_methodIdx].name << "'";
            resetCommand();
            return Close;
         }
         if (m_context.contentIterator)
         {
            const char* errmsg;
            switch (m_context.contentIterator->state())
            {
               case protocol::Generator::Init:
               case protocol::Generator::Processing:
                  return WriteOutput;

               case protocol::Generator::Error:
                  returnCode = m_context.contentIterator->getError( &errmsg);
                  LOG_ERROR << "error " << errmsg << "(" << returnCode << ") in generator calling '" << m_instance->mt[ m_methodIdx].name << "'";
                  resetCommand();
                  return Close;

               case protocol::Generator::EndOfInput:
                  resetCommand();
                  return Close;

               case protocol::Generator::EndOfMessage:
                  m_state = WaitForInput;
                  return ReadInput;
            }
         }
      }
   }
   return Close;
}

const char* CommandHandler::getCaps()
{
   m_lineBuffer.init();
   unsigned int ii;
   if (m_instance && m_instance->mt)
   {
      for (ii=0; m_instance->mt[ii].call && m_instance->mt[ii].name; ii++)
      {
         if (ii>0)
         {
            m_lineBuffer.push_back( ',');
            m_lineBuffer.push_back( ' ');
         }
         m_lineBuffer.append( m_instance->mt[ii].name);
      }
   }
   return m_lineBuffer.c_str();
}      


