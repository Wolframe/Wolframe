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
	typedef FilterBase<IOCharset, AppCharset> ThisFilterBase;
	typedef typename protocol::FormatOutput::ElementType ElementType;
	typedef typename protocol::FormatOutput::size_type size_type;
	typedef protocol::EscapingBuffer<textwolf::StaticBuffer> BufferType;

	///\class InputFilter
	///\brief input filter for single characters
	struct InputFilter :public protocol::InputFilter
	{
		enum ErrorCodes {Ok=0,ErrBufferTooSmall=1};

		virtual bool getNext( ElementType* type, void* buffer, size_type buffersize, size_type* bufferpos)
		{
			BufferType buf( (char*)buffer + *bufferpos, buffersize - *bufferpos);
			setState( Open);
			*type = Value;
			CharIterator itr( (char*)ptr(), size());
			textwolf::TextScanner<CharIterator,AppCharset> ts( itr);

			textwolf::UChar ch;
			if ((ch = *itr) != 0)
			{
				AppCharset::print( ch, buf);
				if (buf.overflow())
				{
					setState( protocol::InputFilter::Error, ErrBufferTooSmall);
					return false;
				}
				*bufferpos += buf.size();
				++itr;
				skip( itr.pos());
				return true;
			}
			if (!gotEoD()) setState( EndOfMessage);
			return false;
		}
	};
	///\class FormatOutput
	///\brief format output filter for single characters
	struct FormatOutput :public protocol::FormatOutput
	{
		virtual bool print( ElementType type, const void* element, size_type elementsize)
		{
			if (type == Value)
			{
				BufferType buf( rest(), restsize());
				ThisFilterBase::printToBuffer( (const char*)element, elementsize, buf);
				if (buf.overflow())
				{
					setState( EndOfBuffer);
					return false;
				}
				incPos( buf.size());
			}
			return true;
		}
	};
};

}}//namespace
#endif


