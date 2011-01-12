#ifndef _SMERP_PROTOCOL_TEXT_ITERATOR_HPP_INCLUDED
#define _SMERP_PROTOCOL_TEXT_ITERATOR_HPP_INCLUDED

namespace _SMERP {
namespace protocol {

//@section protocolTextIterator
// iterator over a text content that returns 0 (end of data) with 
// [CR]LF dot(".") [CR]LF reached.
// when 0 is returned the iterator for the protocol can take over again
template <class Iterator>
class TextIterator
{
private:
   Iterator* src_;
   Iterator& src() const                  {return *src_;};
   enum State {SRC,CR,CR_LF,CR_LF_DOT,CR_LF_DOT_CR,CR_LF_DOT_CR_LF,EOFSRC};
   State state;
   
public:
   TextIterator()                         :state(SRC) {};
   TextIterator( Iterator* p_src)         :src_(p_src),state(SRC) {};

   //return the current character
   //  or a '\r' for the '.' after EOL
   //  or 0 if we reached end of data marked by [CR]LF dot(".") [CR]LF
   char cur()
   {
      char ch = *src();
      if (state < CR_LF) return ch;
      if (state == CR_LF)
      {
         if (ch == '.')
         {
            skip();
            ch = *src();
         }
         if (state == CR_LF_DOT && ch == '\r') return 0;
         return ch;
      }
      return (state < CR_LF_DOT_CR_LF)?ch:0;
   };
   
   //skip one character and adapt the EOF sequence detection state
   // its a cascaded if instead of a switch because the states are ordered 
   // ascending according their probability
   void skip()
   {
      if (state == SRC)
      {
         char ch = *src(); 
         if (ch < 32)
         {
            if (ch == '\r')
            {
               state = CR;
            }
            else if (ch == '\n')
            {
               state = CR_LF;
            }
         }
         ++src();
      }
      else if (state == CR)
      {
         char ch = *src();
         if (ch == '\n')
         {
            state = CR_LF;
         }
         else if (ch == '\r')
         {
            //state = CR;
         }
         else
         {
            state = SRC;
         }
         ++src();
      }
      else if (state == CR_LF)
      {
         char ch = *src();
         if (ch == '.')
         {
            state = CR_LF_DOT;         
         }
         else if (ch == '\r')
         {
            state = CR;
         }
         else if (ch == '\n')
         {
            //state = CR_LF;
         }
         else
         {
            state = SRC;
         }
         ++src();
      }
      else if (state == CR_LF_DOT)
      {
         char ch = *src();
         if (ch == '\r')
         {
            state = CR_LF_DOT_CR;
         } 
         else if (ch == '\n')
         {
            state = CR_LF_DOT_CR_LF;
         }
         else
         {
            state = SRC;
         }
         ++src();
      }
      else if (state == CR_LF_DOT_CR)
      {
         char ch = *src();
         if (ch == '\n')
         {
            state = CR_LF_DOT_CR_LF;
            ++src();
         }
         state = EOFSRC;
      }
      else if (state == CR_LF_DOT_CR_LF)
      {
         ++src();
         state = EOFSRC;
      }
   };

   void resume()
   {
      while (state = EOFSRC) ++src();
   };

   TextIterator& operator++()                                  {skip(); return *this;};
   TextIterator operator++(int)                                {TextIterator tmp(*this); skip(); return tmp;};
   char operator*()                                            {return cur();};
   TextIterator& operator=( Iterator* p_src)                   {src_=p_src; return *this;};
};
   
} // namespace protocol
} // namespace _SMERP

#endif // _SMERP_PROTOCOL_TEXT_ITERATOR_HPP_INCLUDED

