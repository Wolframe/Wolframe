#include "implementation_c.h"
#include "methodtable_c.h"
#include <string.h>
#include <malloc.h>
#define UNUSED(x) ( (void)(x) )

typedef struct MethodDataT
{
   char buf;
   ContentIterator input;
   FormatOutput output;
}
MethodData;

static bool getNext( ContentIterator* this_, ElementType* type, void* buffer, size_t buffersize, size_t* bufferpos)
{
   char* in = (char*)this_->m_ptr;
   size_t nn = this_->m_size - this_->m_pos;
   *type = Value;

   if (buffersize == *bufferpos)
   {
      this_->m_state = Error;
      this_->m_errorCode = 1;
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
      *bufferpos += 1;
      return 1;
   }
}

static bool print( FormatOutput* this_, int type_, void* element, size_t elementsize)
{
   char* out = (char*)this_->m_ptr + this_->m_pos;
   size_t nn = this_->m_size - this_->m_pos;
   UNUSED(type_);

   if (elementsize > nn)
   {
      return false;
   }
   else
   {
      memcpy( out, element, elementsize);
      this_->m_pos += elementsize;
      return true;
   }
}

static int echo( MethodContext* ctx, unsigned int argc, const char** argv)
{
   MethodData* data = (MethodData*)ctx->data;
   size_t bb = 0;
   ElementType type;
   UNUSED(argc);
   UNUSED(argv);

   if (!ctx->contentIterator)
   {
      ctx->contentIterator = &data->input;
      ctx->formatOutput = &data->output;
   }
   if (data->buf != 0)
   {
      if (!data->output.m_print( &data->output, 0, &data->buf, 1)) return 0;
      data->buf = 0;
   }
   while (ctx->contentIterator->m_getNext( ctx->contentIterator, &type, &data->buf, 1, &bb))
   {
      if (!data->output.m_print( &data->output, 0, &data->buf, 1)) return 0;
      data->buf = 0;
      bb = 0;
   }
   return 0;
}

static int printarg( MethodContext* ctx, unsigned int argc, const char** argv)
{
   MethodData* data = (MethodData*)ctx->data;
   UNUSED(argc);
   UNUSED(argv);

   if (!ctx->contentIterator)
   {
      ctx->contentIterator = &data->input;
      ctx->formatOutput = &data->output;
   }
   return 0;
}

static const Method methodTable[3] = {{"echo",&echo,true},{"parg",&printarg,false},{0,0,false}};

static MethodDataP createMethodData(void)
{
   MethodData* rt = (MethodData*)calloc( sizeof(MethodData), 1);
   if (rt == NULL) return NULL;
   rt->input.m_getNext = &getNext;
   rt->output.m_print = &print;
   return (MethodDataP)rt;
}

static void destroyMethodData( MethodDataP d)
{
   free( d);
}

static Implementation implementation = { methodTable, 0, &createMethodData, &destroyMethodData };

void* mtproc_getImplementation(void)
{
   return &implementation;
}


