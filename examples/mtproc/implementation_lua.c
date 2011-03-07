#include "implementation_lua.h"
#include "methodtable_c.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include <string.h>
#include <malloc.h>
#define UNUSED(x) ( (void)(x) )

typedef struct
{
   char buf; 
   ContentIterator input;
   FormatOutput output;
   lua_State* lua;
}
MethodData;

static bool getNext( ContentIterator* this_, void* buffer, unsigned int buffersize)
{
   char* in = (char*)this_->m_ptr;
   unsigned int nn = this_->m_size - this_->m_pos;

   if (buffersize == 0)
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
      return true;
   }
}

static bool print( FormatOutput* this_, int type_, void* element, unsigned int elementsize)
{
   char* out = (char*)this_->m_ptr + this_->m_pos;
   unsigned int nn = this_->m_size - this_->m_pos;
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

static int luacall_withData( MethodContext* ctx, unsigned int argc, const char** argv)
{
   //MethodData* data = (MethodData*)ctx->data;
   UNUSED(ctx);
   UNUSED(argc);
   UNUSED(argv);
   return 0;
}

static int luacall_withoutData( MethodContext* ctx, unsigned int argc, const char** argv)
{
   //MethodData* data = (MethodData*)ctx->data;
   UNUSED(ctx);
   UNUSED(argc);
   UNUSED(argv);
   return 0;
}

static const Method methodTable[3] = {{"echo",&luacall_withData,true},{"parg",&luacall_withoutData,false},{0,0,false}};

static MethodDataP createMethodData(void)
{
   MethodData* rt = (MethodData*)calloc( sizeof(MethodData), 1);
   if (rt == NULL) return NULL;
   rt->input.m_getNext = &getNext;
   rt->output.m_print = &print;
   rt->lua = luaL_newstate();
   if (rt->lua == NULL)
   {
      free( rt);
      return NULL;
   }
   return (MethodDataP)rt;
}

static void destroyMethodData( MethodDataP d)
{
   MethodData* this_ = (MethodData*)d;
   lua_close( this_->lua);
   free( d);
}

static Implementation implementation = { methodTable, 0, &createMethodData, &destroyMethodData };

void* mtproc_getImplementation_Lua(void)
{
   return &implementation;
}


