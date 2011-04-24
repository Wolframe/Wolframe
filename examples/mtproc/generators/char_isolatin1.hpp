#ifndef _Wolframe_PROTOCOL_GENERATOR_CHAR_ISOLATIN1_HPP_INCLUDED
#define _Wolframe_PROTOCOL_GENERATOR_CHAR_ISOLATIN1_HPP_INCLUDED
#include "protocol/generator.hpp"
#include "protocol/formatoutput.hpp"
#include <cstring>

namespace _Wolframe {
namespace mtproc {

namespace filter {

struct CharIsoLatin1
{
	struct FormatOutput :public protocol::FormatOutput
	{
		virtual bool print( ElementType, const void* element, size_type elementsize)
		{
			char* out = (char*)cur();
			size_type nn = restsize();

			if (elementsize > nn)
			{
				return false;
			}
			else
			{
				std::memcpy( out, element, elementsize);
				incr( elementsize);
				return true;
			}
		}
	};

	struct Generator :public protocol::Generator
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

}//namespace generator1
}}//namespace
#endif


