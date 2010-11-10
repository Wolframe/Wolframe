//
// xmlselectHandler.cpp
//

#include "xmlselectHandler.hpp"
#include "textwolf.hpp"
#include "logger.hpp"

#include <vector>
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

//iterator over a content that terminates with a defined character sequence
template <class Iterator>
class ProtocolContentIterator
{
private:
   const char* endSequence;
   const char* retSequence;
   unsigned int state;
   char next;
   Iterator src;

public:
   ProtocolContentIterator( const char* p_endSequence, const char* p_retSequence) :endSequence(p_endSequence),retSequence(p_retSequence),state(0),next(endSequence[0]) {};

   void set( const Iterator& p_src)                            {src=p_src;};
   void skip()                                                 {src++;};
   ProtocolContentIterator& operator++()                       {skip(); return *this;};
   ProtocolContentIterator operator++(int)                     {ProtocolContentIterator tmp(*this); skip(); return tmp;};
   char operator*()                                            {if (*src == next) {next = endSequence[++state]; if (next==0) return 0; else return retSequence[state-1];} else {state=0; return *src;}};
   ProtocolContentIterator& operator=( const Iterator& p_src)  {src=p_src; return *this;};
};

//specialization of the content iterator with the termination sequence CRLF dot CRLF with the dot escaping any other line than a single dot
struct TextContentIterator :public ProtocolContentIterator<InputBlock::iterator>
{
   TextContentIterator() :ProtocolContentIterator<InputBlock::iterator>( "\r\n.\r\n", "\r\n \r\n") {};
   TextContentIterator& operator=( const InputBlock::iterator& p_src)    {set(p_src); return *this;};
};

struct XPathExpression
{
public:
   struct Error {};

   struct Element
   {
      const char* name;
      unsigned int namesize;
      enum Type
      {
	 Tag,
	 Follow,
	 AttributeName,
	 AttributeValue,
	 Count
      };
      Type type;

      Element()                                                               :name(0),namesize(0),type(Count) {};
      Element( Type p_type, const char* p_name=0, unsigned int p_namesize=0)  :name(p_name),namesize(p_namesize),type(p_type) {};
      Element( const Element& o)                                              :name(o.name),namesize(o.namesize),type(o.type) {};
   };
private:
   const char* input;
   std::vector<Element> ar;
   unsigned int pos;
   bool valid;

   void skipB()
   {
      const char* name = input+pos;
      unsigned int ii;
      for (ii=0; name[ii] != '\0' && name[ii]<=' '; ii++);
      if (name[ii] == '\0') throw Error();
      pos += ii;
   };
   void parseNumber( Element::Type type)
   {
      const char* name = input+pos;
      unsigned int ii;
      for (ii=0; name[ii] >= '0' && name[ii] <= '9'; ii++);
      if (ii == 0) throw Error();
      ar.push_back( Element( type, name, ii));
      pos += ii;
      skipB();
   };
   void parseName( Element::Type type)
   {
      const char* name = input+pos;
      unsigned int ii;
      for (ii=0; name[ii] != '/' && name[ii] != '[' && name[ii] != '@' && name[ii] != '=' && name[ii] != '\0'; ii++);
      if (ii == 0) throw Error();
      ar.push_back( Element( type, name, ii));
      pos += ii;
      skipB();
   };
   void parseString( Element::Type type, char eb)
   {
      const char* name = input+pos;
      unsigned int ii;
      for (ii=0; name[ii] != eb && name[ii] != '\0'; ii++);
      if (name[ii] == '\0') throw Error();
      ar.push_back( Element( type, name, ii));
      pos += ii+1;
      skipB();
   };
   void parseElement()
   {
      skipB();
      if (input[pos] == '@')
      {
	 pos++;
	 parseName( Element::AttributeName);
	 if (input[pos] == '=')
	 {
	    pos++;
	    if (input[pos] == '\'' || input[pos] == '\"')
	    {
	       pos++;
	       parseString( Element::AttributeValue, input[pos-1]);
	    }
	    else
	    {
	       parseName( Element::AttributeValue);
	    }
	 }
      }
      else
      {
	 parseNumber( Element::Count);
      }
      if (input[pos] != ']') throw Error();
      pos++;
      skipB();
   };
   void parse()
   {
      while (input[pos] != '\0')
      {
	  if (input[pos] == '/')
	  {
	     pos++;
	     if (input[pos] == '/')
	     {
		ar.push_back( Element::Follow);
	     }
	     else
	     {
		parseName( Element::Tag);
	     }
	  }
	  else if (input[pos] == '@')
	  {
	     parseName( Element::AttributeName);
	  }
	  else if (input[pos] == '[')
	  {
	     parseElement();
	  }
      }
   };
public:
   XPathExpression( const char* p_input)      :input(p_input),pos(0),valid(true)
   {
      try
      {
	 parse();
      }
      catch (Error)
      {
	 valid = false;
      };
   };

