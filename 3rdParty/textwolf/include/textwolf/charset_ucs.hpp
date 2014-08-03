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
/// \file textwolf/charset_ucs.hpp
/// \brief Definition of UCS-2/UCS-4 encodings

#ifndef __TEXTWOLF_CHARSET_UCS_HPP__
#define __TEXTWOLF_CHARSET_UCS_HPP__
#include "textwolf/char.hpp"
#include "textwolf/charset_interface.hpp"
#include "textwolf/exception.hpp"
#include <cstddef>

namespace textwolf {
namespace charset {

/// \class UCS2
/// \brief Character set UCS-2 (little/big endian)
/// \tparam byteorder charset::ByteOrder::LE or charset::ByteOrder::BE
///   UCS-2 encoding is defined to be big-endian only. Although the similar designations 'UCS-2BE and UCS-2LE
///   imitate the UTF-16 labels, they do not represent official encoding schemes. (http://en.wikipedia.org/wiki/UTF-16/UCS-2)
///   therefore we take byteorder=ByteOrder::BE as default.
template <int byteorder=ByteOrder::BE>
struct UCS2
{
	enum
	{
		LSB=(byteorder==ByteOrder::BE),			//< least significant byte index (0 or 1)
		MSB=(byteorder==ByteOrder::LE),			//< most significant byte index (0 or 1)
		Print1shift=(byteorder==ByteOrder::BE)?8:0,	//< value to shift with to get the 1st character to print
		Print2shift=(byteorder==ByteOrder::LE)?8:0,	//< value to shift with to get the 2nd character to print
		MaxChar=0xFFFFU
	};

	/// \brief See template<class Iterator>Interface::skip(char*,unsigned int&,Iterator&)
	template <class Iterator>
	static inline void skip( char*, unsigned int& bufpos, Iterator& itr)
	{
		for (;bufpos < 2; ++bufpos)
		{
			++itr;
		}
	}

	/// \brief See template<class Iterator>Interface::fetchbytes(char*,unsigned int&,Iterator&)
	template <class Iterator>
	static inline void fetchbytes( char* buf, unsigned int& bufpos, Iterator& itr)
	{
		if (bufpos<2)
		{
			if (bufpos<1)
			{
				buf[0] = *itr;
				++itr;
				++bufpos;
			}
			buf[1] = *itr;
			++itr;
			++bufpos;
		}
	}

	template <class Iterator>
	static inline UChar value_impl( char* buf, unsigned int& bufpos, Iterator& itr)
	{
		fetchbytes( buf, bufpos, itr);
		UChar res = (unsigned char)buf[MSB];
		return (res << 8) + (unsigned char)buf[LSB];
	}

	/// \brief See template<class Iterator>Interface::value(char*,unsigned int&,Iterator&)
	template <class Iterator>
	inline UChar value( char* buf, unsigned int& bufpos, Iterator& itr) const
	{
		return value_impl( buf, bufpos, itr);
	}

	/// \brief See template<class Iterator>Interface::value(char*,unsigned int&,Iterator&)
	template <class Iterator>
	static inline signed char asciichar( char* buf, unsigned int& bufpos, Iterator& itr)
	{
		UChar ch = value_impl( buf, bufpos, itr);
		return (ch > 127)?-1:(char)ch;
	}

	/// \brief See template<class Buffer>Interface::print(UChar,Buffer&)
	template <class Buffer_>
	inline void print( UChar chr, Buffer_& buf) const
	{
		if (chr>MaxChar)
		{
			char tb[ 32];
			char* cc = tb;
			Encoder::encode( chr, tb, sizeof(tb));
			while (*cc)
			{
				buf.push_back( (UChar)*cc >> Print1shift);
				buf.push_back( (UChar)*cc >> Print2shift);
				++cc;
			}
		}
		else
		{
			buf.push_back( (unsigned char)(chr >> Print1shift));
			buf.push_back( (unsigned char)(chr >> Print2shift));
		}
	}

