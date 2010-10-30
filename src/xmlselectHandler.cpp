//
// xmlselectHandler.cpp
//

#include "xmlselectHandler.hpp"
#include "textwolf.hpp"
#include "logger.hpp"

#include <string>
#include <cstring>
#include <boost/static_assert.hpp>

namespace
{
  
struct MemBlock
{
   void* ptr;
   unsigned int size;
   unsigned int filled;
   bool lastblock;
  
   void setEOF() {lastblock=true;};
   
   MemBlock( unsigned int p_size) :ptr(0),size(p_size),filled(0),lastblock(false)
   {
      ptr = (unsigned char*)new unsigned char[ size];
   };
   ~MemBlock()
   {
      free [] (unsigned char*)ptr;
   };
   void init()
   {
      lastblock = false;
      filled = 0;
   },
    
private:
   MemBlock( const MemBlock&) {};
   MemBlock& operator=const MemBlock& o){return *this;};
};


class InputBlock 
{
private:
   MemBlock mem;
   unsigned char* content() const       {return (unsigned char*)mem.ptr;};
   unsigned int size() const            {return mem.size;};
   unsigned int end() const             {return mem.filled;};
   unsigned int pos;
  
public:
   InputBlock( unsigned int p_size)     :mem(p_size),pos(0) {};

   MemBlock& operator->() const         {return mem;};
  
   void init()
   {
      mem.init();
      pos = 0;
   };
   
   struct End {};

   char getchar()
   {         
      if (pos == end())
      {
        pos = 0;
        mem.filled = 0;
        if (lastblock) return 0;
        throw End();
      }
      return content()[ pos++];
   };
  
   struct iterator
   {
      NetMessage* input;
      int ch;
      
      iterator( NetMessage* p_input)      :input(p_input),ch(0) {};
      iterator()                          :input(0),ch(0) {};
      iterator( const iterator& o)        :input(o.input),ch(o.ch) {};
      
      void skip()                         {ch=input->getchar();};                                
      iterator& operator++()              {skip(); return *this;};
      iterator operator++(int)            {iterator tmp(*this); skip(); return tmp;};
      char operator*()                    {return ch;};
   };
   iterator begin()                       {iterator rt(this); rt.skip(); return rt;};
   iterator end()                         {return iterator();};
};


class OutputBlock
{
  unsigned char* content() const         {return (unsigned char*)mem.ptr + mem.filled;};
  unsigned int size() const              {return mem.size - mem.filled;};
  MemBlock mem;
  
public:
   OutputBlock( unsigned int p_size)     :mem(p_size) {};

   MemBlock& operator->() const         {return mem;};

   void init()
   {
      mem.init();
   };
   
   void pack()
   {
      memmove( mem.ptr, content(), size());
      mem.filled = 0;
   };
};

}//namespace io
}//namespace textwolf
#endif


using namespace _SMERP::xmlselect;
namespace tw = textwolf;

struct Connection::Data
{
   InputBlock input;
   OutputBlock output;
   
   typedef tw::XMLPathSelectAutomaton<tw::charset::UTF8> Automaton;
   typedef tw::XMLPathSelect<tw::Input::iterator,tw::charset::IsoLatin1,tw::charset::UTF8> Processor;   

   enum ElementType
   {
      Name, Vorname, Strasse, PLZ, Gemeinde, Tel, Fax, Titel, Sparte, Doc
   };
   enum State
   {
      Init, Welcome, Processing, Error
   };
   const char* error;
   State state;
   Automaton atm;
   Processor* proc;
   Processor::iterator itr;
   Processor::iterator end;
   
   Data() :error(0),state(Init),proc(0),input(MemBlockSize),output(MemBlockSize)
   {
      (*atm)["docs"]["doc"]["name"] = Name;
      (*atm)["docs"]["doc"]["vorname"] = Vorname;
      (*atm)["docs"]["doc"]["strasse"] = Strasse;
      (*atm)["docs"]["doc"]["plz"] = PLZ;
      (*atm)["docs"]["doc"]["gemeinde"] = Gemeinde;
      (*atm)["docs"]["doc"]["tel"] = Tel;
      (*atm)["docs"]["doc"]["fax"] = Fax;
      (*atm)["docs"]["doc"]["titel"] = Titel;
      (*atm)["docs"]["doc"]["sparte"] = Sparte;
      (*atm)["docs"] = Doc;
   };
   
