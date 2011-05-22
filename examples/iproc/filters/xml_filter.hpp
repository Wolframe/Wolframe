#ifndef _Wolframe_FILTER_CHAR_ISOLATIN1_HPP_INCLUDED
#define _Wolframe_FILTER_CHAR_ISOLATIN1_HPP_INCLUDED
#include "protocol/inputfilter.hpp"
#include "protocol/formatoutput.hpp"
#include <cstring>
#include "textwolf.hpp"

namespace _Wolframe {
namespace filter {

template <struct Charset_>
struct XmlFilterT
{
	static size_type printToBuffer( const char* src, unsigned int srcsize, char* buf, size_type bufsize)
	{
		size_type bufpos = 0;
		struct iterator
		{
			const char* m_src;
			unsigned int m_size;
			unsigned int m_pos;

			iterator( const char* src, unsigned int size) :m_src(src),m_size(size),m_pos(0){}
			char operator* () {return (m_pos < m_size)?m_src[m_pos]:0;}
			iterator& operator++() {m_pos++; return *this;}
		};

		iterator itr( src, srcsize);
		TextScanner<iterator,textwolf::charset::UTF8> ts( itr);

		textwol::UChar ch;
		while ((ch = *itr) != 0)
		{
			size_type size = Charset_::print( ch, buffer+bufpos, sizeof(buf)-bufpos);
			if (size == 0) return 0;
			bufpos += size;
		}
		return bufpos;
	}

	static size_type printCharToBuffer( char ch, char* buf, size_type bufsize)
	{
		return printToBuffer( &ch, 1, buf, bufsize);
	}

	struct FormatOutput :public protocol::FormatOutput
	{
		enum ErrorCodes {Ok,ErrConvBufferTooSmall};

		FormatOutput( unsigned int bufsize) :m_xmlstate(Content),m_buf(new char[bufsSize]),m_bufsize(bufsize),m_bufpos(0){}

		enum XMLState
		{
			Content,
			Tag,
			Attribute,
			Header,
			HeaderAttribute
		};
		XMLState m_xmlstate;

