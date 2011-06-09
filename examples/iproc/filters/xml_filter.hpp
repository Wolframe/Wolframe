#ifndef _Wolframe_FILTER_XML_HPP_INCLUDED
#define _Wolframe_FILTER_XML_HPP_INCLUDED
#include "protocol/inputfilter.hpp"
#include "protocol/formatoutput.hpp"
#include "filters/filterBase.hpp"
#include <cstring>
#include "textwolf.hpp"

namespace _Wolframe {
namespace filter {

class XmlHeaderFilter;		///< forward declaration for declaring friends
class XmlHeaderInputFilter;	///< forward declaration for declaring friends


///\class XmlFilter
///\brief XML filter template
///\tparam IOCharset character set encoding of input and output
///\tparam AppCharset character set encoding of the application processor
template <class IOCharset, class AppCharset=textwolf::charset::UTF8>
struct XmlFilter :public FilterBase<IOCharset,AppCharset>
{
	friend class XmlHeaderFilter;
	typedef typename FilterBase<IOCharset, AppCharset>::FormatOutputBase FormatOutputBase;
	typedef typename FormatOutputBase::ElementType ElementType;
	typedef typename FormatOutputBase::size_type size_type;

	///\class FormatOutput
	///\brief format output filter for XML
	struct FormatOutput :public FormatOutputBase
	{
		enum {
			TagBufferSize=1024	///< default size of buffer use for storing tag hierarchy of output
		};
		///\enum XMLState
		///\brief Enumeration of XML printer states
		enum XMLState
		{
			Content,		///< processing content
			Tag,			///< processing inside a tag defintion
			Attribute,		///< processing inside a tag defintion, just read an attribute
			Header,			///< processing inside a header defintion
			HeaderAttribute		///< processing inside a header defintion, just read an attribute
		};

		/// \brief Constructor
		/// \param [in] bufsize (optional) size of internal buffer to use (for the tag hierarchy stack)
		FormatOutput( unsigned int bufsize=TagBufferSize)

			:FormatOutputBase(bufsize?bufsize:(unsigned int)TagBufferSize)
			,m_xmlstate(Content){}

