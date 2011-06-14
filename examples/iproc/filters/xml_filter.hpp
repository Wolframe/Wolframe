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
	typedef FilterBase<IOCharset, AppCharset> ThisFilterBase;
	typedef typename protocol::FormatOutput::ElementType ElementType;
	typedef typename protocol::FormatOutput::size_type size_type;
	typedef textwolf::StaticBuffer BufferType;

	///\class FormatOutput
	///\brief format output filter for XML
	struct FormatOutput :public protocol::FormatOutput
	{
		enum {
			TagBufferSize=1024	///< default size of buffer use for storing tag hierarchy of output
		};

		///\enum ErrorCodes
		///\brief Enumeration of error codes
		enum ErrorCodes
		{
			Ok,				///< no error
			ErrTagStackExceedsLimit,	///< tack stack overflow
			ErrTagHierarchy,		///< tack hierarchy error
			ErrIllegalState			///< illegal state (should not happen)
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

		///\brief Constructor
		///\param [in] bufsize (optional) size of internal buffer to use (for the tag hierarchy stack)
		FormatOutput( unsigned int bufsize=TagBufferSize)
			:m_tagstk(new char[bufsize?bufsize:(unsigned int)TagBufferSize])
			,m_tagstksize(bufsize?bufsize:(unsigned int)TagBufferSize)
			,m_tagstkpos(0)
			,m_xmlstate(Content){}

		///\brief Implementation of protocol::InputFilter::print(ElementType,const void*,size_type)
		///\param [in] type type of the element to print
		///\param [in] element pointer to the element to print
		///\param [in] elementsize size of the element to print in bytes
		virtual bool print( ElementType type, const void* element, size_type elementsize)
		{
			BufferType buf( protocol::OutputBlock::rest(), protocol::OutputBlock::restsize());

			const void* cltag;
			size_type cltagsize;

			switch (type)
			{
				case protocol::FormatOutput::OpenTag:
					ThisFilterBase::printToBuffer( '<', buf);
					ThisFilterBase::printToBuffer( (const char*)element, elementsize, buf);

					if (buf.overflow())
					{
						setState( EndOfBuffer);
						return false;
					}
					if (!pushTag( element, elementsize))
					{
						setState( Error, ErrTagStackExceedsLimit);
						return false;
					}
					m_xmlstate = ((const char*)(element))[0]=='?'?Header:Tag;
					incPos( buf.size());
					setState( Open);
					return true;

				case protocol::FormatOutput::Attribute:
					ThisFilterBase::printToBuffer( (const char*)element, elementsize, buf);
					ThisFilterBase::printToBuffer( '=', buf);

					if (buf.overflow())
					{
						setState( EndOfBuffer);
						return false;
					}
					m_xmlstate = (m_xmlstate==Header)?HeaderAttribute:Attribute;
					incPos( buf.size());
					setState( Open);
					return true;

				case protocol::FormatOutput::Value:
					if (m_xmlstate == Attribute)
					{
						printToBufferAttributeValue( (const char*)element, elementsize, buf);
						if (buf.overflow())
						{
							setState( EndOfBuffer);
							return false;
						}
						m_xmlstate = Tag;
					}
					else if (m_xmlstate == HeaderAttribute)
					{
						printToBufferAttributeValue( (const char*)element, elementsize, buf);
						if (buf.overflow())
						{
							setState( EndOfBuffer);
							return false;
						}
						m_xmlstate = Header;
					}
					else
					{
						printToBufferContent( (const char*)element, elementsize, buf);
						if (buf.overflow())
						{
							setState( EndOfBuffer);
							return false;
						}
					}
					incPos( buf.size());
					setState( Open);
					return true;

				case protocol::FormatOutput::CloseTag:
					if (!topTag( cltag, cltagsize) || !cltagsize)
					{
						setState( Error, ErrTagHierarchy);
						return false;
					}
					if (m_xmlstate == Header)
					{
						printToBufferAttributeValue( (const char*)element, elementsize, buf);
						ThisFilterBase::printToBuffer( '?', buf);
						ThisFilterBase::printToBuffer( '>', buf);
					}
					else if (m_xmlstate == Tag)
					{
						ThisFilterBase::printToBuffer( '/', buf);
						ThisFilterBase::printToBuffer( '>', buf);
					}
					else
					{
						ThisFilterBase::printToBuffer( '<', buf);
						ThisFilterBase::printToBuffer( '/', buf);
						printToBufferAttributeValue( (const char*)element, elementsize, buf);
						ThisFilterBase::printToBuffer( '>', buf);
					}
					m_xmlstate = Content;
					popTag();
					incPos( buf.size());
					setState( Open);
					return true;
			}
			setState( Error, ErrIllegalState);
			return false;
		}
	private:
		///\brief print attribute value string
		///\param [in] src pointer to attribute value string to print
		///\param [in] srcsize size of src in bytes
		///\param [in,out] buf buffer to print to
		///\param [in] characters to escape
		static void printToBufferAttributeValue( const char* src, size_type srcsize, BufferType& buf)
		{
			CharIterator itr( src, srcsize);
			textwolf::TextScanner<CharIterator,AppCharset> ts( itr);

			textwolf::UChar ch;
			IOCharset::print( '\'', buf);
			while ((ch = ts.chr()) != 0)
			{
				if (ch == '&') ThisFilterBase::printToBuffer( "&amp;", 5, buf);
				else if (ch == '<') ThisFilterBase::printToBuffer( "&lt;", 4, buf);
				else if (ch == '>') ThisFilterBase::printToBuffer( "&gt;", 4, buf);
				else if (ch == '&') ThisFilterBase::printToBuffer( "&amp;", 5, buf);
				else if (ch == '\'') ThisFilterBase::printToBuffer( "&apos;", 6, buf);
				else if (ch == '\"') ThisFilterBase::printToBuffer( "&quot;", 6, buf);
				else IOCharset::print( ch, buf);
				++ts;
			}
			IOCharset::print( '\'', buf);
		}

		///\brief print content value string
		///\param [in] src pointer to content string to print
		///\param [in] srcsize size of src in bytes
		///\param [in,out] buf buffer to print to
		///\param [in] characters to escape
		static void printToBufferContent( const char* src, size_type srcsize, BufferType& buf)
		{
			CharIterator itr( src, srcsize);
			textwolf::TextScanner<CharIterator,AppCharset> ts( itr);

			textwolf::UChar ch;
			while ((ch = ts.chr()) != 0)
			{
				if (ch == '&') ThisFilterBase::printToBuffer( "&amp;", 5, buf);
				else if (ch == '<') ThisFilterBase::printToBuffer( "&lt;", 4, buf);
				else if (ch == '>') ThisFilterBase::printToBuffer( "&gt;", 4, buf);
				else if (ch == '&') ThisFilterBase::printToBuffer( "&amp;", 5, buf);
				else IOCharset::print( ch, buf);
				++ts;
			}
			IOCharset::print( ' ', buf);
		}

		static size_type getAlign( size_type n)
		{
			return (sizeof(size_type) - (n & (sizeof(size_type)-1))) & (sizeof(size_type)-1);
		}

		bool pushTag( const void* element, size_type elementsize)
		{
			size_type align = getAlign( elementsize);
			if (align + elementsize + sizeof(size_type) >= m_tagstksize-m_tagstkpos) return false;
			std::memcpy( m_tagstk + m_tagstkpos, element, elementsize);
			m_tagstkpos += elementsize + align + sizeof( size_type);
			*(size_type*)(m_tagstk+m_tagstkpos-sizeof( size_type)) = elementsize;
			return true;
		}

		bool topTag( const void*& element, size_type& elementsize)
		{
			if (m_tagstkpos < sizeof( size_type)) return false;
			elementsize = *(size_type*)(m_tagstk+m_tagstkpos-sizeof( size_type));
			size_type align = getAlign( elementsize);
			if (align + elementsize + sizeof(size_type) > m_tagstkpos) return false;
			element = m_tagstk + m_tagstkpos - elementsize + align + sizeof( size_type);
			return true;
		}

		void popTag()
		{
			size_type elementsize = *(size_type*)(m_tagstk+m_tagstkpos-sizeof( size_type));
			size_type align = getAlign( elementsize);
			m_tagstkpos -= elementsize + align + sizeof( size_type);
			if (m_tagstkpos >= m_tagstksize) throw std::logic_error( "element stack is corrupt");
		}
	private:
		char* m_tagstk;			///< tag stack buffer
		size_type m_tagstksize;		///< size of tag stack buffer in bytes
		size_type m_tagstkpos;		///< used size of tag stack buffer in bytes
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
			ErrBufferTooSmall,	///< output buffer is too small to hold the element
			ErrBrokenInputStream,	///< unexpected EoD
			ErrXML,			///< error in input XML
			ErrUnexpectedState	///< something unexpected happened,
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

		typedef textwolf::XMLScanner<Iterator,IOCharset,AppCharset,BufferType> XMLScanner;

		///\brief Constructor
		InputFilter() :m_outputbuf(1),m_scanner(0)
		{
			m_src = Iterator(this);
			m_scanner = new XMLScanner( m_src, m_outputbuf);
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
			BufferType buf( (char*)buffer + *bufferpos, buffersize - *bufferpos);
			m_scanner->setOutputBuffer( buf);
			try
			{
				setState( Open);
				++m_itr;
				if (buf.overflow())
				{
					setState( protocol::InputFilter::Error, ErrBufferTooSmall);
					return false;
				}
				*bufferpos += buf.size();
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
	private:
		BufferType m_outputbuf;			///< dummy buffer of size 1
		Iterator m_src;				///< source iterator
		XMLScanner* m_scanner;			///< XML scanner
		typename XMLScanner::iterator m_itr;	///< input iterator created from scanned XML from source iterator
		typename XMLScanner::iterator m_end;	///< end of data (EoD) pointer
	};
};


///\class XmlHeaderInputFilter
///\brief Input filter for the XML header only (returns EoD after the header)
struct XmlHeaderInputFilter :public XmlFilter<textwolf::charset::IsoLatin1,textwolf::charset::IsoLatin1>::InputFilter
{
	typedef textwolf::StaticBuffer BufferType;

	///\brief Constructor
	XmlHeaderInputFilter() {}
	///\brief Destructor
	virtual ~XmlHeaderInputFilter(){}

	///\brief Implementation of protocol::InputFilter::getNext( ElementType*, void*, size_type, size_type*)
	virtual bool getNext( ElementType* type, void* buffer, size_type buffersize, size_type* bufferpos)
	{
		BufferType buf( (char*)buffer + *bufferpos, buffersize - *bufferpos);
		m_scanner->setOutputBuffer( buf);
		try
		{
			setState( Open);
			++m_itr;
			if (buf.overflow())
			{
				setState( protocol::InputFilter::Error, ErrBufferTooSmall);
				return false;
			}
			*bufferpos += buf.size();
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


