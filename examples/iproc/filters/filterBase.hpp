#ifndef _Wolframe_FILTER_BASE_HPP_INCLUDED
#define _Wolframe_FILTER_BASE_HPP_INCLUDED
#include "textwolf.hpp"

namespace _Wolframe {
namespace filter {

struct CharIterator
{
public:
	CharIterator( const char* src, unsigned int size) :m_src(src),m_size(size),m_pos(0){}
	char operator* () {return (m_pos < m_size)?m_src[m_pos]:0;}
	CharIterator& operator++() {m_pos++; return *this;}
	unsigned int pos() const {return m_pos;}

private:
	const char* m_src;
	unsigned int m_size;
	unsigned int m_pos;
};

template <class IOCharset, class AppCharset>
struct FilterBase
{
	typedef std::size_t size_type;

	template <class InputCharset>
	static size_type printToBuffer_( const char* src, unsigned int srcsize, char* buf, size_type bufsize)
	{
		size_type bufpos = 0;
		CharIterator itr( src, srcsize);
		textwolf::TextScanner<CharIterator,AppCharset> ts( itr);

		textwolf::UChar ch;
		while ((ch = ts.chr()) != 0)
		{
			size_type size = IOCharset::print( ch, buf+bufpos, bufsize-bufpos);
			if (size == 0) return 0;
			++ts;
			bufpos += size;
		}
		return bufpos;
	}

	template <class InputCharset>
	static size_type printToBuffer_( char ch, char* buf, size_type bufsize)
	{
		return printToBuffer_<InputCharset>( &ch, 1, buf, bufsize);
	}

	static size_type printToBuffer( const char* src, unsigned int srcsize, char* buf, size_type bufsize)
	{
		return printToBuffer_<AppCharset>( src, srcsize, buf, bufsize);
	}

	static size_type printToBuffer( char ch, char* buf, size_type bufsize)
	{
		return printToBuffer( &ch, 1, buf, bufsize);
	}

	static size_type printAsciiCharToBuffer( char ch, char* buf, size_type bufsize)
	{
		return printToBuffer_<textwolf::charset::IsoLatin1>( ch, buf, bufsize);
	}

	struct FormatOutputBase :public protocol::FormatOutput
	{
		virtual bool print( ElementType type, const void* element, size_type elementsize)
		{
			if (type == Value)
			{
				size_type bufpos=m_bufpos;
				if (!printElem( (const char*)element, elementsize, bufpos)) return false;

				if (!ContentOutputBlock::print( m_buf+m_bufpos, bufpos-m_bufpos))
				{
					setState( EndOfBuffer);
					return false;
				}
			}
			return true;
		}

		enum ErrorCodes {Ok,ErrConvBufferTooSmall, ErrTagStackExceedsLimit, ErrTagHierarchy, ErrIllegalState};

		FormatOutputBase( unsigned int bufsize) :m_buf(new char[bufsize]),m_bufsize(bufsize),m_bufpos(0){}

		static size_type getAlign( size_type n)
		{
			return (sizeof(size_type) - (n & (sizeof(size_type)-1))) & (sizeof(size_type)-1);
		}

		unsigned int push( const void* element, size_type elementsize)
		{
			size_type align = getAlign( elementsize);
			if (align + elementsize + sizeof(size_type) >= m_bufsize-m_bufpos) return 0;
			std::memcpy( m_buf + m_bufpos, element, elementsize);
			m_bufpos += elementsize + align + sizeof( size_type);
			*(size_type*)(m_buf+m_bufpos-sizeof( size_type)) = elementsize;
		}

		bool top( const void*& element, size_type& elementsize)
		{
			if (m_bufpos < sizeof( size_type)) return false;
			elementsize = *(size_type*)(m_buf+m_bufpos-sizeof( size_type));
			size_type align = getAlign( elementsize);
			if (align + elementsize + sizeof(size_type) > m_bufpos) return false;
			element = m_buf + m_bufpos - elementsize + align + sizeof( size_type);
			return true;
		}

		void pop()
		{
			size_type elementsize = *(size_type*)(m_buf+m_bufpos-sizeof( size_type));
			size_type align = getAlign( elementsize);
			m_bufpos -= elementsize + align + sizeof( size_type);
			if (m_bufpos >= m_bufsize) throw std::logic_error( "element stack is corrupt");
		}

		bool printElem( char ch, size_type& bufpos)
		{
			size_type nn = printAsciiCharToBuffer( '<', m_buf, m_bufsize);
			if (nn == 0)
			{
				setState( EndOfBuffer);
				return false;
			}
			bufpos += nn;
			return true;
		}

		bool printElem( const char* src, size_type& bufpos)
		{
			size_type nn = printToBuffer( src, strlen(src), m_buf+bufpos, m_bufsize-bufpos);
			if (nn == 0)
			{
				setState( EndOfBuffer);
				return false;
			}
			bufpos += nn;
			return true;
		}

		bool printElem( const char* src, unsigned int srcsize, size_type& bufpos)
		{
			size_type nn = printToBuffer( src, srcsize, m_buf+bufpos, m_bufsize-bufpos);
			if (nn == 0)
			{
				setState( EndOfBuffer);
				return false;
			}
			bufpos += nn;
			return true;
		}

		bool printOutput( size_type bufpos)
		{
			if (!ContentOutputBlock::print( m_buf+m_bufpos, bufpos-m_bufpos))
			{
				setState( Error, ErrConvBufferTooSmall);
				return false;
			}
			return true;
		}

		char* m_buf;
		size_type m_bufsize;
		size_type m_bufpos;
	};
};

}}//namespace
#endif


