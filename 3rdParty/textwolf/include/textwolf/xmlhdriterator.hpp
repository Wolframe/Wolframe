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

    Copyright (C) 2010,2011,2012 Patrick Frey

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
///\file textwolf/xmlhdriterator.hpp
///\brief textwolf XML source iterator template for parsing the header

#ifndef __TEXTWOLF_XML_HEADER_ITERATOR_HPP__
#define __TEXTWOLF_XML_HEADER_ITERATOR_HPP__
#include <cstdlib>
#include "textwolf/sourceiterator.hpp"

///\namespace textwolf
///\brief Toplevel namespace of the library
namespace textwolf {

///\class XmlHdrIterator
///\brief Iterator that parses the header character stream without the NUL characters
class XmlHdrSrcIterator :public SrcIterator
{
public:
	///\brief Constructor
	template <class InputIterator>
	explicit XmlHdrSrcIterator( InputIterator&)
		:m_state(Left0)
		,m_cnt0(0){}

	XmlHdrSrcIterator()
		:m_state(Left0)
		,m_cnt0(0){}

	///\brief Copy constructor
	///\brief param[in] o iterator to copy
	XmlHdrSrcIterator( const XmlHdrSrcIterator& o)
		:SrcIterator(o)
		,m_state(o.m_state)
		,m_cnt0(o.m_cnt0){}

	///\brief Assingment of new chunk
	///\param [in] buf source chunk to iterate on
	///\param [in] size size of source chunk to iterate on in bytes
	///\param [in] eof true, if end of data has been reached (no next chunk anymore)
	XmlHdrSrcIterator& operator=( const SrcIterator& o)
	{
		SrcIterator::operator=(o);
		return *this;
	}

	///\brief Element access
	///\return current character
	char operator* ()
	{
		for (;;)
		{
			if (m_cnt0 >= 4) return 0;
			char ch = cur();
			switch (m_state)
			{
				case Left0:
					if (ch)
					{
						if (m_cnt0)
						{
							m_state = Src;
							m_cnt0 = 0;
						}
						else
						{
							m_state = Right0;
						}
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

	///\brief Preincrement
	XmlHdrSrcIterator& operator++()
	{
		if (m_state != End) skip();
		return *this;
	}

private:
	char cur()	{return SrcIterator::operator*();}
	void skip()	{SrcIterator::operator++();}

	enum State
	{
		Left0,
		Right0,
		Src,
		Rest,
		End
	};

	static const char* stateName( State i)
	{
		static const char* ar[] = {"Left0","Right0","Src","Rest","End"};
		return ar[ (int)i];
	}
	State m_state;			//< header parsing state
	std::size_t m_cnt0;		//< counter of 0
};

}//namespace
#endif