		///\brief Implementation of protocol::InputFilter::print(ElementType,const void*,size_type)
		///\param [in] type type of the element to print
		///\param [in] element pointer to the element to print
		///\param [in] elementsize size of the element to print in bytes
		virtual bool print( ElementType type, const void* element, size_type elementsize)
		{
			size_type bufpos = FormatOutputBase::m_bufpos;
			const void* cltag;
			size_type cltagsize;

			switch (type)
			{
				case FormatOutputBase::OpenTag:
					if (!printElem( '<', bufpos) || !printElem( (const char*)element, elementsize, bufpos)) return false;

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
					if (!printElem( (const char*)element, elementsize, bufpos) || !printElem( '=', bufpos)) return false;

					setState( FormatOutputBase::Open);
					m_xmlstate = (m_xmlstate==Header)?HeaderAttribute:Attribute;
					return printOutput( bufpos);

				case FormatOutputBase::Value:
					if (m_xmlstate == Attribute)
					{
						if (!printElem( '\'', bufpos) || !printElem( (const char*)element, elementsize, bufpos) || !printElem( '\'', bufpos)) return false;
						m_xmlstate = Tag;
					}
					else if (m_xmlstate == HeaderAttribute)
					{
						if (!printElem( '\'', bufpos) || !printElem( (const char*)element, elementsize, bufpos) || !printElem( '\'', bufpos)) return false;
						m_xmlstate = Header;
					}
					else
					{
						if (!printElem( ' ', bufpos) || !printElem( (const char*)element, elementsize, bufpos)) return false;
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
						if (!printElem( (const char*)cltag+1, cltagsize-1, bufpos) || !printElem( "?>", bufpos)) return false;
					}
					else if (m_xmlstate == Tag)
					{
						if (!printElem( "/>", bufpos)) return false;
					}
					else
					{
						if (!printElem( "</", bufpos) || !printElem( (const char*)cltag, cltagsize, bufpos) || !printElem( '>', bufpos)) return false;
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
		XMLState m_xmlstate;		///< current state of output
	};

	///\class InputFilter
	///\brief input filter for XML
	struct InputFilter :public protocol::InputFilter
	{
		friend class XmlHeaderInputFilter;
		///\enum ErrorCodes
		///\brief Enumeration of error codes
		enum ErrorCodes
		{
			Ok,			///< no error
			ErrBrokenInputStream,	///< unexpected EoD
			ErrXML,			///< error in input XML
			ErrUnexpectedState	///< something unexpected happened
		};
		///\class EndOfMessageException
		///\brief Exception thrown when EoM is reached and more data has to be read from input
		struct EndOfMessageException {};

		///\class Iterator
		///\brief Input iterator as source for the XML scanner (throws EndOfMessageException on EoM)
		struct Iterator
		{
			InputFilter* m_gen;	///< input for the iterator (from network message)

			///\brief Empty constructor
			Iterator() :m_gen(0) {}
			///\brief Constructor
			Iterator( InputFilter* gen) :m_gen(gen) {}
			///\brief Copy constructor
			Iterator( const Iterator& o) :m_gen(o.m_gen) {}

			///\brief access operator (required by textwolf for an input iterator)
			char operator*()
			{
				if (!m_gen->size()) throw EndOfMessageException();
				return *(char*)m_gen->ptr();
			}

			///\brief prefix increment operator (required by textwolf for an input iterator)
			Iterator& operator++()
			{
				m_gen->skip(1);
				return *this;
			}
		};

		typedef textwolf::XMLScanner<Iterator,IOCharset,AppCharset> XMLScanner;
		char m_outputbuf;			///< dummy buffer of size 1
		Iterator m_src;				///< source iterator
		XMLScanner* m_scanner;			///< XML scanner
		typename XMLScanner::iterator m_itr;	///< input iterator created from scanned XML from source iterator
		typename XMLScanner::iterator m_end;	///< end of data (EoD) pointer

		///\brief Constructor
		InputFilter() :m_scanner(0)
		{
			m_src = Iterator(this);
			m_scanner = new XMLScanner( m_src, &m_outputbuf, 1);
			m_itr = m_scanner->begin();
			m_end = m_scanner->end();
		}

		///\brief Destructor
		virtual ~InputFilter()
		{
			delete m_scanner;
		}

		///\brief Implementation of protocol::InputFilter::getNext( ElementType*, void*, size_type, size_type*)
		virtual bool getNext( ElementType* type, void* buffer, size_type buffersize, size_type* bufferpos)
		{
			m_scanner->setOutputBuffer( (char*)buffer + *bufferpos, buffersize - *bufferpos);
			try
			{
				setState( Open);
				++m_itr;
				*bufferpos += m_itr->size();
				switch (m_itr->type())
				{
					case textwolf::XMLScannerBase::None: setState( Error, ErrBrokenInputStream); return false;
					case textwolf::XMLScannerBase::ErrorOccurred: setState( Error, ErrXML); return false;
					case textwolf::XMLScannerBase::HeaderAttribName: *type = Attribute; return true;
					case textwolf::XMLScannerBase::HeaderAttribValue: *type = Value; return true;
					case textwolf::XMLScannerBase::HeaderEnd: *type = CloseTag; return true;
					case textwolf::XMLScannerBase::TagAttribName: setState( Error, ErrXML); return false;
					case textwolf::XMLScannerBase::TagAttribValue: setState( Error, ErrXML); return false;
					case textwolf::XMLScannerBase::OpenTag: setState( Error, ErrXML); return false;
					case textwolf::XMLScannerBase::CloseTag: setState( Error, ErrXML); return false;
					case textwolf::XMLScannerBase::CloseTagIm: setState( Error, ErrXML); return false;
					case textwolf::XMLScannerBase::Content: setState( Error, ErrXML); return false;
					case textwolf::XMLScannerBase::Exit: setState( Error, ErrBrokenInputStream); return false;
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


///\class XmlHeaderInputFilter
///\brief Input filter for the XML header only (returns EoD after the header)
struct XmlHeaderInputFilter :public XmlFilter<textwolf::charset::IsoLatin1,textwolf::charset::IsoLatin1>::InputFilter
{
	///\brief Constructor
	XmlHeaderInputFilter() {}
	///\brief Destructor
	virtual ~XmlHeaderInputFilter(){}

	///\brief Implementation of protocol::InputFilter::getNext( ElementType*, void*, size_type, size_type*)
	virtual bool getNext( ElementType* type, void* buffer, size_type buffersize, size_type* bufferpos)
	{
		m_scanner->setOutputBuffer( (char*)buffer + *bufferpos, buffersize - *bufferpos);
		try
		{
			setState( Open);
			++m_itr;
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

///\class XmlHeaderFilter 
///\brief Filter for the XML header only
struct XmlHeaderFilter :public XmlFilter<textwolf::charset::IsoLatin1,textwolf::charset::IsoLatin1>
{
	typedef XmlFilter<textwolf::charset::IsoLatin1,textwolf::charset::IsoLatin1> XmlFilterBase;
	typedef XmlFilterBase::FormatOutput FormatOutput;
	typedef XmlHeaderInputFilter InputFilter;
};


}}//namespace
#endif


