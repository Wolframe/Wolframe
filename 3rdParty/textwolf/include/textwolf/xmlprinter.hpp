/*
---------------------------------------------------------------------
    The template library textwolf implements an input iterator on
    a set of XML path expressions without backward references on an
    STL conforming input iterator as source. It does no buffering
    or read ahead and is dedicated for stream processing of XML
    for a small set of XML queries.
    Stream processing in this Object refers to processing the
    document without buffering anything but the current result token
    processed with its tag hierarchy information.

    Copyright (C) 2010,2011,2012,2013,2014 Patrick Frey

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3.0 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

--------------------------------------------------------------------

	The latest version of textwolf can be found at 'http://github.com/patrickfrey/textwolf'
	For documentation see 'http://patrickfrey.github.com/textwolf'

--------------------------------------------------------------------
*/
/// \file textwolf/xmlprinter.hpp
/// \brief XML printer interface hiding character encoding properties

#ifndef __TEXTWOLF_XML_PRINTER_HPP__
#define __TEXTWOLF_XML_PRINTER_HPP__
#include "textwolf/cstringiterator.hpp"
#include "textwolf/textscanner.hpp"
#include "textwolf/xmlscanner.hpp"
#include "textwolf/charset.hpp"
#include "textwolf/xmltagstack.hpp"
#include <cstring>
#include <cstdlib>

/// \namespace textwolf
/// \brief Toplevel namespace of the library
namespace textwolf {

/// \class XMLPrinter
/// \brief Character encoding dependent XML printer
/// \tparam IOCharset Character set encoding of input and output
/// \tparam AppCharset Character set encoding of the application processor
/// \tparam BufferType STL back insertion sequence to use for printing output
template <class IOCharset, class AppCharset,class BufferType>
class XMLPrinter
{
private:
	/// \brief Prints a character string to an STL back insertion sequence buffer in the IO character set encoding
	/// \param [in] src pointer to string to print
	/// \param [in] srcsize size of src in bytes
	/// \param [out] buf buffer to append result to
	void printToBuffer( const char* src, std::size_t srcsize, BufferType& buf) const
	{
		CStringIterator itr( src, srcsize);
		TextScanner<CStringIterator,AppCharset> ts( itr);

		UChar ch;
		while ((ch = ts.chr()) != 0)
		{
			m_output.print( ch, buf);
			++ts;
		}
	}

	/// \brief print a character substitute or the character itself
	/// \param [in] ch character to print
	/// \param [in,out] buf buffer to print to
	/// \param [in] nof_echr number of elements in echr and estr
	/// \param [in] echr ASCII characters to substitute
	/// \param [in] estr ASCII strings to substitute with (array parallel to echr)
	void printEsc( char ch, BufferType& buf, unsigned int nof_echr, const char* echr, const char** estr) const
	{
		const char* cc = (const char*)memchr( echr, ch, nof_echr);
		if (cc)
		{
			unsigned int ii = 0;
			const char* tt = estr[ cc-echr];
			while (tt[ii]) m_output.print( tt[ii++], buf);
		}
		else
		{
			m_output.print( ch, buf);
		}
	}

	/// \brief print a value with some characters replaced by a string
	/// \param [in] src pointer to attribute value string to print
	/// \param [in] srcsize size of src in bytes
	/// \param [in,out] buf buffer to print to
	/// \param [in] nof_echr number of elements in echr and estr
	/// \param [in] echr ASCII characters to substitute
	/// \param [in] estr ASCII strings to substitute with (array parallel to echr)
	void printToBufferSubstChr( const char* src, std::size_t srcsize, BufferType& buf, unsigned int nof_echr, const char* echr, const char** estr) const
	{
		CStringIterator itr( src, srcsize);
		textwolf::TextScanner<CStringIterator,AppCharset> ts( itr);

		textwolf::UChar ch;
		while ((ch = ts.chr()) != 0)
		{
			if (ch < 128)
			{
				printEsc( (char)ch, buf, nof_echr, echr, estr);
			}
			else
			{
				m_output.print( ch, buf);
			}
			++ts;
		}
	}

