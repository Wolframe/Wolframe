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
      delete [] (unsigned char*)ptr;
   };
   void init()
   {
      lastblock = false;
      filled = 0;
   };
    
private:
   MemBlock( const MemBlock&) {};
   MemBlock& operator=( const MemBlock&);
};


class InputBlock 
{
private:
   MemBlock mem;
public:
   unsigned char* content() const       {return (unsigned char*)mem.ptr;};
   unsigned int size() const            {return mem.size;};
   unsigned int endpos() const          {return mem.filled;};
   unsigned int pos;
  
public:
   InputBlock( unsigned int p_size)     :mem(p_size),pos(0) {};

   MemBlock* operator->()               {return &mem;};
  
   void init()
   {
      mem.init();
      pos = 0;
   };
   
   struct End {};

   char getchar()
   {         
      if (pos == endpos())
      {
        pos = 0;
        mem.filled = 0;
        if (mem.lastblock) return 0;
        throw End();
      }
      return content()[ pos++];
   };
  
   struct iterator
   {
      InputBlock* input;
      int ch;
      
      iterator( InputBlock* p_input)          :input(p_input),ch(0) {};
      iterator()                              :input(0),ch(0) {};
      iterator( const iterator& o)            :input(o.input),ch(o.ch) {};
      iterator& operator=( const iterator& o) {input=o.input;ch=o.ch;return *this;}
      
      void skip()                             {ch=input->getchar();};                                
      iterator& operator++()                  {skip(); return *this;};
      iterator operator++(int)                {iterator tmp(*this); skip(); return tmp;};
      char operator*()                        {return ch;};
   };
   iterator begin()                           {iterator rt(this); rt.skip(); return rt;};
   iterator end()                             {return iterator();};
};


class OutputBlock
{
private:
   MemBlock mem;
public:
   unsigned char* content() const         {return (unsigned char*)mem.ptr + mem.filled;};
   unsigned int size() const              {return mem.size - mem.filled;};
   void shift( unsigned int nn)           {mem.filled += nn;};
   
public:
   OutputBlock( unsigned int p_size)     :mem(p_size) {};

   MemBlock* operator->()                {return &mem;};

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

}//anonymous namespace 


using namespace _SMERP;
using namespace _SMERP::xmlselect;
namespace tw = textwolf;

struct Connection::Private
{
   InputBlock input;
   OutputBlock output;
   
   typedef tw::XMLPathSelectAutomaton<tw::charset::UTF8> Automaton;
   typedef tw::XMLPathSelect<InputBlock::iterator,tw::charset::IsoLatin1,tw::charset::UTF8> Processor;   

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
   InputBlock::iterator src;
   
   Private() :input(MemBlockSize),output(MemBlockSize),error(0),state(Init),proc(0),itr(Processor::iterator(Processor::End())),end(Processor::iterator(Processor::End()))
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
   
   ~Private()
   {
      done();
   };

   void done()
   {
      itr = Processor::iterator( Processor::End());
      if (proc) delete proc;
      proc = 0;
      input.init();
      output.init();
   };
   
   enum {ElemHdrSize=3,ElemTailSize=2};
   void produceElement( unsigned int type, unsigned int size)
   {
      static char HEX[17] = "0123456789abcdef";
      output.content()[0] = HEX[ (type & 0xF0) >> 8];
      output.content()[1] = HEX[ (type & 0x0F)];
      output.content()[2] = ' ';
      output.content()[ElemHdrSize+size] = '\r';
      output.content()[ElemHdrSize+size+1] = '\n';
      output.shift( ElemHdrSize+ElemTailSize+size);
   };
   
   char* elementPtr() const
   {
      return (char*)output.content() + ElemHdrSize;
   };
   
   unsigned int elementSize() const
   {
      if (output.size() <= ElemHdrSize+ElemTailSize) return 0;
      return output.size()-ElemHdrSize+ElemTailSize;
   };

   enum Result {Read,Write,WriteLast,ReportError};
   Result get()
   {      
      if (!proc)
      {
         src = input.begin();
         proc = new (std::nothrow) Processor( &atm, src, elementPtr(), elementSize());
         if (!proc)
         {
            error = "OutOfMem";
            return ReportError;
         }
         end = proc->end();
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
            case Processor::iterator::Element::Ok:          return Write;
            case Processor::iterator::Element::EndOfOutput: return Write;
            case Processor::iterator::Element::EndOfInput:  return WriteLast;
            case Processor::iterator::Element::ErrorState:  error=itr->content; return ReportError;
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
   data = new Connection::Private();
   LOG_TRACE << "Created connection handler for " << local.toString();
   data->state = Connection::Private::Init;
}

Connection::Connection( const LocalSSLendpoint& local )
{
   data = new Connection::Private();
   LOG_TRACE << "Created connection handler (SSL) for " << local.toString();
   data->state = Connection::Private::Init;
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

void* Connection::parseInput( const void *begin, std::size_t bytesTransferred )
{
   data->input->filled = bytesTransferred;
   return (void*)(((char*)begin) + bytesTransferred);
}

/// Handle a request and produce a reply.
NetworkOperation Connection::nextOperation()
{
  std::string msg;
  switch( data->state)
  {
     case Connection::Private::Init:
        data->done();
        data->state = Connection::Private::Welcome;
        msg = "OK expecting data\n";
        return NetworkOperation( NetworkOperation::WRITE, msg.c_str(), msg.length());
     
     case Connection::Private::Welcome:
        data->state = Connection::Private::Processing;
        return NetworkOperation( NetworkOperation::READ, data->input->ptr, data->input->size);
     
     case Connection::Private::Processing:
     {
         switch (data->get())
         {
           case Private::Read:
               return NetworkOperation( NetworkOperation::READ, data->input->ptr, data->input->size);
              
            case Private::Write:
               return NetworkOperation( NetworkOperation::WRITE, data->output->ptr, data->output->size);
               
            case Private::WriteLast:
               data->state = Connection::Private::Init;
               return NetworkOperation( NetworkOperation::WRITE, data->output->ptr, data->output->size);
               
            case Private::Error:
               data->state = Connection::Private::Error;
               msg.append( "\r\n.\r\nERROR ");
               msg.append( data->error?data->error:"unknown");
               data->done();
               return NetworkOperation( NetworkOperation::WRITE, msg.c_str(), msg.length());
         }
     }
     case Connection::Private::Error:
         //TODO handle error case
     break;
              
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

