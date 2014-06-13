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
/// \file textwolf/xmlhdrparser.hpp
/// \brief Class for parsing the header to get the character set encoding

#ifndef __TEXTWOLF_XML_HEADER_PARSER_HPP__
#define __TEXTWOLF_XML_HEADER_PARSER_HPP__
#include <cstdlib>
#include "textwolf/sourceiterator.hpp"

/// \namespace textwolf
/// \brief Toplevel namespace of the library
namespace textwolf {

/// \class XmlHdrParser
/// \brief Class for parsing the header to get the character set encoding
/// \remark Works with all single byte or multibyte character sets with ASCII as base
class XmlHdrParser
{
public:
	/// \brief Constructor
	XmlHdrParser()
		:m_state(Init)
		,m_attributetype(Encoding)
		,m_idx(0)
		,m_charsConsumed(0)
		,m_zeroCount(0){}

	/// \brief Copy constructor
	/// \brief param[in] o object to copy
	XmlHdrParser( const XmlHdrParser& o)
		:m_state(o.m_state)
		,m_attributetype(o.m_attributetype)
		,m_idx(o.m_idx)
		,m_charsConsumed(o.m_charsConsumed)
		,m_zeroCount(o.m_zeroCount)
		,m_item(o.m_item)
		,m_src(o.m_src){}


	/// \brief Add another input chunk to process
	/// \param[in] src_ pointer to chunk 
	/// \param[in] srcsize_ size of chunk in bytes
	void putInput( const char* src_, std::size_t srcsize_)
	{
		m_src.append( src_, srcsize_);
	}

	/// \brief Get the whole original data added with subsequent calls of putInput(const char*,std::size_t)
	/// \return the data block as string reference
	const std::string& consumedData() const
	{
		return m_src;
	}

	/// \brief Call the first/next iteration of parsing the header
	/// \return true on success, false if more data is needed (putInput(const char*,std::size_t)) or if an error occurred. Check lasterror() for an error
	bool parse()
	{
		unsigned char ch = nextChar();
		for (;ch != 0; ch = nextChar())
		{
			switch (m_state)
			{
				case Init:
					if (ch == '<')
					{
						m_state = ParseXmlOpen;
					}
					else if (ch <= 32)
					{
						continue;
					}
					else
					{
						setError( "expected open tag angle bracket '>'");
						return false;
					}
					break;

				case ParseXmlOpen:
					if (ch == '?')
					{
						m_state = ParseXmlHdr;
					}
					else if (ch <= 32)
					{
						break;
					}
					else if (((ch|32) >= 'a' && (ch|32) <= 'z') || ch == '_')
					{
						return true;
					}
					else
					{
						setError( "expected xml header question mark '?' after open tag angle bracket '<'");
						return false;
					}
					break;

				case ParseXmlHdr:
					if (ch <= 32 || ch == '?')
					{
						if (m_item != "xml")
						{
							setError( "expected '<?xml' as xml header start");
							return false;
						}
						m_item.clear();
						if (ch == '?') return true; /*...."<?xml?>"*/

						m_state = FindAttributeName;
					}
					else if (((ch|32) >= 'a' && (ch|32) <= 'z') || ch == '_')
					{
						m_item.push_back(ch);
						continue;
					}
					else if (ch == '>')
					{
						setError( "unexpected close angle bracket '>' in xml header after '<?xml'");
						return false;
					}
					else
					{
						setError( "expected '<?xml' as xml header start (invalid character)");
						return false;
					}
					break;

				case FindAttributeName:
					if (ch <= 32)
					{
						continue;
					}
					else if (ch == '>' || ch == '?')
					{
						if (ch == '>')
						{
							setError( "unexpected close angle bracket '>' in xml header (missing '?')");
							return false;
						}
						return true;
					}
					else if (((ch|32) >= 'a' && (ch|32) <= 'z') || ch == '_')
					{
						m_item.push_back(ch);
						m_state = ParseAttributeName;
					}
					else
					{
						setError( "invalid character in xml header attribute name");
						return false;
					}
					break;
				case ParseAttributeName:
					if (ch <= 32 || ch == '=')
					{
						if (m_item == "encoding")
						{
							m_attributetype = Encoding;
						}
						else if (m_item == "version")
						{
							m_attributetype = Version;
						}
						else if (m_item == "standalone")
						{
							m_attributetype = Standalone;
						}
						else
						{
							setError( "unknown xml header attribute name");
							return false;
						}
						m_item.clear();
						if (ch == '=')
						{
							m_state = FindAttributeValue;
							continue;
						}
						m_state = FindAttributeAssign;
					}
					else if (((ch|32) >= 'a' && (ch|32) <= 'z') || ch == '_')
					{
						m_item.push_back(ch);
						continue;
					}
					else
					{
						setError( "invalid character in xml header attribute name");
						return false;
					}
					break;
				case FindAttributeAssign:
					if (ch == '=')
					{
						m_state = FindAttributeValue;
					}
					else if (ch <= 32)
					{
						continue;
					}
					else
					{
						setError( "expected '=' after xml header attribute name");
						return false;
					}
					break;
				case FindAttributeValue:
					if (ch == '"')
					{
						m_state = ParseAttributeValueDq;
						continue;
					}
					else if (ch == '\'')
					{
						m_state = ParseAttributeValueSq;
						continue;
					}
					else if (ch <= 32)
					{
						continue;
					}
					else
					{
						setError( "expected single or double quote string as xml header attribute value");
						return false;
					}
					break;
				case ParseAttributeValueSq:
					if (ch == '\'')
					{
						switch (m_attributetype)
						{
							case Encoding:
								m_encoding = m_item;
								break;
							case Version:
							case Standalone:
								break;
						}
						m_item.clear();
						m_state = FindAttributeName;
						continue;
					}
					else
					{
						m_item.push_back( ch);
					}
					break;
				case ParseAttributeValueDq:
					if (ch == '\"')
					{
						switch (m_attributetype)
						{
							case Encoding:
								m_encoding = m_item;
								break;
							case Version:
							case Standalone:
								break;
						}
						m_item.clear();
						m_state = FindAttributeName;
						continue;
					}
					else
					{
						m_item.push_back( ch);
					}
					break;
			}/*switch(..)*/
		}/*for(;..;..)*/
		return false;
	}

