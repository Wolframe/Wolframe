/************************************************************************
Copyright (C) 2011 - 2013 Project Wolframe.
All rights reserved.

This file is part of Project Wolframe.

Commercial Usage
Licensees holding valid Project Wolframe Commercial licenses may
use this file in accordance with the Project Wolframe
Commercial License Agreement provided with the Software or,
alternatively, in accordance with the terms contained
in a written agreement between the licensee and Project Wolframe.

GNU General Public License Usage
Alternatively, you can redistribute this file and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Wolframe is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
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


