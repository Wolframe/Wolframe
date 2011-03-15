#include "implementation.hpp"
#include "logger.hpp"
#include "generators/char_isolatin1.hpp"
#include <new>

using namespace _Wolframe::mtproc;

//example mtprocHandler object implementation in C++ish form
struct Method::Data
{
   char buf;
   protocol::Generator input;
   protocol::FormatOutput output;

   Data() :buf(0),input(generator::CharIsoLatin1::GetNext),output(generator::CharIsoLatin1::Print){}
};


Method::Data* Implementation::createData()
{
   return new (std::nothrow) Method::Data();
}

void Implementation::destroyData( Method::Data* data)
{
   delete data;
}

int Implementation::echo( Method::Context* ctx, unsigned int, const char**)
{
   LOG_DATA << "Method Call echo";

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
   protocol::Generator::ElementType type;
   unsigned int bp = 0;
   while (ctx->contentIterator->getNext( &type, &ctx->data->buf, 1, &bp))
   {
      if (!ctx->output->print( 0, &ctx->data->buf, 1)) return 0;
      ctx->data->buf = 0;
      bp = 0;
   }
   return 0;
}

int Implementation::printarg( Method::Context* ctx, unsigned int, const char**)
{
   LOG_DATA << "Method Call printarg";

   if (!ctx->contentIterator)
   {
      ctx->contentIterator = &ctx->data->input;
      ctx->output = &ctx->data->output;
   }
   return 0;
}