	/// \brief Get the last error occurred
	/// \return the pointer to the last error or NULL if no error occurred
	const char* lasterror() const
	{
		return m_lasterror.empty()?0:m_lasterror.c_str();
	}

	/// \brief Get the encoding specified as attribute in the header
	/// \return the encoding or NULL if not specified or not encountered yet in the source parsed
	const char* encoding() const
	{
		return m_encoding.empty()?0:m_encoding.c_str();
	}

	/// \brief Get the number of ASCII characters consumed
	/// \return the number of characters
	std::size_t charsConsumed() const
	{
		return m_charsConsumed;
	}

	/// \brief Clear the data, reset the state
	void clear()
	{
		m_state = Init;
		m_attributetype = Encoding;
		m_idx = 0;
		m_charsConsumed = 0;
		m_zeroCount = 0;
		m_item.clear();
		m_src.clear();
		m_encoding.clear();
		m_lasterror.clear();
	}

private:
	void setError( const std::string& m)
	{
		m_lasterror = m;
	}

	unsigned char nextChar()
	{
		for (; m_zeroCount<4; m_zeroCount++)
		{
			if (m_idx >= m_src.size()) return 0;
			unsigned char ch = m_src[m_idx];
			++m_idx;
			if (ch != 0)
			{
				m_zeroCount = 0;
				if (ch > 32)
				{
					++m_charsConsumed;
				}
				return ch;
			}
		}
		throw std::runtime_error( "illegal XML header (more than 4 null bytes in a row)");
	}

	enum State
	{
		Init,
		ParseXmlOpen,
		ParseXmlHdr,
		FindAttributeName,
		ParseAttributeName,
		FindAttributeAssign,
		FindAttributeValue,
		ParseAttributeValueSq,
		ParseAttributeValueDq
	};

	enum AttributeType
	{
		Encoding,
		Version,
		Standalone
	};

	static const char* stateName( State i)
	{
		static const char* ar[] = {"Init","ParseXmlOpen","ParseXmlHdr","FindAttributeName","ParseAttributeName","FindAttributeAssign","FindAttributeValue","ParseAttributeValueSq","ParseAttributeValueDq"};
		return ar[ (int)i];
	}

private:
	State m_state;			///< header parsing state
	AttributeType m_attributetype;	///< currently parsed attribute type
	std::size_t m_idx;		///< source index (index in m_src)
	std::size_t m_charsConsumed;	///< number of characters consumed
	std::size_t m_zeroCount;	///< counter of subsequent null bytes
	std::string m_item;		///< parsed item
	std::string m_src;		///< source buffered
	std::string m_encoding;		///< character set encoding parsed
	std::string m_lasterror;	///< last error
};

}//namespace
#endif

