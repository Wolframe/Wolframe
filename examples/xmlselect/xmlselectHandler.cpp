//
// xmlselectHandler.cpp
//

#include "xmlselectHandler.hpp"
#include "protocol.hpp"
#include "textwolf.hpp"
#include "protocol.hpp"
#include "logger.hpp"
#include <vector>
#include <string>
#include <cstring>

namespace
{

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
   //* typedefs for input output blocks and input iterators  
   typedef protocol::InputBlock Input;                                           //< input buffer type 
   typedef protocol::OutputBlock Output;                                         //< output buffer type 
   typedef Input::iterator ProtocolIterator;                                     //< iterator type for protocol commands  
   typedef protocol::TextIterator<Input::iterator> ContentIterator;              //< iterator type for content
   typedef tw::XMLPathSelectAutomaton<tw::charset::UTF8> Automaton;
   typedef tw::XMLPathSelect<ContentIterator,tw::charset::IsoLatin1,tw::charset::UTF8> Processor;

   enum ElementType
   {
      Name, Vorname, Strasse, PLZ, Gemeinde, Tel, Fax, Titel, Sparte, Doc
   };
   //* typedefs for state variables and buffers
   //list of processor states
   static const char* elementTypeName( ElementType i)
   {
      static const char* ar[] = {"Name", "Vorname", "Strasse", "PLZ", "Gemeinde", "Tel", "Fax", "Titel", "Sparte", "Doc"};
      return ar[i];
   };

   enum State {Init,EnterCommand,EmptyLine,StartProcessing,ProcessingAfterWrite,Processing,HandleError,Terminate};
   static const char* stateName( State i)
   {
      static const char* ar[] = {"Init","EnterCommand","EmptyLine","StartProcessing","ProcessingAfterWrite","Processing","HandleError","Terminate"};
      return ar[i];
   };

   //buffer for printing messages and reading the arguments of a protocol command with a subset of std::string interface
   class Buffer
   {
   private:
      enum {Size=64};
      unsigned int pos;
      char buf[ Size+1];

   public:
      Buffer()                     :pos(0){};
      void init()                  {pos=0;};
      void push_back( char ch)     {if (pos<=Size) buf[pos++]=ch;};
      unsigned int size() const    {return pos;};
      const char* c_str()          {buf[pos]=0; return buf;}; 
   };

   //* all state variables of this processor
   //1. automaton
   Automaton atm;
   Processor* proc;
   //2. states
   State state;                               //< state of the processor
   const char* error;
   //3. buffers and context
   protocol::Parser::Context protocolState;   //< context (sub state) for partly parsed protocol commands
   Buffer buffer;                             //< context (sub state) for partly parsed input lines 
   Input input;                               //< buffer for READ network messages 
   Output output;                             //< buffer for WRITE network messages
   //3. Iterators
   ProtocolIterator itr;                      //< iterator to scan protocol commands
   ContentIterator citr;
   Processor::iterator src;                   //< iterator to scan content terminated with (CR)LF dor (CR)LF
   Processor::iterator end;                   //< end of content content terminated with (CR)LF dor (CR)LF

   //* helper methods for I/O
   //helper function to send a line message with CRLF termination as C string
   Operation WriteLine( const char* str, const char* arg=0)
   {
      unsigned int ii;
      buffer.init();
      for (ii=0; str[ii]; ii++) buffer.push_back( str[ii]);
      if (arg)
      {
         buffer.push_back( ' ');
         for (ii=0; arg[ii]; ii++) buffer.push_back( arg[ii]);
      }
      buffer.push_back( '\r');
      buffer.push_back( '\n');
      const char* msg = buffer.c_str();
      buffer.init();
      return Operation( Operation::WRITE, msg, ii+2);
   };

   
   Private() :proc(0),state(Init),error(0),input(MemBlockSize),output(MemBlockSize)
   {
      itr = input.begin();
      citr = &itr;
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
      if (proc) delete proc;
   };

   enum {ElemHdrSize=3,ElemTailSize=2};
   void produceElement( unsigned int type, unsigned int size)
   {
      static char HEX[17] = "0123456789abcdef";
      output.rest()[ 0] = HEX[ (type & 0xF0) >> 8];
      output.rest()[ 1] = HEX[ (type & 0x0F)];
      output.rest()[ 2] = ' ';
      output.rest()[ ElemHdrSize+size] = '\r';
      output.rest()[ ElemHdrSize+size+1] = '\n';
      output.shift( ElemHdrSize+ElemTailSize+size);
   };

   char* elementPtr() const
   {
      return (char*)output.rest() + ElemHdrSize;
   };

   unsigned int elementSize() const
   {
      if (output.restsize() <= ElemHdrSize+ElemTailSize) return 0;
      return output.restsize() - ElemHdrSize+ElemTailSize;
   };