   ~Data()
   {
      done();
   };

   void done()
   {
      itr = end = Processor::iterator( Processor::End());
      if (proc) delete proc;
      proc = 0;
      input.init();
      output.init();
   };
   
   enum {ElemHdrSize=3,ElemTailSize=2};
   void produceElement( unsigned int type, unsigned int size)
   {
      static char HEX[16] = "0123456789abcdef";
      output.content()[0] = HEX[ (type & 0xF0) >> 8];
      output.content()[1] = HEX[ (type & 0x0F)];
      output.content()[2] = ' ';
      output.content()[ElemHdrSize+size] = '\r';
      output.content()[ElemHdrSize+size+1] = '\n';
      output.shift( ElemHdrSize+ElemTailSize+size);
   };
   
   void* elementPtr() const
   {
      output.content() + ElemHdrSize;
   };
   
   unsigned int elementSize() const
   {
      if (output.size() <= ElemHdrSize+ElemTailSize) return 0;
      return output.size()-ElemHdrSize+ElemTailSize;
   };

   enum Result {Read,Write,WriteLast,Error};
   Action get()
   {      
      if (!proc)
      {
         proc = new (std::nothrow) Processor( &atm, input.begin(), elementPtr(), elementSize());
         if (!proc)
         {
            error = "OutOfMem";
            return Error;
         }
         end = processor->end();
         itr = processor->init();
      } 
      else
      {
         output.pack();
      }
      try
      {
         for (itr++; itr!=end; itr++)
         {
            produceElement( itr->type, itr->size);
            proc->setOutputBuffer( elementPtr(), elementSize());
         }
         switch (itr->state)
         {
            case Processor::Ok:          return Write;
            case Processor::EndOfOutput: return Write;
            case Processor::EndOfInput:  return WriteLast;
            case Processor::ErrorState:  error=itr->content; return Error;
         }
      }
      catch (InputBlock::End)
      {
         return Read;
      };
   };
};

Connection::Connection( const LocalTCPendpoint& local )
{
   data = new Connection::Data();
   LOG_TRACE << "Created connection handler for " << local.toString();
   data->state = Init;
}

Connection::Connection( const LocalSSLendpoint& local )
{
   data = new Connection::Data();
   LOG_TRACE << "Created connection handler (SSL) for " << local.toString();
   data->state = Init;
}

Connection::~Connection()
{
   LOG_TRACE << "Connection handler destroyed";
   delete data;
}

void Connection::setPeer( const RemoteTCPendpoint& remote )
{
   LOG_TRACE << "Peer set to " << remote.toString();
}

void Connection::setPeer( const RemoteSSLendpoint& remote )
{
   LOG_TRACE << "Peer set to " << remote.toString();
}


/// Handle a request and produce a reply.
NetworkOperation Connection::nextOperation()
{
  std::string msg;
  switch( data->state)
  {
     case Init:
        data->done();
        data->state = Welcome;
        msg = "OK expecting data\n";
        return NetworkOperation( NetworkOperation::WRITE, msg.c_str(), msg.length());
     
     case Welcome:
        data->state = Processing;
        return NetworkOperation( NetworkOperation::READ, input->ptr, input->size, &input->filled);
     
     case Processing:
     {
         switch (data->get())
         {
            case Read:
               return NetworkOperation( NetworkOperation::READ, data->input->ptr, data->input->size, &data->input->filled);
              
            case Write:
               return NetworkOperation( NetworkOperation::WRITE, data->output->ptr, data->output->size);
               
            case WriteLast:
               data->state = Init;
               return NetworkOperation( NetworkOperation::WRITE, data->output->ptr, data->output->size);
               
            case Error:
               data->state = Error;
               msg.append( "\r\n.\r\nERROR ");
               msg.append( data->error?data->error:"unknown");
               data->done();
               return NetworkOperation( NetworkOperation::WRITE, msg.c_str(), msg.length());
         }
     }
  }
  return NetworkOperation( NetworkOperation::TERMINATE );
}


connectionHandler* Server::newConnection( const LocalTCPendpoint& local )
{
  return new Connection( local );
}

connectionHandler* Server::newSSLconnection( const LocalSSLendpoint& local )
{
  return new Connection( local );
}

