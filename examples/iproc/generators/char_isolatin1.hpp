#ifndef _Wolframe_PROTOCOL_GENERATOR_CHAR_ISOLATIN1_HPP_INCLUDED
#define _Wolframe_PROTOCOL_GENERATOR_CHAR_ISOLATIN1_HPP_INCLUDED
#include "protocol/generator.hpp"
#include "protocol/formatoutput.hpp"
#include <cstring>

#if 0 //only temporarily here
#include "textwolf.hpp"
#endif


namespace _Wolframe {
namespace filter {

#if 0 //only temporarily here
template <struct Charset_>
struct XmlFilterT
{
	struct Generator //[+]:public protocol::Generator
	{
		enum ErrorCodes {Ok=0,ErrBufferTooSmall=1,ErrBrokenInputStream=2};

		struct InputIterator
		{
			struct EOM{};
			Generator* m_gen;

			InputIterator() :m_gen(0) {}
			InputIterator( Generator* gen) :m_gem(gen){}
			InputIterator( const InputIterator& o) :m_gem(o.m_gen){}

			char operator*()
			{
				if (m_gen->m_pos >= m_gen->m_size) throw EOM();
				return ((char*)m_gen->m_ptr)[ m_gen->m_pos];
			}
			InputIterator& operator++()
			{
				m_gen->m_pos++;
			}
		};
		typedef textwolf::XMLScanner<InputIterator,Charset_,charset::UTF8> XMLScanner;
		char m_outputbuf;
		InputIterator itr;
		XMLScanner* scanner;

		Generator() :scanner(0)
		{
			itr = InputIterator(this);
			scanner = new XMLScanner( itr, &m_outputbuf, 1);
		}

		~Generator()
		{
			delete scanner;
		}

		virtual bool getNext( ElementType* type, void* buffer, size_type buffersize, size_type* bufferpos)
		{
			scanner->setOutputBuffer( (char*)buffer + *bufferpos, buffersize - *bufferpos);
			try
			{
				itr++;
				*bufferpos += itr->size();
				switch (itr->type())
				{
					case ... *type = ... break;
					case ... *type = ... break;
					case EOF
                                        case Error: setState( Error, errorcode);
				}
			}
			catch (InputIterator::EOM)
			{
				if (gotEoD())
				{
					setState( Error, ErrBrokenInputStream);
					return false;
				}
				else
				{
					setState( EndOfMessage);
					return false;
				}
			};
			if (EOF)
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
			return true;
		}
	};
};
#endif

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

}}//namespace
#endif


