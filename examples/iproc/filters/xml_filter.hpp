#ifndef _Wolframe_FILTER_XML_HPP_INCLUDED
#define _Wolframe_FILTER_XML_HPP_INCLUDED
#include "protocol/inputfilter.hpp"
#include "protocol/formatoutput.hpp"
#include "filters/filterBase.hpp"
#include <cstring>
#include <iostream>
#include "textwolf.hpp"

namespace _Wolframe {
namespace filter {

// Aba: changed to structs, also the friends below. Windows CL doesn't like
// mixing 'struct' and 'class', e.g.
// warning C4099: '_Wolframe::filter::XmlHeaderInputFilter' : type name first
// seen using 'struct' now seen using 'class'
struct XmlHeaderFilter;		///< forward declaration for declaring friends
struct XmlHeaderInputFilter;	///< forward declaration for declaring friends


///\class XmlFilter
///\brief XML filter template
///\tparam IOCharset character set encoding of input and output
///\tparam AppCharset character set encoding of the application processor
template <class IOCharset, class AppCharset=textwolf::charset::UTF8>
struct XmlFilter :public FilterBase<IOCharset,AppCharset>
{
	friend struct XmlHeaderFilter;
	typedef FilterBase<IOCharset, AppCharset> ThisFilterBase;
	typedef typename protocol::FormatOutput::ElementType ElementType;
	typedef typename protocol::FormatOutput::size_type size_type;
	typedef protocol::EscapingBuffer<textwolf::StaticBuffer> EscBufferType;
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
			,m_xmlstate(Tag)
			,m_bufstate(EscBufferType::SRC){}

		///\brief Implementation of protocol::InputFilter::print(ElementType,const void*,size_type)
		///\param [in] type type of the element to print
		///\param [in] element pointer to the element to print
		///\param [in] elementsize size of the element to print in bytes
		virtual bool print( ElementType type, const void* element, size_type elementsize)
		{
			EscBufferType buf( rest(), restsize(), m_bufstate);

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
					m_bufstate = buf.state();
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
					m_bufstate = buf.state();
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
						if (m_xmlstate == Content) IOCharset::print( ' ', buf);

						printToBufferContent( (const char*)element, elementsize, buf);
						if (buf.overflow())
						{
							setState( EndOfBuffer);
							return false;
						}
						m_xmlstate = Content;
					}
					incPos( buf.size());
					setState( Open);
					m_bufstate = buf.state();
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
					m_bufstate = buf.state();
					return true;
			}
			setState( Error, ErrIllegalState);
			return false;
		}
	private:
		///\brief print a character substitute or the character itself
		///\param [in,out] buf buffer to print to
		///\param [in] nof_echr number of elements in echr and estr
		///\param [in] echr ASCII characters to substitute
		///\param [in] estr ASCII strings to substitute with (array parallel to echr)
		static void printEsc( char ch, EscBufferType& buf, unsigned int nof_echr, const char* echr, const char** estr)
		{
			const char* cc = (const char*)memchr( echr, ch, nof_echr);
			if (cc) 
			{
				unsigned int ii = 0;
				const char* tt = estr[ cc-echr];
				while (cc[ii]) IOCharset::print( tt[ii++], buf);
			}
			else
			{
				IOCharset::print( ch, buf);
			}
		}

		///\brief print a value with some characters replaced by a string
		///\param [in] src pointer to attribute value string to print
		///\param [in] srcsize size of src in bytes
		///\param [in,out] buf buffer to print to
		///\param [in] nof_echr number of elements in echr and estr
		///\param [in] echr ASCII characters to substitute
		///\param [in] estr ASCII strings to substitute with (array parallel to echr)
		static void printToBufferSubstChr( const char* src, size_type srcsize, EscBufferType& buf, unsigned int nof_echr, const char* echr, const char** estr)
		{
			StrIterator itr( src, srcsize);
			textwolf::TextScanner<StrIterator,AppCharset> ts( itr);

			textwolf::UChar ch;
			while ((ch = ts.chr()) != 0)
			{
				if (ch < 128)
				{
					printEsc( (char)ch, buf, nof_echr, echr, estr);
				}
				else
				{
					IOCharset::print( ch, buf);
				}
				++ts;
			}
		}
		///\brief print attribute value string
		///\param [in] src pointer to attribute value string to print
		///\param [in] srcsize size of src in bytes
		///\param [in,out] buf buffer to print to
		static void printToBufferAttributeValue( const char* src, size_type srcsize, EscBufferType& buf)
		{
			enum {nof_echr = 12};
			static const char* estr[nof_echr] = {"&lt;", "&gt;", "&apos;", "&quot;", "&amp;", "&#0;", "&#8;", "&#9;", "&#10;", "&#13;", "&nbsp;"};
			static const char echr[nof_echr+1] = "<>'\"&\0\b\t\n\r ";
			IOCharset::print( '\'', buf);
			printToBufferSubstChr( src, srcsize, buf, nof_echr, echr, estr);
			IOCharset::print( '\'', buf);
		}