	/// \brief print attribute value string
	/// \param [in] src pointer to attribute value string to print
	/// \param [in] srcsize size of src in bytes
	/// \param [in,out] buf buffer to print to
	void printToBufferAttributeValue( const char* src, std::size_t srcsize, BufferType& buf) const
	{
		enum {nof_echr = 12};
		static const char* estr[nof_echr] = {"&lt;", "&gt;", "&apos;", "&quot;", "&amp;", "&#0;", "&#8;", "&#9;", "&#10;", "&#13;"};
		static const char echr[nof_echr+1] = "<>'\"&\0\b\t\n\r";
		m_output.print( '"', buf);
		printToBufferSubstChr( src, srcsize, buf, nof_echr, echr, estr);
		m_output.print( '"', buf);
	}

	/// \brief print content value string
	/// \param [in] src pointer to content string to print
	/// \param [in] srcsize size of src in bytes
	/// \param [in,out] buf buffer to print to
	void printToBufferContent( const char* src, std::size_t srcsize, BufferType& buf) const
	{
		enum {nof_echr = 6};
		static const char* estr[nof_echr] = {"&lt;", "&gt;", "&amp;", "&#0;", "&#8;"};
		static const char echr[nof_echr+1] = "<>&\0\b";
		printToBufferSubstChr( src, srcsize, buf, nof_echr, echr, estr);
	}

	/// \brief Prints a character to an STL back insertion sequence buffer in the IO character set encoding
	/// \param [in] ch character to print
	/// \param [in,out] buf buffer to print to
	void printToBuffer( char ch, BufferType& buf) const
	{
		m_output.print( (textwolf::UChar)(unsigned char)ch, buf);
	}

public:
	/// \brief Default constructor
	XMLPrinter()
		:m_state(Init){}

	/// \brief Constructor
	explicit XMLPrinter( const IOCharset& output_)
		:m_state(Init),m_output(output_){}

	/// \brief Copy constructor
	XMLPrinter( const XMLPrinter& o)
		:m_state(o.m_state),m_buf(o.m_buf),m_tagstack(o.m_tagstack),m_output(o.m_output)
	{}

	/// \brief Prints an XML header (version "1.0")
	/// \param [in] encoding character set encoding name
	/// \param [in] standalone standalone attribute ("yes","no" or NULL for undefined)
	/// \param [out] buf buffer to print to
	/// \return true on success, false if failed (check lasterror())
	bool printHeader( const char* encoding, const char* standalone, BufferType& buf)
	{
		if (m_state != Init)
		{
			m_lasterror = "printing document not starting with xml header";
			return false;
		}
		std::string enc = encoding?encoding:"UTF-8";
		printToBuffer( "<?xml version=\"1.0\" encoding=\"", 30, buf);
		printToBuffer( enc.c_str(), enc.size(), buf);
		if (standalone)
		{
			printToBuffer( "\" standalone=\"", 14, buf);
			printToBuffer( standalone, std::strlen(standalone), buf);
			printToBuffer( "\"?>\n", 4, buf);
		}
		else
		{
			printToBuffer( "\"?>\n", 4, buf);
		}
		m_state = Content;
		return true;
	}

	/// \brief Prints an XML <!DOCTYPE ...> declaration
	/// \param [in] rootid root element name
	/// \param [in] publicid PUBLIC attribute
	/// \param [in] systemid SYSTEM attribute
	/// \param [out] buf buffer to print to
	/// \return true on success, false if failed (check lasterror())
	bool printDoctype( const char* rootid, const char* publicid, const char* systemid, BufferType& buf)
	{
		if (rootid)
		{
			if (publicid)
			{
				if (!systemid)
				{
					m_lasterror = "defined DOCTYPE with PUBLIC id but no SYSTEM id";
					return false;
				}
				printToBuffer( "<!DOCTYPE ", 10, buf);
				printToBuffer( rootid, std::strlen( rootid), buf);
				printToBuffer( " PUBLIC \"", 9, buf);
				printToBuffer( publicid, std::strlen( publicid), buf);
				printToBuffer( "\" \"", 3, buf);
				printToBuffer( systemid, std::strlen( systemid), buf);
				printToBuffer( "\">", 2, buf);
			}
			else if (systemid)
			{
				printToBuffer( "<!DOCTYPE ", 10, buf);
				printToBuffer( rootid, std::strlen( rootid), buf);
				printToBuffer( " SYSTEM \"", 9, buf);
				printToBuffer( systemid, std::strlen( systemid), buf);
				printToBuffer( "\">", 2, buf);
			}
			else
			{
				printToBuffer( "<!DOCTYPE ", 11, buf);
				printToBuffer( rootid, std::strlen( rootid), buf);
				printToBuffer( ">", 2, buf);
			}
		}
		return true;
	}

