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
///\file textwolf/xmlhdrparser.hpp
///\brief Class for parsing the header to get the character set encoding

#ifndef __TEXTWOLF_XML_HEADER_PARSER_HPP__
#define __TEXTWOLF_XML_HEADER_PARSER_HPP__
#include <cstdlib>
#include "textwolf/sourceiterator.hpp"

///\namespace textwolf
///\brief Toplevel namespace of the library
namespace textwolf {

///\class XmlHdrParser
///\brief Class for parsing the header to get the character set encoding
class XmlHdrParser
{
public:
	///\brief Constructor
	XmlHdrParser()
		:m_state(Init)
		,m_idx(0){}

	///\brief Copy constructor
	///\brief param[in] o object to copy
	XmlHdrParser( const XmlHdrParser& o)
		:m_state(o.m_state)
		,m_idx(o.m_idx)
		,m_item(o.m_item)
		,m_src(o.m_src){}


	void feedData( const char* src_, std::size_t srcsize_)
	{
		m_src.append( src_, srcsize_);
	}

	bool parse()
	{
		for (;;)
		{
			unsigned char ch = nextChar();
			if (ch == 0) return false;

			switch (m_state)
			{
				case Init:
					if (ch == '<')
					{
						m_state = 
						return ch;
					}
					else
					{
						skip();
						++m_cnt0;
						continue;
					}

				case Right0:
					if (ch)
					{
						m_state = Src;
						return ch;
					}
					else
					{
						skip();
						++m_cnt0;
						continue;
					}

				case Src:
					if (ch)
					{
						if (ch == '>')
						{
							skip();
							m_state = Rest;
							continue;
						}
						return ch;
					}
					else
					{
						skip();
						continue;
					}

				case Rest:
					while (m_cnt0 > 0)
					{
						if (cur()) throw std::runtime_error( "illegal xml header");
						skip();
						--m_cnt0;
					}
					m_state = End;
					return '>';

				case End:
					return 0;
			}
		}
	}

private:
	unsigned char nextChar() const
	{
		for (std::size_t ofs=0; ofs<4; ofs++)
		{
			if (m_idx >= m_src.size()) return 0;
			unsigned char ch = m_src[m_idx];
			if (ch <= 127 && ch != 0)
			{
				return ch;
			}
		}
		throw std::runtime_error( "illegal XML header");
	}

	enum State
	{
		Init,
		ParseXmlOpen,
		ParseXmlQuestionMark,
		ParseXmlHdr,
		FindAttributeName,
		ParseAttributeName,
		FindAttributeAssign,
		FindAttributeValue,
		ParseAttributeValueId,
		ParseAttributeValueSq,
		ParseAttributeValueDq
	};

	static const char* stateName( State i)
	{
		static const char* ar[] = {"Left0","Right0","Src","Rest","End"};
		return ar[ (int)i];
	}
	State m_state;			//< header parsing state
	std::size_t m_idx;		//< source index (index in m_src)
	std::string m_item;		//< parsed item
	std::string m_src;		//< source buffered
};

}//namespace
#endif

