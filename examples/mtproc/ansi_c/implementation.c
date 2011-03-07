#include "implementation.h"
#include "methodtable.h"

typedef struct
{
   char buf; 
   protocol::Generator input;
   protocol::FormatOutput output;
}
Data;

static bool getNext( ContentIterator* this_, void* buffer, unsigned int buffersize)
{
   char* in = (char*)this_->m_ptr;
   unsigned int nn = this_->m_size - this->m_pos;

   if (buffersize == 0)
   {
      this_->m_state = Error;
      this_->m_errorcode = 1;
      return false;
   }
   else if (nn == 0)
   {
      if (this_->m_gotEoD)
      {
         this_->m_state = Open;
         return false;
      }
      else
      { 
         this_->m_state = EndOfMessage;
         return false;
      }
   }
   else
   {
      this_->m_state = Open;
      *(char*)buffer = *in;
      this_->m_pos += 1;
      return true;
   }
}

static bool print( FormatOutput* this_, int, void* element, unsigned int elementsize)
{
   char* out = (char*)this_->m_ptr + this_->m_pos;
   unsigned int nn = this_->m_size - this_->m_pos;

   if (elementsize > nn)
   {
      return false;
   }
   else
   {
      std::memcpy( out, element, elementsize);
      this_->m_pos += elementsize;
      return true;
   }
}

static int echo( MethodContext* ctx, unsigned int, const char**)
{
   if (!ctx->contentIterator)
   {
      ctx->contentIterator = &ctx->data->input;
      ctx->output = &ctx->data->output;
   }
   if (ctx->data->buf != 0)
   {
      if (!ctx->output->print( 0, &ctx->data->buf, 1)) return 0;
      ctx->data->buf = 0;
   }
   while (ctx->contentIterator->getNext( &ctx->data->buf, 1))
   {
      if (!ctx->output->print( 0, &ctx->data->buf, 1)) return 0;
      ctx->data->buf = 0;
   }
   return 0;
}

static int printarg( MethodContext* ctx, unsigned int argc, const char** argv)
{
   if (!ctx->contentIterator)
   {
      ctx->contentIterator = &ctx->data->input;
      ctx->output = &ctx->data->output;
   }
   return 0;
}

static const Method methodTable[3] = {{"echo",&echo,true},{"parg",&printarg,false},{0,0}};

static MethodData* createMethodData()
{
   MethodData* rt = (MethodData*)calloc( sizeof(MethodData), 1);
   if (rt == NULL) return 0;
   rt->input.m_getNext = &getNext;
   rt->output.m_print = &print;
   return rt;
}

static void destroyMethodData( MethodData* d)
{
   free( d);
}

static Implementation implementation = { methodTable, 0, &createMethodData, &destroyMethodData };

void* mtproc_getImplementation()
{
   return &implementation;
}