   enum Result {Read,Write,WriteLast,ReportError};
   Result get()
   {
       if (!proc)
       {
          proc = new (std::nothrow) Processor( &atm, citr, elementPtr(), elementSize());
          if (!proc)
          {
             error = "OutOfMem";
             return ReportError;
          }
          src = proc->begin();
          end = proc->end();
       }
       try
       {
          for (src++; src!=end; src++)
          {
             produceElement( src->type, src->size);
             proc->setOutputBuffer( elementPtr(), elementSize());
          }
          switch (src->state)
          {
             case Processor::iterator::Element::Ok:          return Write;
             case Processor::iterator::Element::EndOfOutput: return Write;
             case Processor::iterator::Element::EndOfInput:  return WriteLast;
             case Processor::iterator::Element::ErrorState:  error=src->content; return ReportError;
          }
       }
       catch (Input::End)
       {
          return Read;
       };
   };

   Operation nextOperation()
   {
      try 
      {
         for (;;)
         {
             LOG_DATA << "\nState: " << stateName(state);
      
             switch( state)
             {
                case Init:
                {
                    //start or restart:
                    state = EnterCommand;
                    return WriteLine( "OK expecting command");
                }
                
                case EnterCommand:
                {
                    //parsing the command:
                    enum Command {empty, caps, select, quit};
                    static const char* cmd[5] = {"","caps","select","quit",0};
                    //... the empty command is for an empty line for not bothering the client with obscure error messages.
                    //    the next state should read one character for sure otherwise it may result in an endless loop
                    static const protocol::Parser parser(cmd);
                    
                    switch (parser.getCommand( itr, protocolState))
                    {             
                       case empty:
                       {
                          state = EmptyLine;
                          continue;
                       }   
                       case caps: 
                       {
                          state = EnterCommand;  
                          return WriteLine( "OK caps select quit");
                       }
                       case select: 
                       {
                          state = EmptyLine;  
                          continue;
                       }
                       case quit:
                       {
                          state = Terminate;
                          return WriteLine( "BYE");
                       }
                       default:
                       {
                          state = HandleError;
                          return WriteLine( "BAD unknown command");                 
                       }
                    }
                }

                case EmptyLine:
                {
                    //this state is for reading until the end of the line. there is no buffering below,
                    //so we have to the next line somehow:
                    protocol::Parser::getLine( itr, buffer);
                    itr++; //< consume the end of line for not getting into an endless loop

                    if (buffer.size() > 0)
                    {
                       state = Init;
                       buffer.init();
                       //a line starting with a space that is not an empty line leads to an error:
                       return WriteLine( "BAD command line");
                    }
                    else
                    {
                       //here is an empty line, so we jump back to the line promt:
                       state = EnterCommand;
                       continue;
                    }
                }

                case StartProcessing:
                {
                    //read the rest of the line and reject more arguments than expected. 
                    //go on with processing, if this is clear. do not cosnsume the first end of line because it could be
                    //the first character of the EOF sequence.
                    protocol::Parser::getLine( itr, buffer);
                    if (buffer.size() > 0)
                    {
                       state = Init;
                       buffer.init();
                       return WriteLine( "BAD too many arguments");
                    }
                    else
                    {
                       state = Processing;
                       return WriteLine( "OK enter data");
                    }
                }
            
                case ProcessingAfterWrite:
                {
                    //do processing but first release the output buffer content that has been written in the processing state:
                    state = Processing;
                    output.release();
                    continue;
                }
                
                case Processing:
                {
                    switch (get())
                    {
                       case Read:
                          return Operation( Operation::READ, input->ptr, input->size);

                       case Write:
                          state = ProcessingAfterWrite;
                          return Operation( Operation::WRITE, output->ptr, output->filled);

                       case WriteLast:
                          state = Terminate;
                          return Operation( Operation::WRITE, output->ptr, output->size);

                       case ReportError:
                       {
                          state = HandleError;
                          return WriteLine( "ERR", error?error:"unknown");
                       }
                    }
                }

                case HandleError:
                {
                    //in the error case, start again after complaining (Operation::WRITE sent in previous state):
                    protocol::Parser::getLine( itr, buffer);  //parse the rest of the line to clean the input for the next command
                    state = Init;
                    continue;
                }
                
                case Terminate:
                {
                    state = Terminate;
                    return Operation( Operation::TERMINATE);                      
                }

             }//switch(..)
         }//for(,,)
      }
      catch (Input::End)
      {
         LOG_DATA << "End of input interrupt";
         return Operation( Operation::READ, input->ptr, input->size);         
      };
      return Operation( Operation::TERMINATE);
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

Connection::Operation Connection::nextOperation()
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

