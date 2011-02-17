#include "implementation.hpp"
#include "generators/char_isolatin1.hpp"

using namespace _SMERP::mtproc;

int Method::Data::echo( Context* ctx, unsigned int, const char**)
{
   if (!ctx->contentIterator) ctx->contentIterator = new isolatin1::Generator();
   return 0;
}

int Method::Data::printarg( Context*, unsigned int, const char**)
{
   return 0;
}


