#ifndef _Wolframe_FILTER_CHAR_HPP_INCLUDED
#define _Wolframe_FILTER_CHAR_HPP_INCLUDED
#include "protocol/inputfilter.hpp"
#include "protocol/formatoutput.hpp"
#include "filters/filterBase.hpp"
#include <cstring>

namespace _Wolframe {
namespace filter {

template <class IOCharset, class AppCharset=textwolf::charset::UTF8>
struct CharFilter :FilterBase<IOCharset, AppCharset>
{
	typedef typename FilterBase<IOCharset, AppCharset>::FormatOutputBase FormatOutputBase;

	struct FormatOutput :public FormatOutputBase
	{
		FormatOutput( unsigned int bufsize=32) :FormatOutputBase(bufsize){}
	};

	struct InputFilter :public protocol::InputFilter
	{
		enum ErrorCodes {Ok=0,ErrBufferTooSmall=1};

		virtual bool getNext( ElementType* type, void* buffer, size_type buffersize, size_type* bufferpos)
		{
			setState( Open);
			*type = Value;
			CharIterator itr( (char*)ptr(), size());
			textwolf::TextScanner<CharIterator,AppCharset> ts( itr);

			textwolf::UChar ch;
			if ((ch = *itr) != 0)
			{
				size_type nn = IOCharset::print( ch, (char*)buffer+*bufferpos, buffersize-*bufferpos);
				if (nn > 0)
				{
					*bufferpos += nn;
					++itr;
					skip( itr.pos());
					return true;
				}
			}
			if (!gotEoD()) setState( EndOfMessage);
			return false;
		}
	};
};

}}//namespace
#endif


