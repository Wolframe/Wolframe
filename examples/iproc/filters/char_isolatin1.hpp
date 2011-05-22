#ifndef _Wolframe_FILTER_CHAR_ISOLATIN1_HPP_INCLUDED
#define _Wolframe_FILTER_CHAR_ISOLATIN1_HPP_INCLUDED
#include "protocol/inputfilter.hpp"
#include "protocol/formatoutput.hpp"
#include <cstring>

namespace _Wolframe {
namespace filter {

struct CharIsoLatin1
{
	struct FormatOutput :public protocol::FormatOutput
	{
		virtual bool print( ElementType, const void* element, size_type elementsize)
		{
			if (elementsize > restsize())
			{
				setState( EndOfBuffer);
				return false;
			}
			else
			{
				setState( Open);
				ContentOutputBlock::print( element, elementsize);
				incr( elementsize);
				return true;
			}
		}
	};

	struct InputFilter :public protocol::InputFilter
	{
		enum ErrorCodes {Ok=0,ErrBufferTooSmall=1};

		virtual bool getNext( ElementType* type, void* buffer, size_type buffersize, size_type* bufferpos)
		{
			char* in = (char*)ptr();
			size_type nn = size();
			*type = Value;

			if (buffersize == *bufferpos)
			{
				setState( Error, ErrBufferTooSmall);
				return false;
			}
			else if (nn == 0)
			{
				if (gotEoD())
				{
					setState( Open);
					return false;
				}
				else
				{
					setState( EndOfMessage);
					return false;
				}
			}
			else
			{
				setState( Open);
				((char*)buffer)[*bufferpos] = *in;
				skip( 1);
				*bufferpos += 1;
				return true;
			}
		}
	};
};

}}//namespace
#endif