		///\brief print content value string
		///\param [in] src pointer to content string to print
		///\param [in] srcsize size of src in bytes
		///\param [in,out] buf buffer to print to
		static void printToBufferContent( const char* src, size_type srcsize, EscBufferType& buf)
		{
			enum {nof_echr = 10};
			static const char* estr[nof_echr] = {"&lt;", "&gt;", "&amp;", "&#0;", "&#8;", "&#9;", "&#10;", "&#13;", "&nbsp;"};
			static const char echr[nof_echr+1] = "<>&\0\b\t\n\r ";
			printToBufferSubstChr( src, srcsize, buf, nof_echr, echr, estr);			
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
		char* m_tagstk;				///< tag stack buffer
		size_type m_tagstksize;			///< size of tag stack buffer in bytes
		size_type m_tagstkpos;			///< used size of tag stack buffer in bytes
		XMLState m_xmlstate;			///< current state of output
		typename EscBufferType::State m_bufstate;	///< state of escaping the output
	};

	///\class InputFilter
	///\brief input filter for XML
	struct InputFilter :public protocol::InputFilter
	{
		friend struct XmlHeaderInputFilter;
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

		typedef textwolf::XMLScanner<SrcIterator,IOCharset,AppCharset,BufferType> XMLScanner;

		///\brief Constructor
		InputFilter() :m_outputbuf(0,0),m_scanner(0)
		{
			m_src = SrcIterator(this);
			m_scanner = new XMLScanner( m_src, m_outputbuf);
			m_itr = m_scanner->begin(false);
			m_end = m_scanner->end();
		}

		///\brief Destructor
		virtual ~InputFilter()
		{
			delete m_scanner;
		}

		struct ElementTypeMap :public textwolf::CharMap<int,-1,textwolf::XMLScannerBase::NofElementTypes>
		{
			ElementTypeMap()
			{
				(*this)
				(textwolf::XMLScannerBase::None,-1)
				(textwolf::XMLScannerBase::ErrorOccurred,-1)
				(textwolf::XMLScannerBase::HeaderAttribName,(int)Attribute)
				(textwolf::XMLScannerBase::HeaderAttribValue,(int)Value)
				(textwolf::XMLScannerBase::HeaderEnd,(int)CloseTag)
				(textwolf::XMLScannerBase::TagAttribName,(int)Attribute)
				(textwolf::XMLScannerBase::TagAttribValue,(int)Value)
				(textwolf::XMLScannerBase::OpenTag,(int)OpenTag)
				(textwolf::XMLScannerBase::CloseTag,(int)CloseTag)
				(textwolf::XMLScannerBase::CloseTagIm,(int)CloseTag)
				(textwolf::XMLScannerBase::Content,(int)Value)
				(textwolf::XMLScannerBase::Exit,(int)CloseTag);
			}
		};

		///\brief Implementation of protocol::InputFilter::getNext( ElementType*, void*, size_type, size_type*)
		virtual bool getNext( ElementType* type, void* buffer, size_type buffersize, size_type* bufferpos)
		{
			static const ElementTypeMap tmap;
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
				int st = tmap[ m_itr->type()];
				if (st == -1)
				{
					setState( Error, ErrXML);
					return false;
				}
				else
				{
					*type = (ElementType)st;
					return true;
				}
			}
			catch (SrcIterator::EoM)
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
		SrcIterator m_src;			///< source iterator
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

	struct ElementTypeMap :public textwolf::CharMap<int,-1,textwolf::XMLScannerBase::NofElementTypes>
	{
		ElementTypeMap()
		{
			(*this)
			(textwolf::XMLScannerBase::None,-1)
			(textwolf::XMLScannerBase::ErrorOccurred,-1)
			(textwolf::XMLScannerBase::HeaderAttribName,(int)Attribute)
			(textwolf::XMLScannerBase::HeaderAttribValue,(int)Value)
			(textwolf::XMLScannerBase::HeaderEnd,(int)CloseTag)
			(textwolf::XMLScannerBase::TagAttribName,-1)
			(textwolf::XMLScannerBase::TagAttribValue,-1)
			(textwolf::XMLScannerBase::OpenTag,-1)
			(textwolf::XMLScannerBase::CloseTag,-1)
			(textwolf::XMLScannerBase::CloseTagIm,-1)
			(textwolf::XMLScannerBase::Content,-1)
			(textwolf::XMLScannerBase::Exit,(int)CloseTag);
		}
	};

	///\brief Constructor
	XmlHeaderInputFilter() {}
	///\brief Destructor
	virtual ~XmlHeaderInputFilter(){}

	///\brief Implementation of protocol::InputFilter::getNext( ElementType*, void*, size_type, size_type*)
	virtual bool getNext( ElementType* type, void* buffer, size_type buffersize, size_type* bufferpos)
	{
		static const ElementTypeMap tmap;
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
			int st = tmap[ m_itr->type()];
			if (st == -1)
			{
				setState( Error, ErrXML);
				return false;
			}
			else
			{
				*type = (ElementType)st;
				return true;
			}
		}
		catch (SrcIterator::EoM)
		{
			if (!gotEoD())
			{
				setState( EndOfMessage);
			}
			else
			{
				setState( Error, ErrXML);
			}
			return false;
		};
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


