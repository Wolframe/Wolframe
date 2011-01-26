#include "protocol.hpp"

using namespace _SMERP;
using namespace protocol;

#if 0
void InputBlock::markEndOfData( EODState& state, const InputBlock::const_iterator& start)
{
   if (size()>start.pos()) return;
        
   unsigned int bufsize = size()-start.pos();
   char* buf = charptr()+start.pos();
   unsigned int bufpos,eatsize,dstsize;
   dstsize = bufpos = eatsize = 0;

   while (bufpos<bufsize)
   {
       if (state == SRC)
       {
           char* cc = (char*)memchr( buf+bufpos, '\n', bufsize-bufpos);
           if (cc)
           {
               bufpos = cc - buf;
               state = LF;
           }
           else
           {
               bufpos = bufsize;
               if (dstsize != eatsize) memmove( buf+dstsize, buf+eatsize, bufpos-eatsize);
               dstsize += bufpos-eatsize;
               eatsize = bufpos;
           }
       }
       else if (state == LF)
       {
          if (buf[bufpos] == '.')
          {
              state = LF_DOT;
              if (dstsize != eatsize) memmove( buf+dstsize, buf+eatsize, bufpos-eatsize);
              dstsize += bufpos-eatsize;
              bufpos++;
              eatsize = bufpos;
          }
          else
          {
              state = SRC;
          }
       }
       else if (state == LF_DOT)
       {
          if (buf[bufpos] == '\r')
          {
              memmove( buf+dstsize++, "", 1);  //< feed EOD
              state = LF_DOT_CR;
              bufpos++; 
          }
          else if (buf[bufpos] == '\n')
          {
              memmove( buf+dstsize++, "", 1);  //< feed EOD
              state = LF_DOT_CR_LF;
              bufpos++;
          }
          else
          {
              state = SRC;
          }
       }
       else if (state == LF_DOT_CR)
       {
           if (buf[bufpos] == '\n') bufpos++; 
           state = LF_DOT_CR_LF;
       }
       else //if (state == LF_DOT_CR_LF)
       {
            bufpos = bufsize;
            if (dstsize != eatsize) memmove( buf+dstsize, buf+eatsize, bufpos-eatsize);
            dstsize += bufpos-eatsize;
            eatsize = bufpos;
       }
   }
   setPos( start.pos() + dstsize);
}
#endif
