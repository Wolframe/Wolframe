#include "implementation.hpp"
#include "generators/char_isolatin1.hpp"

using namespace _SMERP::mtproc;

int Method::Data::echo( Context* ctx, unsigned int, const char**)
{
   if (!ctx->contentIterator) ctx->contentIterator = new protocol::Generator( generator::CharIsoLatin1::GetNext);
   return 0;
}

int Method::Data::printarg( Context*, unsigned int, const char**)
{
   return 0;
}