   int errorPos()
   {
      return (valid)?-1:(int)pos;
   };


   struct iterator
   {
      XPathExpression* input;
      Element cur;
      unsigned int pos;
      bool eof;

      iterator( XPathExpression* p_input)     :input(p_input),pos(0),eof(false) {};
      iterator()                              :input(0),pos(0),eof(true) {};
      iterator( const iterator& o)            :input(o.input),cur(o.cur),pos(o.pos),eof(o.eof) {};
      iterator& operator=( const iterator& o) {input=o.input;cur=o.cur;pos=o.pos;eof=o.eof; return *this;}

      void skip()
      {
	 if (pos < input->ar.size())
	 {
	    cur = input->ar[ pos++];
	 }
	 else
	 {
	    eof = true;
	 }
      };
      iterator& operator++()                  {skip(); return *this;};
      iterator operator++(int)                {iterator tmp(*this); skip(); return tmp;};
      Element& operator*()                    {return cur;};
      Element* operator->()                   {return &cur;};
   };
   iterator begin()                           {iterator rt(this); rt.skip(); return rt;};
   iterator end()                             {return iterator();};
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
   typedef tw::XMLPathSelect<TextContentIterator,tw::charset::IsoLatin1,tw::charset::UTF8> Processor;

   enum ElementType
   {
      Name, Vorname, Strasse, PLZ, Gemeinde, Tel, Fax, Titel, Sparte, Doc
   };
   enum State
   {
      Init, Welcome, Processing, Terminate
   };
   const char* error;
   State state;
   Automaton atm;
   Processor* proc;
   Processor::iterator itr;
   Processor::iterator end;
   TextContentIterator src;

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

   Network::NetworkOperation nextOperation()
   {
      std::string msg;
      const char* errstr;

      switch( state)
      {
	case Init:
	   done();
	   state = Welcome;
	   msg = "OK expecting data\n";
	   return Network::NetworkOperation( Network::NetworkOperation::WRITE, msg.c_str(), msg.length());

	case Welcome:
	   state = Processing;
	   return Network::NetworkOperation( Network::NetworkOperation::READ, input->ptr, input->size);

	case Processing:
	{
	   switch (get())
	   {
		case Read:
		   return Network::NetworkOperation( Network::NetworkOperation::READ, input->ptr, input->size);

		case Write:
		   return Network::NetworkOperation( Network::NetworkOperation::WRITE, output->ptr, output->size);

		case WriteLast:
		   state = Terminate;
		   return Network::NetworkOperation( Network::NetworkOperation::WRITE, output->ptr, output->size);

		case ReportError:
		   state = Terminate;
		   errstr = error?error:"unknown";
		   LOG_TRACE << "Error processing xml: " << errstr;
		   msg.append( "\r\n.\r\nERROR ");
		   msg.append( errstr);
		   done();
		   return Network::NetworkOperation( Network::NetworkOperation::WRITE, msg.c_str(), msg.length());
	    }
	}
	case Terminate:
	    state = Init;
	    return Network::NetworkOperation( Network::NetworkOperation::TERMINATE);
	break;

      }
      return Network::NetworkOperation( Network::NetworkOperation::TERMINATE);
   };
};

Connection::Connection( const Network::LocalTCPendpoint& local)
{
   data = new Private();
   LOG_TRACE << "Created connection handler for " << local.toString();
   data->state = Connection::Private::Init;
}

Connection::Connection( const Network::LocalSSLendpoint& local)
{
   data = new Private();
   LOG_TRACE << "Created connection handler (SSL) for " << local.toString();
   data->state = Connection::Private::Init;
}

Connection::~Connection()
{
   LOG_TRACE << "Connection handler destroyed";
   delete data;
}

void Connection::setPeer( const Network::RemoteTCPendpoint& remote)
{
   LOG_TRACE << "Peer set to " << remote.toString();
}

void Connection::setPeer( const Network::RemoteSSLendpoint& remote)
{
   LOG_TRACE << "Peer set to " << remote.toString();
}

void* Connection::parseInput( const void *begin, std::size_t bytesTransferred)
{
   data->input->filled = bytesTransferred;
   return (void*)(((char*)begin) + bytesTransferred);
}

Network::NetworkOperation Connection::nextOperation()
{
   return data->nextOperation();
}

Network::connectionHandler* Server::newConnection( const Network::LocalTCPendpoint& local)
{
   return new Connection( local);
}

Network::connectionHandler* Server::newSSLconnection( const Network::LocalSSLendpoint& local)
{
   return new Connection( local);
}

