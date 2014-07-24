/*
---------------------------------------------------------------------
    The template library textwolf implements an input iterator on
    a set of XML path expressions without backward references on an
    STL conforming input iterator as source. It does no buffering
    or read ahead and is dedicated for stream processing of XML
    for a small set of XML queries.
    Stream processing in this context refers to processing the
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
/// \file textwolf/charset_isolatin.hpp
/// \brief Definition of IsoLatin encodings

#ifndef __TEXTWOLF_CHARSET_ISOLATIN_HPP__
#define __TEXTWOLF_CHARSET_ISOLATIN_HPP__
#include "textwolf/char.hpp"
#include "textwolf/charset_interface.hpp"
#include "textwolf/exception.hpp"
#include "textwolf/codepages.hpp"
#include <cstddef>

namespace textwolf {
namespace charset {

/// \class IsoLatin
/// \brief Character set IsoLatin-1,..IsoLatin-9 (ISO-8859-1,...ISO-8859-9)
struct IsoLatin :public IsoLatinCodePage
{
	enum {MaxChar=0xFFU};

	IsoLatin( const IsoLatin& o)
		:IsoLatinCodePage(o){}
	IsoLatin( unsigned int codePageIdx=1)
		:IsoLatinCodePage(codePageIdx){}

	/// \brief See template<class Iterator>Interface::skip(char*,unsigned int&,Iterator&)
	template <class Iterator>
	static inline void skip( char*, unsigned int& bufpos, Iterator& itr)
	{
		if (bufpos==0)
		{
			++itr;
			++bufpos;
		}
	}

	/// \brief See template<class Iterator>Interface::fetchbytes(char*,unsigned int&,Iterator&)
	template <class Iterator>
	static inline void fetchbytes( char* buf, unsigned int& bufpos, Iterator& itr)
	{
		if (bufpos==0)
		{
			buf[0] = *itr;
			++itr;
			++bufpos;
		}
	}

	/// \brief See template<class Iterator>Interface::asciichar(char*,unsigned int&,Iterator&)
	template <class Iterator>
	static inline signed char asciichar( char* buf, unsigned int& bufpos, Iterator& itr)
	{
		fetchbytes( buf, bufpos, itr);
		return ((unsigned char)(buf[0])>127)?-1:buf[0];
	}

	/// \brief See template<class Iterator>Interface::value(char*,unsigned int&,Iterator&)
	template <class Iterator>
	inline UChar value( char* buf, unsigned int& bufpos, Iterator& itr) const
	{
		fetchbytes( buf, bufpos, itr);
		return ucharcode( buf[0]);
	}

	/// \brief See template<class Buffer>Interface::print(UChar,Buffer&)
	template <class Buffer_>
	void print( UChar chr, Buffer_& buf) const
	{
		char chr_ = invcode( chr);
		if (chr_ == 0)
		{
			char tb[ 32];
			char* cc = tb;
			Encoder::encode( chr, tb, sizeof(tb));
			while (*cc) buf.push_back( *cc++);
		}
		else
		{
			buf.push_back( chr_);
		}
	}

	/// \brief See template<class Buffer>Interface::is_equal( const Interface&, const Interface&)
	static inline bool is_equal( const IsoLatin& a, const IsoLatin& b)
	{
		return IsoLatinCodePage::is_equal( a, b);
	}
};

}//namespace
}//namespace
#endif
