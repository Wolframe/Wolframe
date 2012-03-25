/************************************************************************
Copyright (C) 2011 Project Wolframe.
All rights reserved.

This file is part of Project Wolframe.

Commercial Usage
Licensees holding valid Project Wolframe Commercial licenses may
use this file in accordance with the Project Wolframe
Commercial License Agreement provided with the Software or,
alternatively, in accordance with the terms contained
in a written agreement between the licensee and Project Wolframe.

GNU General Public License Usage
Alternatively, you can redistribute this file and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Wolframe is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file textwolf_filter.cpp
///\brief Filter implementation reading/writing xml with the textwolf xml library
#include "filter/textwolf_filter.hpp"
#include "filter/textwolf_filterBase.hpp"
#include "filter/textwolf.hpp"
#include "filter.hpp"
#include "protocol/inputfilter.hpp"
#include "protocol/outputfilter.hpp"
#include <string>
#include <cstring>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using namespace _Wolframe;
using namespace langbind;

namespace {

///\class OutputFilterImpl
///\brief output filter filter for XML using textwolf
///\tparam IOCharset character set encoding of input and output
///\tparam AppCharset character set encoding of the application processor
template <class IOCharset, class AppCharset=textwolf::charset::UTF8>
struct OutputFilterImpl :public protocol::OutputFilter, public FilterBase<IOCharset,AppCharset>
{
	typedef protocol::OutputFilter Parent;

	enum {
		TagBufferSize=1024		///< default size of buffer use for storing tag hierarchy of output
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
	OutputFilterImpl( std::size_t bufsize=TagBufferSize)
		:m_elemitr(0)
		,m_tagstk(new char[bufsize?bufsize:(std::size_t)TagBufferSize])
		,m_tagstksize(bufsize?bufsize:(std::size_t)TagBufferSize)
		,m_tagstkpos(0)
		,m_xmlstate(Tag)
		,m_pendingOpenTag(false)
		,m_bufstate(protocol::EscapingBuffer<std::string>::SRC){}

	///\brief Copy constructor
	///\param [in] o output filter to copy
	OutputFilterImpl( const OutputFilterImpl& o)
		:protocol::OutputFilter(o)
		,m_element(o.m_element)
		,m_elemitr(o.m_elemitr)
		,m_tagstk( new char[o.m_tagstksize])
		,m_tagstksize(o.m_tagstksize)
		,m_tagstkpos(o.m_tagstkpos)
		,m_xmlstate(o.m_xmlstate)
		,m_pendingOpenTag(o.m_pendingOpenTag)
		,m_bufstate(o.m_bufstate)
	{
		std::memcpy( m_tagstk, o.m_tagstk, m_tagstkpos);
	}

	virtual ~OutputFilterImpl()
	{
		delete [] m_tagstk;
	}

	///\brief self copy
	///\return copy of this
	virtual protocol::OutputFilter* copy() const
	{
		return new OutputFilterImpl( *this);
	}

	///\brief Get a member value of the filter
	///\param [in] name case sensitive name of the variable
	///\param [in] value the value returned
	///\return true on success, false, if the variable does not exist or the operation failed
	virtual bool getValue( const char* name, std::string& value)
	{
		if (std::strcmp( name, "tagstacksize") == 0)
		{
			value = boost::lexical_cast<std::string>( m_tagstksize);
			return true;
		}
		return Parent::getValue( name, value);
	}

	///\brief Set a member value of the filter
	///\param [in] name case sensitive name of the variable
	///\param [in] value new value of the variable to set
	///\return true on success, false, if the variable does not exist or the operation failed
	virtual bool setValue( const char* name, const std::string& value)
	{
		std::size_t tagstksize;
		if (std::strcmp( name, "tagstacksize") == 0)
		{
			tagstksize = boost::lexical_cast<std::size_t>( value);
			if (tagstksize <= m_tagstkpos) return false;
			char* tagstk = new (std::nothrow) char[ tagstksize];
			if (!tagstk) return false;
			std::memcpy( tagstk, m_tagstk, m_tagstkpos);
			delete [] m_tagstk;
			m_tagstk = tagstk;
			m_tagstksize = tagstksize;
			return true;
		}
		return Parent::setValue( name, value);
	}

	bool emptybuf()
	{
		std::size_t nn = m_element.size() - m_elemitr;
		if (nn > restsize())
		{
			nn = restsize();
		}
		std::memcpy( rest(), m_element.c_str() + m_elemitr, nn);
		m_elemitr += nn;
		incPos( nn);

		if (m_elemitr == m_element.size())
		{
			m_element.clear();
			m_elemitr = 0;
			return true;
		}
		return false;
	}

	///\brief Implementation of protocol::OutputFilter::print(protocol::OutputFilter::ElementType,const void*,std::size_t)
	///\param [in] type type of the element to print
	///\param [in] element pointer to the element to print
	///\param [in] elementsize size of the element to print in bytes
	///\return true, if success, false else
	virtual bool print( protocol::OutputFilter::ElementType type, const void* element, std::size_t elementsize)
	{
		setState( Open);
		if (m_elemitr < m_element.size())
		{
			/// there is something to print left from last time
			if (!emptybuf())
			{
				setState( EndOfBuffer);
				return false;
			}
			/// we finished the printing left
			return true;
		}
		protocol::EscapingBuffer<std::string> buf( &m_element, m_bufstate);

		const void* cltag;
		std::size_t cltagsize;

		switch (type)
		{
			case protocol::OutputFilter::OpenTag:
				if (m_pendingOpenTag == true)
				{
					FilterBase<IOCharset,AppCharset>::printToBuffer( '>', buf);
				}
				FilterBase<IOCharset,AppCharset>::printToBuffer( '<', buf);
				FilterBase<IOCharset,AppCharset>::printToBuffer( (const char*)element, elementsize, buf);

				if (!pushTag( element, elementsize))
				{
					setState( Error, "textwolf: tag stack exceeds limit");
					return false;
				}
				m_xmlstate = ((const char*)(element))[0]=='?'?Header:Tag;
				m_pendingOpenTag = true;
				m_bufstate = buf.state();

				if (!emptybuf())
				{
					setState( EndOfBuffer);
					return false;
				}
				return true;

			case protocol::OutputFilter::Attribute:
				if (!m_pendingOpenTag)
				{
					setState( Error, "textwolf: illegal operation");
					return false;
				}
				FilterBase<IOCharset,AppCharset>::printToBuffer( ' ', buf);
				FilterBase<IOCharset,AppCharset>::printToBuffer( (const char*)element, elementsize, buf);
				FilterBase<IOCharset,AppCharset>::printToBuffer( '=', buf);

				m_xmlstate = (m_xmlstate==Header)?HeaderAttribute:Attribute;
				m_bufstate = buf.state();

				if (!emptybuf())
				{
					setState( EndOfBuffer);
					return false;
				}
				return true;

			case protocol::OutputFilter::Value:
				if (m_xmlstate == Attribute)
				{
					printToBufferAttributeValue( (const char*)element, elementsize, buf);
					m_xmlstate = Tag;
				}
				else if (m_xmlstate == HeaderAttribute)
				{
					printToBufferAttributeValue( (const char*)element, elementsize, buf);
					m_xmlstate = Header;
				}
				else
				{
					if (m_pendingOpenTag == true)
					{
						FilterBase<IOCharset,AppCharset>::printToBuffer( '>', buf);
					}
					printToBufferContent( (const char*)element, elementsize, buf);
					m_pendingOpenTag = false;
					m_xmlstate = Content;
				}
				m_bufstate = buf.state();

				if (!emptybuf())
				{
					setState( EndOfBuffer);
					return false;
				}
				return true;

			case protocol::OutputFilter::CloseTag:
				if (!topTag( cltag, cltagsize) || !cltagsize)
				{
					setState( Error, "textwolf: tags not balanced");
					return false;
				}
				if (m_xmlstate == Header)
				{
					FilterBase<IOCharset,AppCharset>::printToBuffer( '?', buf);
					FilterBase<IOCharset,AppCharset>::printToBuffer( '>', buf);
					FilterBase<IOCharset,AppCharset>::printToBufferEOL( buf);
				}
				else if (m_pendingOpenTag == true)
				{
					FilterBase<IOCharset,AppCharset>::printToBuffer( '/', buf);
					FilterBase<IOCharset,AppCharset>::printToBuffer( '>', buf);
				}
				else
				{
					FilterBase<IOCharset,AppCharset>::printToBuffer( '<', buf);
					FilterBase<IOCharset,AppCharset>::printToBuffer( '/', buf);
					FilterBase<IOCharset,AppCharset>::printToBuffer( (const char*)cltag, cltagsize, buf);
					FilterBase<IOCharset,AppCharset>::printToBuffer( '>', buf);
				}
				m_xmlstate = Content;
				m_pendingOpenTag = false;
				popTag();
				m_bufstate = buf.state();

				if (!emptybuf())
				{
					setState( EndOfBuffer);
					return false;
				}
				return true;
		}
		setState( Error, "textwolf: illegal state");
		return false;
	}
private:
	///\brief print a character substitute or the character itself
	///\param [in,out] buf buffer to print to
	///\param [in] nof_echr number of elements in echr and estr
	///\param [in] echr ASCII characters to substitute
	///\param [in] estr ASCII strings to substitute with (array parallel to echr)
	static void printEsc( char ch, protocol::EscapingBuffer<std::string>& buf, unsigned int nof_echr, const char* echr, const char** estr)
	{
		const char* cc = (const char*)memchr( echr, ch, nof_echr);
		if (cc)
		{
			unsigned int ii = 0;
			const char* tt = estr[ cc-echr];
			while (tt[ii]) IOCharset::print( tt[ii++], buf);
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
	static void printToBufferSubstChr( const char* src, std::size_t srcsize, protocol::EscapingBuffer<std::string>& buf, unsigned int nof_echr, const char* echr, const char** estr)
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
	static void printToBufferAttributeValue( const char* src, std::size_t srcsize, protocol::EscapingBuffer<std::string>& buf)
	{
		enum {nof_echr = 12};
		static const char* estr[nof_echr] = {"&lt;", "&gt;", "&apos;", "&quot;", "&amp;", "&#0;", "&#8;", "&#9;", "&#10;", "&#13;"};
		static const char echr[nof_echr+1] = "<>'\"&\0\b\t\n\r";
		IOCharset::print( '"', buf);
		printToBufferSubstChr( src, srcsize, buf, nof_echr, echr, estr);
		IOCharset::print( '"', buf);
	}

	///\brief print content value string
	///\param [in] src pointer to content string to print
	///\param [in] srcsize size of src in bytes
	///\param [in,out] buf buffer to print to
	static void printToBufferContent( const char* src, std::size_t srcsize, protocol::EscapingBuffer<std::string>& buf)
	{
		enum {nof_echr = 6};
		static const char* estr[nof_echr] = {"&lt;", "&gt;", "&amp;", "&#0;", "&#8;"};
		static const char echr[nof_echr+1] = "<>&\0\b";
		printToBufferSubstChr( src, srcsize, buf, nof_echr, echr, estr);
	}

	static std::size_t getAlign( std::size_t n)
	{
		return (sizeof(std::size_t) - (n & (sizeof(std::size_t)-1))) & (sizeof(std::size_t)-1);
	}

	bool pushTag( const void* element, std::size_t elementsize)
	{
		std::size_t align = getAlign( elementsize);
		if (align + elementsize + sizeof(std::size_t) >= m_tagstksize-m_tagstkpos) return false;
		std::memcpy( m_tagstk + m_tagstkpos, element, elementsize);
		std::size_t ofs = elementsize + align + sizeof( std::size_t);
		m_tagstkpos += ofs;
		void* tt = m_tagstk+m_tagstkpos-sizeof( std::size_t);
		*(std::size_t*)(tt) = elementsize;
		return true;
	}

	bool topTag( const void*& element, std::size_t& elementsize)
	{
		if (m_tagstkpos < sizeof( std::size_t)) return false;
		void* tt = m_tagstk+m_tagstkpos-sizeof( std::size_t);
		elementsize = *(std::size_t*)(tt);
		std::size_t align = getAlign( elementsize);
		std::size_t ofs = elementsize + align + sizeof( std::size_t);
		if (ofs > m_tagstkpos) return false;
		element = m_tagstk + m_tagstkpos - ofs;
		return true;
	}

	void popTag()
	{
		void* tt = m_tagstk+m_tagstkpos-sizeof( std::size_t);
		std::size_t elementsize = *(std::size_t*)(tt);
		std::size_t align = getAlign( elementsize);
		std::size_t ofs = elementsize + align + sizeof( std::size_t);
		if (m_tagstkpos < ofs) throw std::logic_error( "tag stack for output is corrupt");
		m_tagstkpos -= ofs;
	}
private:
	std::string m_element;								///< buffer for the currently printed element
	std::size_t m_elemitr;								///< iterator to pass it to output
	char* m_tagstk;									///< tag stack buffer
	std::size_t m_tagstksize;							///< size of tag stack buffer in bytes
	std::size_t m_tagstkpos;							///< used size of tag stack buffer in bytes
	XMLState m_xmlstate;								///< current state of output
	bool m_pendingOpenTag;								///< true if last open tag instruction has not been ended yet
	typename protocol::EscapingBuffer<std::string>::State m_bufstate;		///< state of escaping the output
};


static bool getEncoding( const std::string& xmlHeader, TextwolfEncoding::Id &enc)
{
	bool rt = true;
	enc = TextwolfEncoding::Unknown;
	typedef textwolf::XMLScanner<char*,textwolf::charset::UTF8,textwolf::charset::UTF8,std::string> Scan;
	char* src = const_cast<char*>( xmlHeader.c_str());
	std::string valuebuf;
	std::string encoding;
	Scan scan( src, valuebuf);
	Scan::iterator itr = scan.begin(),end = scan.end();
	bool encodingAttributeParsed = false;

	while (itr != end)
	{
		switch (itr->type())
		{
			case textwolf::XMLScannerBase::ErrorOccurred:
			case textwolf::XMLScannerBase::None:
				return false;

			case textwolf::XMLScannerBase::HeaderStart:
			case textwolf::XMLScannerBase::HeaderEnd:
			case textwolf::XMLScannerBase::TagAttribName:
			case textwolf::XMLScannerBase::TagAttribValue:
			case textwolf::XMLScannerBase::OpenTag:
			case textwolf::XMLScannerBase::CloseTag:
			case textwolf::XMLScannerBase::CloseTagIm:
			case textwolf::XMLScannerBase::Content:
			case textwolf::XMLScannerBase::Exit:
				encodingAttributeParsed = false;
				break;

			case textwolf::XMLScannerBase::HeaderAttribName:
				encodingAttributeParsed = (strcmp( valuebuf.c_str(), "encoding") == 0);
				break;

			case textwolf::XMLScannerBase::HeaderAttribValue:
				if (encodingAttributeParsed)
				{
					enc = TextwolfEncoding::getId( valuebuf.c_str());
					return true;
				}
				encodingAttributeParsed = false;
				break;
			default:
				encodingAttributeParsed = false;
		}
		++itr;
	}
	return rt;
}

///\class InputFilterImpl
///\brief input filter for XML using textwolf
///\tparam IOCharset character set encoding of input and output
///\tparam AppCharset character set encoding of the application processor
template <class IOCharset, class AppCharset=textwolf::charset::UTF8>
struct InputFilterImpl :public protocol::InputFilter, public FilterBase<IOCharset,AppCharset>
{
	typedef protocol::InputFilter Parent;
	typedef textwolf::XMLScanner<SrcIterator,IOCharset,AppCharset,textwolf::StaticBuffer> XMLScanner;

	///\brief Constructor
	InputFilterImpl( std::size_t genbufsize, bool withEmpty, bool doTokenize)
		:protocol::InputFilter( genbufsize)
		,m_outputbuf(0,0)
		,m_scanner(0)
		,m_withEmpty(withEmpty)
		,m_doTokenize(doTokenize)
		,m_bufstart(0)
	{
		m_src.setInput( this);
		m_scanner = new XMLScanner( m_src, m_outputbuf);
		m_scanner->doTokenize(m_doTokenize);
		m_itr = m_scanner->begin(false);
		m_end = m_scanner->end();
	}

	///\brief Destructor
	virtual ~InputFilterImpl()
	{
		delete m_scanner;
	}

	///\brief Copy constructor
	///\param [in] o output filter to copy
	InputFilterImpl( const InputFilterImpl& o)
		:protocol::InputFilter( o)
		,m_outputbuf(o.m_outputbuf)
		,m_src( o.m_src)
		,m_scanner(0)
		,m_withEmpty(o.m_withEmpty)
		,m_doTokenize(o.m_doTokenize)
		,m_bufstart(o.m_bufstart)
	{
		m_src.setInput( this);
		m_scanner = new XMLScanner( *o.m_scanner);
		m_scanner->setSource( m_src);
		m_scanner->setOutputBuffer( m_outputbuf);
		m_scanner->doTokenize(m_doTokenize);
		m_itr = m_scanner->begin(false);
		m_end = m_scanner->end();
	}

	///\brief Get a member value of the filter
	///\param [in] name case sensitive name of the variable
	///\param [in] val the value returned
	///\return true on success, false, if the variable does not exist or the operation failed
	virtual bool getValue( const char* name, std::string& val)
	{
		if (std::strcmp( name, "empty") == 0)
		{
			val = m_withEmpty?"true":"false";
			return true;
		}
		if (std::strcmp( name, "tokenize") == 0)
		{
			val = m_doTokenize?"true":"false";
			return true;
		}
		return Parent::getValue( name, val);
	}

	///\brief Set a member value of the filter
	///\param [in] name case sensitive name of the variable
	///\param [in] value new value of the variable to set
	///\return true on success, false, if the variable does not exist or the operation failed
	virtual bool setValue( const char* name, const std::string& value)
	{
		if (std::strcmp( name, "empty") == 0)
		{
			if (std::strcmp( value.c_str(), "true") == 0)
			{
				m_withEmpty = true;
			}
			else if (std::strcmp( value.c_str(), "false") == 0)
			{
				m_withEmpty = false;
			}
			else
			{
				return false;
			}
			return true;
		}
		if (std::strcmp( name, "tokenize") == 0)
		{
			if (std::strcmp( value.c_str(), "true") == 0)
			{
				m_doTokenize = true;
				if (m_scanner) m_scanner->doTokenize(true);
			}
			else if (std::strcmp( value.c_str(), "false") == 0)
			{
				m_doTokenize = false;
				if (m_scanner) m_scanner->doTokenize(false);
			}
			else
			{
				return false;
			}
			return true;
		}
		return Parent::setValue( name, value);
	}

	///\brief self copy
	///\return copy of this
	virtual protocol::InputFilter* copy() const
	{
		return new InputFilterImpl( *this);
	}

	struct ElementTypeMap :public textwolf::CharMap<int,-1,textwolf::XMLScannerBase::NofElementTypes>
	{
		ElementTypeMap()
		{
			(*this)
			(textwolf::XMLScannerBase::None,-1)
			(textwolf::XMLScannerBase::ErrorOccurred,-1)
			(textwolf::XMLScannerBase::HeaderStart,(int)OpenTag)
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

	///\brief Implementation of protocol::InputFilter::getNext( protocol::OutputFilter::ElementType*, void*, std::size_t, std::size_t*)
	virtual bool getNext( protocol::InputFilter::ElementType* type, void* buffer, std::size_t buffersize, std::size_t* bufferpos)
	{
		static const ElementTypeMap tmap;
		if (m_bufstart > *bufferpos)
		{
			m_bufstart = *bufferpos;
		}
		textwolf::StaticBuffer buf( (char*)buffer + m_bufstart, buffersize - m_bufstart, *bufferpos - m_bufstart);
		m_scanner->setOutputBuffer( buf);
		try
		{
			setState( Open);
			++m_itr;
			if (buf.overflow())
			{
				setState( protocol::InputFilter::Error, "textwolf: input filter buffer too small for one element");
				return false;
			}
			int st = tmap[ m_itr->type()];
			if (st == -1)
			{
				setState( Error, "textwolf: syntax error in XML");
				return false;
			}
			else
			{
				*type = (protocol::InputFilter::ElementType)st;
				if (!m_withEmpty && m_itr->type() == textwolf::XMLScannerBase::Content)
				{
					std::size_t ii=0,nn = buf.size();
					const unsigned char* cc = (unsigned char*)buf.ptr();
					for (;ii<nn && cc[ii] <= ' '; ++ii);
					if (ii==nn)
					{
						*bufferpos = m_bufstart;
						return getNext( type, buffer, buffersize, bufferpos);
					}
				}
				*bufferpos = m_bufstart + buf.size();
				m_bufstart = *bufferpos;
				return true;
			}
		}
		catch (SrcIterator::EoM)
		{
			setState( EndOfMessage);
			*bufferpos = m_bufstart + buf.size();
			return false;
		};
		setState( Error, "textwolf: unexpected state");
		return false;
	}
private:
	textwolf::StaticBuffer m_outputbuf;	///< dummy buffer of size 0 (the buffer redefined in every call getNext. This is a dummy)
	SrcIterator m_src;			///< source iterator
	XMLScanner* m_scanner;			///< XML scanner
	typename XMLScanner::iterator m_itr;	///< input iterator created from scanned XML from source iterator
	typename XMLScanner::iterator m_end;	///< end of data (EoD) pointer
	bool m_withEmpty;			///< do not produce empty tokens (containing only spaces)
	bool m_doTokenize;			///< do tokenize (whitespace sequences as delimiters)
	std::size_t m_bufstart;			///< start of the currently fetched token
};

class InputFilter :public protocol::InputFilter
{
public:
	typedef protocol::InputFilter Parent;

	///\enum ErrorCodes
	///\brief Enumeration of error codes
	enum ErrorCodes
	{
		Ok,			///< no error
		ErrCreateFilter,	///< could not create filter
		ErrEncoding,		///< could not create filter for this encoding
		ErrXML			///< error in XML
	};

public:
	InputFilter( const CountedReference<TextwolfEncoding::Id>& enc, std::size_t bufsize)
		:protocol::InputFilter(bufsize)
		,m_bufsize(bufsize)
		,m_headerParsed(false)
		,m_headerParseLeft(0)
		,m_encoding(enc)
		,m_withEmpty(true)
		,m_doTokenize(false)
		{}
	InputFilter( const InputFilter& o)
		:protocol::InputFilter(o)
		,m_bufsize(o.m_bufsize)
		,m_headerParsed(o.m_headerParsed)
		,m_headerParseLeft(o.m_headerParseLeft)
		,m_header(o.m_header)
		,m_encoding(o.m_encoding)
		,m_withEmpty(o.m_withEmpty)
		,m_doTokenize(o.m_doTokenize){}

	virtual ~InputFilter(){}

	///\brief Get a member value of the filter
	///\param [in] name case sensitive name of the variable
	///\param [in] val the value returned
	///\return true on success, false, if the variable does not exist or the operation failed
	virtual bool getValue( const char* name, std::string& val)
	{
		if (std::strcmp( name, "empty") == 0)
		{
			val = m_withEmpty?"true":"false";
			return true;
		}
		if (std::strcmp( name, "tokenize") == 0)
		{
			val = m_doTokenize?"true":"false";
			return true;
		}
		return Parent::getValue( name, val);
	}

	///\brief Set a member value of the filter
	///\param [in] name case sensitive name of the variable
	///\param [in] value new value of the variable to set
	///\return true on success, false, if the variable does not exist or the operation failed
	virtual bool setValue( const char* name, const std::string& value)
	{
		if (std::strcmp( name, "empty") == 0)
		{
			if (std::strcmp( value.c_str(), "true") == 0)
			{
				m_withEmpty = true;
			}
			else if (std::strcmp( value.c_str(), "false") == 0)
			{
				m_withEmpty = false;
			}
			else
			{
				return false;
			}
			return true;
		}
		if (std::strcmp( name, "tokenize") == 0)
		{
			if (std::strcmp( value.c_str(), "true") == 0)
			{
				m_doTokenize = true;
			}
			else if (std::strcmp( value.c_str(), "false") == 0)
			{
				m_doTokenize = false;
			}
			else
			{
				return false;
			}
			return true;
		}
		return Parent::setValue( name, value);
	}

	virtual protocol::InputFilter* copy() const
	{
		return new InputFilter( *this);
	}

	virtual bool getNext( ElementType*, void*, std::size_t, std::size_t*)
	{
		for (;;)
		if (m_headerParsed)
		{
			return false;
		}
		else if (m_headerParseLeft)
		{
			const char* cc = (char*)ptr();
			std::size_t nn = size();
			std::size_t ii = 0;
			for (;ii < nn && m_headerParseLeft>0; ++ii,--m_headerParseLeft)
			{
				if (cc[ii] != '\0')
				{
					setState( Error, "textwolf: syntax error in XML");
					return false;
				}
			}
			skip( ii);
			if (m_headerParseLeft)
			{
				setState( EndOfMessage);
			}
			else
			{
				m_headerParsed = true;
				setState( Open);
			}
			return false;
		}
		else
		{
			const char* cc = (char*)ptr();
			std::size_t nn = size();
			std::size_t ii = 0;
			for (;ii < nn; ++ii)
			{
				if (cc[ii] == '\n') break;
				if (cc[ii] != '\0')
				{
					if (cc[ii] < 0 || m_header.size() > 128)
					{
						setState( Error, "textwolf: syntax error in XML");
						return false;
					}
					m_header.push_back( cc[ii]);
				}
			}
			if (ii < nn)
			{
				TextwolfEncoding::Id enc;

				m_header.push_back( '\n');
				skip( ii+1);

				if (getEncoding( m_header, enc))
				{
					m_encoding.reset( new TextwolfEncoding::Id( enc));
					if (enc == TextwolfEncoding::Unknown)
					{
						setState( Error, "textwolf: cannot handle this encoding");
					}
					else
					{
						setState( Open);

						if (enc == TextwolfEncoding::UTF16LE
						|| enc == TextwolfEncoding::UCS2LE)
						{
							m_headerParseLeft = 1;
							continue;
						}
						else if (enc == TextwolfEncoding::UCS4LE)
						{
							m_headerParseLeft = 3;
							continue;
						}
						else
						{
							m_headerParseLeft = 0;
						}
					}
				}
				else
				{
					setState( Error, "textwolf: syntax error in XML");
				}
				m_headerParsed = true;
			}
			else
			{
				skip( nn);
				setState( EndOfMessage);
			}
			return false;
		}
	}

	virtual protocol::InputFilter* createFollow() const
	{
		if (!m_headerParsed) return 0;
		TextwolfEncoding::Id enc = m_encoding.get()?*m_encoding.get():TextwolfEncoding::Unknown;
		protocol::InputFilter* rt = 0;
		switch (enc)
		{
			case TextwolfEncoding::Unknown:
				return 0;
			case TextwolfEncoding::IsoLatin:
				rt = new InputFilterImpl<textwolf::charset::IsoLatin1>(m_bufsize,m_withEmpty,m_doTokenize);
				break;
			case TextwolfEncoding::UTF8:
				rt = new InputFilterImpl<textwolf::charset::UTF8>(m_bufsize,m_withEmpty,m_doTokenize);
				break;
			case TextwolfEncoding::UTF16:
				rt = new InputFilterImpl<textwolf::charset::UTF16BE>(m_bufsize,m_withEmpty,m_doTokenize);
				break;
			case TextwolfEncoding::UTF16BE:
				rt = new InputFilterImpl<textwolf::charset::UTF16BE>(m_bufsize,m_withEmpty,m_doTokenize);
				break;
			case TextwolfEncoding::UTF16LE:
				rt = new InputFilterImpl<textwolf::charset::UTF16LE>(m_bufsize,m_withEmpty,m_doTokenize);
				break;
			case TextwolfEncoding::UCS2BE:
				rt = new InputFilterImpl<textwolf::charset::UCS2BE>(m_bufsize,m_withEmpty,m_doTokenize);
				break;
			case TextwolfEncoding::UCS2LE:
				rt = new InputFilterImpl<textwolf::charset::UCS2LE>(m_bufsize,m_withEmpty,m_doTokenize);
				break;
			case TextwolfEncoding::UCS4BE:
				rt = new InputFilterImpl<textwolf::charset::UCS4BE>(m_bufsize,m_withEmpty,m_doTokenize);
				break;
			case TextwolfEncoding::UCS4LE:
				rt = new InputFilterImpl<textwolf::charset::UCS4LE>(m_bufsize,m_withEmpty,m_doTokenize);
				break;
		}
		if (rt)
		{
			*rt = *this;
		}
		return rt;
	}

private:
	std::size_t m_bufsize;
	bool m_headerParsed;
	char m_headerParseLeft;
	std::string m_header;
	CountedReference<TextwolfEncoding::Id> m_encoding;
	bool m_withEmpty;
	bool m_doTokenize;
};

class OutputFilter :public protocol::OutputFilter
{
public:
	///\enum ErrorCodes
	///\brief Enumeration of error codes
	enum ErrorCodes
	{
		Ok,			///< no error
		ErrEncoding,		///< tack stack overflow
		ErrCreateFilter		///< could not output filter
	};

public:
	OutputFilter( const CountedReference<TextwolfEncoding::Id>& enc, std::size_t tagbufsize)
		:m_tagbuffersize(tagbufsize)
		,m_headerPrinted(false)
		,m_headerPos(0)
		,m_encoding(enc){}

	OutputFilter( const OutputFilter& o)
		:protocol::OutputFilter(o)
		,m_tagbuffersize(o.m_tagbuffersize)
		,m_headerPrinted(o.m_headerPrinted)
		,m_headerPos(o.m_headerPos)
		,m_header(o.m_header)
		,m_encoding(o.m_encoding){}

	virtual ~OutputFilter(){}

	virtual OutputFilter* copy() const
	{
		return new OutputFilter( *this);
	}

	virtual protocol::OutputFilter* createFollow()
	{
		namespace tc = textwolf::charset;
		if (!m_headerPrinted) return 0;
		TextwolfEncoding::Id enc = m_encoding.get()?*m_encoding.get():TextwolfEncoding::UTF8;
		protocol::OutputFilter* rt = 0;
		switch (enc)
		{
			case TextwolfEncoding::Unknown:
				setState( Error, "textwolf: cannot handle this encoding");
				return false;
			case TextwolfEncoding::IsoLatin:
				rt = new OutputFilterImpl<tc::IsoLatin1>( m_tagbuffersize);
				break;
			case TextwolfEncoding::UTF8:
				rt = new OutputFilterImpl<tc::UTF8>( m_tagbuffersize);
				break;
			case TextwolfEncoding::UTF16:
				rt = new OutputFilterImpl<tc::UTF16BE>( m_tagbuffersize);
				break;
			case TextwolfEncoding::UTF16BE:
				rt = new OutputFilterImpl<tc::UTF16BE>( m_tagbuffersize);
				break;
			case TextwolfEncoding::UTF16LE:
				rt = new OutputFilterImpl<tc::UTF16LE>( m_tagbuffersize);
				break;
			case TextwolfEncoding::UCS2BE:
				rt = new OutputFilterImpl<tc::UCS2BE>( m_tagbuffersize);
				break;
			case TextwolfEncoding::UCS2LE:
				rt = new OutputFilterImpl<tc::UCS2LE>( m_tagbuffersize);
				break;
			case TextwolfEncoding::UCS4BE:
				rt = new OutputFilterImpl<tc::UCS4BE>( m_tagbuffersize);
				break;
			case TextwolfEncoding::UCS4LE:
				rt = new OutputFilterImpl<tc::UCS4LE>( m_tagbuffersize);
				break;
		}
		if (rt)
		{
			*rt = *this;
		}
		return rt;
	}

	virtual bool print( ElementType, const void*, std::size_t)
	{
		if (m_headerPrinted)
		{
			setState( Open);
			return false;
		}
		if (m_header.size() == 0)
		{
			TextwolfEncoding::Id enc = m_encoding.get()?*m_encoding.get():TextwolfEncoding::UTF8;
			namespace tc = textwolf::charset;

			switch (enc)
			{
				case TextwolfEncoding::Unknown:
				{
					setState( Error, "textwolf: cannot handle this encoding");
					return false;
				}
				case TextwolfEncoding::IsoLatin:
				{
					m_header = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"yes\"?>\n";
					break;
				}
				case TextwolfEncoding::UTF8:
				{
					m_header = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n";
					break;
				}
				case TextwolfEncoding::UTF16:
				{
					typedef FilterBase<tc::UTF16<>,tc::IsoLatin1> FLT;
					const char* elem = "<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"yes\"?>\n";
					FLT::printToBuffer( (const char*)elem, std::strlen(elem), m_header);
					break;
				}
				case TextwolfEncoding::UTF16BE:
				{
					typedef FilterBase<tc::UTF16<tc::ByteOrder::BE>,tc::IsoLatin1> FLT;
					const char* elem = "<?xml version=\"1.0\" encoding=\"UTF-16BE\" standalone=\"yes\"?>\n";
					FLT::printToBuffer( (const char*)elem, std::strlen(elem), m_header);
					break;
				}
				case TextwolfEncoding::UTF16LE:
				{
					typedef FilterBase<tc::UTF16<tc::ByteOrder::LE>,tc::IsoLatin1> FLT;
					const char* elem = "<?xml version=\"1.0\" encoding=\"UTF-16LE\" standalone=\"yes\"?>\n";
					FLT::printToBuffer( (const char*)elem, std::strlen(elem), m_header);
					break;
				}
				case TextwolfEncoding::UCS2BE:
				{
					typedef FilterBase<tc::UCS2<tc::ByteOrder::BE>,tc::IsoLatin1> FLT;
					const char* elem = "<?xml version=\"1.0\" encoding=\"UCS-2BE\" standalone=\"yes\"?>\n";
					FLT::printToBuffer( (const char*)elem, std::strlen(elem), m_header);
					break;
				}
				case TextwolfEncoding::UCS2LE:
				{
					typedef FilterBase<tc::UCS2<tc::ByteOrder::LE>,tc::IsoLatin1> FLT;
					const char* elem = "<?xml version=\"1.0\" encoding=\"UCS-2LE\" standalone=\"yes\"?>\n";
					FLT::printToBuffer( (const char*)elem, std::strlen(elem), m_header);
					break;
				}
				case TextwolfEncoding::UCS4BE:
				{
					typedef FilterBase<tc::UCS4<tc::ByteOrder::BE>,tc::IsoLatin1> FLT;
					const char* elem = "<?xml version=\"1.0\" encoding=\"UCS-4BE\" standalone=\"yes\"?>\n";
					FLT::printToBuffer( (const char*)elem, std::strlen(elem), m_header);
					break;
				}
				case TextwolfEncoding::UCS4LE:
				{
					typedef FilterBase<tc::UCS4<tc::ByteOrder::LE>,tc::IsoLatin1> FLT;
					const char* elem = "<?xml version=\"1.0\" encoding=\"UCS-4LE\" standalone=\"yes\"?>\n";
					FLT::printToBuffer( (const char*)elem, std::strlen(elem), m_header);
					break;
				}
			}
			m_headerPos = 0;
		}
		std::size_t nn = m_header.size() - m_headerPos;
		if (nn > restsize()) nn = restsize();
		memcpy( rest(), m_header.c_str()+m_headerPos, nn);
		incPos( nn);
		m_headerPos += nn;
		m_headerPrinted = (m_headerPos == m_header.size());
		setState( EndOfBuffer);
		return false;
	}

private:
	std::size_t m_tagbuffersize;
	bool m_headerPrinted;
	std::size_t m_headerPos;
	std::string m_header;
	CountedReference<TextwolfEncoding::Id> m_encoding;
};

}//end anonymous namespace

class TextwolfXmlFilter :public Filter
{
public:
	TextwolfXmlFilter( std::size_t elementbufsize, std::size_t tagbufsize)
	{
		CountedReference<TextwolfEncoding::Id> enc;
		m_inputfilter.reset( new InputFilter( enc, elementbufsize));
		m_outputfilter.reset( new OutputFilter( enc, tagbufsize));
	}

	TextwolfXmlFilter( std::size_t elementbufsize, std::size_t tagbufsize, const char* encoding)
	{
		TextwolfEncoding::Id ei = TextwolfEncoding::getId( encoding);
		CountedReference<TextwolfEncoding::Id> enc( new TextwolfEncoding::Id( ei));
		m_inputfilter.reset( new InputFilter( enc, elementbufsize));
		m_outputfilter.reset( new OutputFilter( enc, tagbufsize));
	}
};

Filter TextwolfXmlFilterFactory::create( const char* encoding) const
{
	if (encoding)
	{
		return TextwolfXmlFilter( 8092, 256, encoding);
	}
	else
	{
		return TextwolfXmlFilter( 8092, 256);
	}
}