	/// \brief Close the current tag attribute context opened
	/// \param [out] buf buffer to print to
	/// \return true on success, false if failed (check lasterror())
	bool exitTagContext( BufferType& buf)
	{
		if (m_state != Content)
		{
			if (m_state == Init)
			{
				m_lasterror = "printed xml without root element";
				return false;
			}
			printToBuffer( '>', buf);
			m_state = Content;
		}
		return true;
	}

	/// \brief Print the start of an open tag
	/// \param [in] src start of the tag name
	/// \param [in] srcsize length of the tag name in bytes
	/// \param [out] buf buffer to print to
	/// \return true on success, false if failed (check lasterror())
	bool printOpenTag( const char* src, std::size_t srcsize, BufferType& buf)
	{
		if (!exitTagContext( buf)) return false;
		printToBuffer( '<', buf);
		printToBuffer( (const char*)src, srcsize, buf);

		m_tagstack.push( src, srcsize);
		m_state = TagElement;
		return true;
	}

	/// \brief Print the start of an attribute name
	/// \param [in] src start of the attribute name
	/// \param [in] srcsize length of the attribute name in bytes
	/// \param [out] buf buffer to print to
	/// \return true on success, false if failed (check lasterror())
	bool printAttribute( const char* src, std::size_t srcsize, BufferType& buf)
	{
		if (m_state == TagElement)
		{
			printToBuffer( ' ', buf);
			printToBuffer( (const char*)src, srcsize, buf);
			printToBuffer( '=', buf);
			m_state = TagAttribute;
			return true;
		}
		return false;
	}

	/// \brief Print a content or attribute value depending on context
	/// \param [in] src start of the value
	/// \param [in] srcsize length of the value in bytes
	/// \param [out] buf buffer to print to
	/// \return true on success, false if failed (check lasterror())
	bool printValue( const char* src, std::size_t srcsize, BufferType& buf)
	{
		if (m_state == TagAttribute)
		{
			printToBufferAttributeValue( (const char*)src, srcsize, buf);
			m_state = TagElement;
		}
		else
		{
			if (!exitTagContext( buf)) return false;
			printToBufferContent( (const char*)src, srcsize, buf);
		}
		return true;
	}

	/// \brief Print the close of the current tag open
	/// \param [out] buf buffer to print to
	/// \return true on success, false if failed (check lasterror())
	bool printCloseTag( BufferType& buf)
	{
		const void* cltag;
		std::size_t cltagsize;

		if (!m_tagstack.top( cltag, cltagsize) || !cltagsize)
		{
			return false;
		}
		if (m_state == TagElement)
		{
			printToBuffer( '/', buf);
			printToBuffer( '>', buf);
			m_state = Content;
		}
		else if (m_state != Content)
		{
			return false;
		}
		else
		{
			printToBuffer( '<', buf);
			printToBuffer( '/', buf);
			printToBuffer( (const char*)cltag, cltagsize, buf);
			printToBuffer( '>', buf);
		}
		m_tagstack.pop();
		if (m_tagstack.empty())
		{
			printToBuffer( '\n', buf);
		}
		return true;
	}

	/// \brief Internal state
	enum State
	{
		Init,
		Content,
		TagAttribute,
		TagElement
	};

	/// \brief Get the current internal state
	/// \return the current state
	State state() const
	{
		return m_state;
	}

	/// \brief Get the last error occurred
	/// \return the last error string
	const char* lasterror() const
	{
		return m_lasterror.empty()?0:m_lasterror.c_str();
	}

private:
	State m_state;					///< internal state
	BufferType m_buf;				///< element output buffer
	TagStack m_tagstack;				///< tag name stack of open tags
	IOCharset m_output;				///< output character set encoding
	std::string m_lasterror;			///< the last error occurred
};

} //namespace
#endif