		virtual bool print( ElementType type, const void* element, size_type elementsize)
		{
			size_type bufpos = m_bufpos;
			size_type nn;
			size_type pp;

			switch (type)
			{
				case OpenTag:
					nn = printCharToBuffer( '<', m_buf, m_bufsize);
					if (nn == 0)
					{
						setState( EndOfBuffer);
						return false;
					}
					bufpos += nn;
					pp = bufpos;
					nn = printToBuffer( (const char*)element, elementsize, m_buf+bufpos, m_bufsize-bufpos);
					if (nn == 0)
					{

						setState( EndOfBuffer);
						return false;
					}
					bufpos += nn;
					setState( Open);
					m_xmlstate = ((const char*)(element))[0]=='?'?Header:Tag;

					if (!ContentOutputBlock::print( m_buf+m_bufpos, bufpos))
					{
						setState( Error, ErrConvBufferTooSmall);
						return false;
					}
					m_tagstack.push_back( pp);
					m_bufpos = bufpos;
					return true;

				case Attribute:
					nn = printToBuffer( (const char*)element, elementsize, m_buf+bufpos, m_bufsize-bufpos);
					if (nn == 0)
					{
						setState( EndOfBuffer);
						return false;
					}
					bufpos += nn;
					nn = printCharToBuffer( '=', m_buf, m_bufsize);
					if (nn == 0)
					{
						setState( EndOfBuffer);
						return false;
					}
					bufpos += nn;
					setState( Open);
					m_xmlstate = (m_xmlstate==Header)?HeaderAttribute:Attribute;

					if (!ContentOutputBlock::print( m_buf, bufpos))
					{
						setState( Error, ErrConvBufferTooSmall);
						return false;
					}
					return true;

				case Value:
					if (m_xmlstate == Attribute || m_xmlstate == HeaderAttribute)
					{
						nn = printCharToBuffer( '\'', m_buf, m_bufsize);
						if (nn == 0)
						{
							setState( EndOfBuffer);
							return false;
						}
						bufpos += nn;
						nn = printToBuffer( (const char*)element, elementsize, m_buf+bufpos, m_bufsize-bufpos);
						if (nn == 0)
						{
							setState( EndOfBuffer);
							return false;
						}
						bufpos += nn;
						nn = printCharToBuffer( '\'', m_buf, m_bufsize);
						if (nn == 0)
						{
							setState( EndOfBuffer);
							return false;
						}
						bufpos += nn;
						m_xmlstate = (m_xmlstate == Attribute)?Tag:Header;
					}
					else
					{
						nn = printToBuffer( (const char*)element, elementsize, m_buf+bufpos, m_bufsize-bufpos);
						if (nn == 0)
						{
							setState( EndOfBuffer);
							return false;
						}
						bufpos += nn;
					}
				case CloseTag:
					if (m_xmlstate == Content)
					{
						nn = printCharToBuffer( '<', m_buf+m_bufpos, m_bufsize-m_bufpos);
						if (nn == 0)
						{
							setState( EndOfBuffer);
							return false;
						}
						bufpos += nn;
						nn = printCharToBuffer( '/', m_buf+m_bufpos, m_bufsize-m_bufpos);
						if (nn == 0)
						{
							setState( EndOfBuffer);
							return false;
						}
						bufpos += nn;
						if (!ContentOutputBlock::print( m_buf+m_bufpos, bufpos))
						{
							setState( Error, ErrConvBufferTooSmall);
							return false;
						}
						bufpos = m_tagstack.top();
						if (!ContentOutputBlock::print( m_buf+bufpos, m_bufpos-bufpos))
						{
							setState( Error, ErrConvBufferTooSmall);
							return false;
						}
						m_bufpos = bufpos;
						m_tagstack.pop_back();
						return true;


					}
					else if (m_xmlstate == Tag)
					{
						nn = printCharToBuffer( '/', m_buf, m_bufsize);
						if (nn == 0)
						{
							setState( EndOfBuffer);
							return false;
						}
						bufpos += nn;
						nn = printCharToBuffer( '>', m_buf, m_bufsize);
						if (nn == 0)
						{
							setState( EndOfBuffer);
							return false;
						}
						bufpos += nn;
					}
					else if (m_xmlstate == Header)
					{
						nn = printCharToBuffer( '?', m_buf, m_bufsize);
						if (nn == 0)
						{
							setState( EndOfBuffer);
							return false;
						}
						bufpos += nn;
						nn = printCharToBuffer( '>', m_buf, m_bufsize);
						if (nn == 0)
						{
							setState( EndOfBuffer);
							return false;
						}
						bufpos += nn;

						if (!ContentOutputBlock::print( m_buf, bufpos))
						{
							setState( Error, ErrConvBufferTooSmall);
							return false;
						}
					}
					setState( Open);
					m_xmlstate = Content;
					return true;
			}
			setState( Open);
			if (!ContentOutputBlock::print( element, elementsize))
			{
				setState( EndOfBuffer);
				return false;
			}
			return true;
		}
	private:
		char* m_buf;
		size_type m_bufsize;
		size_type m_bufpos;
		std::vector<size_type> m_tagstack;
	};

	struct InputFilter :public protocol::InputFilter
	{
		enum ErrorCodes {Ok,ErrBrokenInputStream,ErrXML,ErrUnexpectedState};

		struct InputIterator
		{
			struct EOM{};
			InputFilter* m_gen;

			InputIterator() :m_gen(0) {}
			InputIterator( InputFilter* gen) :m_gem(gen){}
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

		InputFilter() :scanner(0)
		{
			itr = InputIterator(this);
			scanner = new XMLScanner( itr, &m_outputbuf, 1);
		}

		~InputFilter()
		{
			delete scanner;
		}

		virtual bool getNext( ElementType* type, void* buffer, size_type buffersize, size_type* bufferpos)
		{
			scanner->setOutputBuffer( (char*)buffer + *bufferpos, buffersize - *bufferpos);
			try
			{
				setState( Open);
				itr++;
				*bufferpos += itr->size();
				switch (itr->type())
				{
					case None: setState( Error, ErrBrokenInputStream); return false;
					case ErrorOccurred: setState( Error, ErrXML); return false;
					case HeaderAttribName: *type = Attribute; return true;
					case HeaderAttribValue: *type = Value; return true;
					case HeaderEnd: *type = CloseTag; return true;
					case TagAttribName: *type = Attribute; return true;
					case TagAttribValue: *type = Value; return true;
					case OpenTag: *type = OpenTag; return true;
					case CloseTag: *type = CloseTag; return true;
					case CloseTagIm: *type = CloseTag; return true;
					case Content: *type = Value; return true;
					case Exit: *type = CloseTag; return true;
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
			setState( Error, ErrUnexpectedState);
			return false;
		}
	};
};

}}//namespace
#endif


