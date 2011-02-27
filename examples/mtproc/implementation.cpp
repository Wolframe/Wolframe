#include "implementation.hpp"
#include "logger.hpp"
#include "generators/char_isolatin1.hpp"

using namespace _Wolframe::mtproc;

int Method::Data::echo( Context* ctx, unsigned int, const char**)
{
   LOG_DATA << "Method Call";

   if (!ctx->contentIterator) ctx->contentIterator = new protocol::Generator( generator::CharIsoLatin1::GetNext);
   if (!ctx->output) ctx->output = new protocol::FormatOutput( generator::CharIsoLatin1::Print);
   char ch;
   while (ctx->contentIterator->getNext( &ch, 1))
   {
      if (!ctx->output->print( 0, &ch, 1)) return 0;
   }
   return 0;
}

int Method::Data::printarg( Context* ctx, unsigned int, const char**)
{
   if (!ctx->output) ctx->output = new protocol::FormatOutput( generator::CharIsoLatin1::Print);
   return 0;
}


