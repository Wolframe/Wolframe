#ifndef _Wolframe_FILTER_XML_HPP_INCLUDED
#define _Wolframe_FILTER_XML_HPP_INCLUDED
#include "protocol/inputfilter.hpp"
#include "protocol/formatoutput.hpp"
#include "filters/filterBase.hpp"
#include <cstring>
#include "textwolf.hpp"

namespace _Wolframe {
namespace filter {

template <class IOCharset, class AppCharset=textwolf::charset::UTF8>
struct XmlFilter :public FilterBase<IOCharset,AppCharset>
{
	typedef typename FilterBase<IOCharset, AppCharset>::FormatOutputBase FormatOutputBase;
	typedef typename FormatOutputBase::ElementType ElementType;
	typedef typename FormatOutputBase::size_type size_type;

	struct FormatOutput :public FormatOutputBase
	{
		FormatOutput( unsigned int bufsize) :FormatOutputBase(bufsize),m_xmlstate(Content){}

		enum XMLState
		{
			Content,
			Tag,
			Attribute,
			Header,
			HeaderAttribute
		};

		virtual bool print( ElementType type, const void* element, size_type elementsize)
		{
			unsigned int prev;
			size_type bufpos = FormatOutputBase::m_bufpos;
			size_type nn;
			size_type pp;
			void* cltag;
			size_type cltagsize;

			switch (type)
			{
				case FormatOutputBase::OpenTag:
					if (!printElem( '<', bufpos) || !printElem( element, elementsize, bufpos)) return false;

					setState( FormatOutputBase::Open);
					m_xmlstate = ((const char*)(element))[0]=='?'?Header:Tag;

					if (!printOutput( bufpos)) return false;

					if (!push( element, elementsize))
					{
						setState( FormatOutputBase::Error, FormatOutputBase::ErrTagStackExceedsLimit);
						return false;
					}
					return true;

				case FormatOutputBase::Attribute:
					if (!printElem( element, elementsize, bufpos) || !printElem( '=', bufpos)) return false;

					setState( FormatOutputBase::Open);
					m_xmlstate = (m_xmlstate==Header)?HeaderAttribute:Attribute;
					return printOutput( bufpos);

				case FormatOutputBase::Value:
					if (m_xmlstate == Attribute)
					{
						if (!printElem( '\'', bufpos) || !printElem( element, elementsize, bufpos) || !printElem( '\'', bufpos)) return false;
						m_xmlstate = Tag;
					}
					else if (m_xmlstate == HeaderAttribute)
					{
						if (!printElem( '\'', bufpos) || !printElem( element, elementsize, bufpos) || !printElem( '\'', bufpos)) return false;
						m_xmlstate = Header;
					}
					else
					{
						if (!printElem( element, elementsize, bufpos)) return false;
					}
					return printOutput( bufpos);

				case FormatOutputBase::CloseTag:
					if (!top( cltag, cltagsize) || !cltagsize)
					{
						setState( FormatOutputBase::Error, FormatOutputBase::ErrTagHierarchy);
						return false;
					}
					if (m_xmlstate == Header)
					{
						if (!printElem( (char*)cltag+1, cltagsize-1, bufpos) || !printElem( "?>", bufpos)) return false;
					}
					else if (m_xmlstate == Tag)
					{
						if (!printElem( "/>", bufpos)) return false;
					}
					else
					{
						if (!printElem( "</", bufpos) || !printElem( cltag, cltagsize, bufpos) || !printElem( '>', bufpos)) return false;
					}
					if (!printOutput( bufpos)) return false;
					m_xmlstate = Content;
					FormatOutputBase::pop();
					return true;
			}
			setState( FormatOutputBase::Error, FormatOutputBase::ErrIllegalState);
			return false;
		}
	private:
		XMLState m_xmlstate;
	};

	struct InputFilter :public protocol::InputFilter
	{
		enum ErrorCodes {Ok,ErrBrokenInputStream,ErrXML,ErrUnexpectedState};
		struct EndOfMessageException {};

		struct Iterator
		{
			InputFilter* m_gen;

			Iterator() :m_gen(0) {}
			Iterator( InputFilter* gen) :m_gen(gen) {}
			Iterator( const Iterator& o) :m_gen(o.m_gen) {}

			char operator*()
			{
				if (m_gen->m_pos >= m_gen->m_size) throw EndOfMessageException();
				return ((char*)m_gen->m_ptr)[ m_gen->m_pos];
			}

			Iterator& operator++()
			{
				m_gen->m_pos++;
			}
		};

		typedef textwolf::XMLScanner<Iterator,IOCharset,AppCharset> XMLScanner;
		char m_outputbuf;
		Iterator m_itr;
		XMLScanner* m_scanner;

		InputFilter() :m_scanner(0)
		{
			m_itr = Iterator(this);
			m_scanner = new XMLScanner( m_itr, &m_outputbuf, 1);
		}

		~InputFilter()
		{
			delete m_scanner;
		}

		virtual bool getNext( ElementType* type, void* buffer, size_type buffersize, size_type* bufferpos)
		{
			m_scanner->setOutputBuffer( (char*)buffer + *bufferpos, buffersize - *bufferpos);
			try
			{
				setState( Open);
				m_itr++;
				*bufferpos += m_itr->size();
				switch (m_itr->type())
				{
					case textwolf::XMLScannerBase::None: setState( Error, ErrBrokenInputStream); return false;
					case textwolf::XMLScannerBase::ErrorOccurred: setState( Error, ErrXML); return false;
					case textwolf::XMLScannerBase::HeaderAttribName: *type = Attribute; return true;
					case textwolf::XMLScannerBase::HeaderAttribValue: *type = Value; return true;
					case textwolf::XMLScannerBase::HeaderEnd: *type = CloseTag; return true;
					case textwolf::XMLScannerBase::TagAttribName: *type = Attribute; return true;
					case textwolf::XMLScannerBase::TagAttribValue: *type = Value; return true;
					case textwolf::XMLScannerBase::OpenTag: *type = OpenTag; return true;
					case textwolf::XMLScannerBase::CloseTag: *type = CloseTag; return true;
					case textwolf::XMLScannerBase::CloseTagIm: *type = CloseTag; return true;
					case textwolf::XMLScannerBase::Content: *type = Value; return true;
					case textwolf::XMLScannerBase::Exit: *type = CloseTag; return true;
				}
			}
			catch (EndOfMessageException)
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