	/// \brief See template<class Buffer>Interface::is_equal( const Interface&, const Interface&)
	static inline bool is_equal( const UCS2&, const UCS2&)
	{
		return true;
	}
};

/// \class UCS4
/// \brief Character set UCS-4 (little/big endian)
/// \tparam byteorder ByteOrder::LE or ByteOrder::BE
template <int byteorder>
struct UCS4
{
	enum
	{
		B0=(byteorder==ByteOrder::BE)?3:0,
		B1=(byteorder==ByteOrder::BE)?2:1,
		B2=(byteorder==ByteOrder::BE)?1:2,
		B3=(byteorder==ByteOrder::BE)?0:3,
		Print1shift=(byteorder==ByteOrder::BE)?24:0,	//< value to shift with to get the 1st character to print
		Print2shift=(byteorder==ByteOrder::BE)?16:8,	//< value to shift with to get the 2nd character to print
		Print3shift=(byteorder==ByteOrder::BE)?8:16,	//< value to shift with to get the 3rd character to print
		Print4shift=(byteorder==ByteOrder::BE)?0:24,	//< value to shift with to get the 4th character to print
		MaxChar=0xFFFFFFFFU
	};

	/// \brief See template<class Iterator>Interface::fetchbytes(char*,unsigned int&,Iterator&)
	template <class Iterator>
	static inline void fetchbytes( char* buf, unsigned int& bufpos, Iterator& itr)
	{
		for (;bufpos < 4; ++bufpos)
		{
			buf[ bufpos] = *itr;
			++itr;
		}
	}

	/// \brief See template<class Iterator>Interface::value(char*,unsigned int&,Iterator&)
	template <class Iterator>
	static inline UChar value( char* buf, unsigned int& bufpos, Iterator& itr)
	{
		fetchbytes( buf, bufpos, itr);
		UChar res = (unsigned char)buf[B3];
		res = (res << 8) + (unsigned char)buf[B2];
		res = (res << 8) + (unsigned char)buf[B1];
		return (res << 8) + (unsigned char)buf[B0];
	}

	/// \brief See template<class Iterator>Interface::skip(char*,unsigned int&,Iterator&)
	template <class Iterator>
	static inline void skip( char*, unsigned int& bufpos, Iterator& itr)
	{
		for (;bufpos < 4; ++bufpos)
		{
			++itr;
		}
	}

	/// \brief See template<class Iterator>Interface::asciichar(char*,unsigned int&,Iterator&)
	template <class Iterator>
	static inline signed char asciichar( char* buf, unsigned int& bufpos, Iterator& itr)
	{
		UChar ch = value( buf, bufpos, itr);
		return (ch > 127)?-1:(char)ch;
	}

	/// \brief See template<class Buffer>Interface::print(UChar,Buffer&)
	template <class Buffer_>
	static void print( UChar chr, Buffer_& buf)
	{
		buf.push_back( (unsigned char)((chr >> Print1shift) & 0xFF));
		buf.push_back( (unsigned char)((chr >> Print2shift) & 0xFF));
		buf.push_back( (unsigned char)((chr >> Print3shift) & 0xFF));
		buf.push_back( (unsigned char)((chr >> Print4shift) & 0xFF));
	}

	/// \brief See template<class Buffer>Interface::is_equal( const Interface&, const Interface&)
	static inline bool is_equal( const UCS4&, const UCS4&)
	{
		return true;
	}
};

/// \class UCS2LE
/// \brief UCS-2 little endian character set encoding
struct UCS2LE :public UCS2<ByteOrder::LE> {};
/// \class UCS2BE
/// \brief UCS-2 big endian character set encoding
struct UCS2BE :public UCS2<ByteOrder::BE> {};
/// \class UCS4LE
/// \brief UCS-4 little endian character set encoding
struct UCS4LE :public UCS4<ByteOrder::LE> {};
/// \class UCS4BE
/// \brief UCS-4 big endian character set encoding
struct UCS4BE :public UCS4<ByteOrder::BE> {};

}//namespace
}//namespace
#endif
