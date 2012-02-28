/**
---------------------------------------------------------------------
    The template library textwolf implements an input iterator on
    a set of XML path expressions without backward references on an
    STL conforming input iterator as source. It does no buffering
    or read ahead and is dedicated for stream processing of XML
    for a small set of XML queries.
    Stream processing in this context refers to processing the
    document without buffering anything but the current result token
    processed with its tag hierarchy information.

    Copyright (C) 2010 Patrick Frey

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

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
**/

#ifndef __TEXTWOLF_HPP__
#define __TEXTWOLF_HPP__
#include <iterator>
#include <vector>
#include <stack>
#include <map>
#include <exception>
#include <limits>
#include <cstddef>
#include <cstring>

#ifdef BOOST_VERSION
#include <boost/cstdint.hpp>
namespace textwolf {
	///\typedef UChar
	///\brief Unicode character type
	typedef boost::uint32_t UChar;
	typedef boost::uint64_t EChar;
}//namespace
#else
#ifdef _MSC_VER
#pragma warning(disable:4290)
#include <BaseTsd.h>
namespace textwolf {
	///\typedef UChar
	///\brief Unicode character type
	typedef DWORD32 UChar;
	typedef DWORD64 EChar;
}//namespace
#else
#include <stdint.h>
namespace textwolf {
	///\typedef UChar
	///\brief Unicode character type
	typedef uint32_t UChar;
	typedef uint64_t EChar;
}//namespace
#endif
#endif

///\namespace textwolf
///\brief Toplevel namespace of the library
namespace textwolf {

///\defgroup Exceptions Exceptions
///\brief Exception classes and structures for error handling
/*! \addtogroup Exceptions
 *  @{
*/

///\class throws_exception
///\brief Base class for structures that can throw exceptions for non recoverable errors
struct throws_exception
{
	///\enum Cause
	///\brief Enumeration of error cases
	enum Cause
	{
		Unknown,			///< uknown error
		DimOutOfRange,			///< memory reserved for statically allocated table or memory block is too small. Increase the size of memory block passed to the XML path select automaton. Usage error !
		StateNumbersNotAscending,	///< XML scanner automaton definition check failed. Labels of states must be equal to their indices. Internal textwold error !
		InvalidParam,			///< parameter check in automaton definition failed. Internal textwold error !
		InvalidState,			///< invalied state definition in automaton. Internal textwold error !
		IllegalParam,			///< parameter check in automaton definition failed. Internal textwold error !
		IllegalAttributeName,		///< invalid string for a tag or attribute in the automaton definition. Usage error !
		OutOfMem,			///< out of memory in the automaton definition. System error (std::bad_alloc) !
		ArrayBoundsReadWrite,		///< invalid array access. Internal textwold error !
		NotAllowedOperation		///< defining an operation in an automaton definition that is not allowed there. Usage error !
	};
};

///\class exception
///\brief textwolf exception class
struct exception	:public std::exception
{
	typedef throws_exception::Cause Cause;
	Cause cause;										///< exception cause tag

	///\brief Constructor
	///\return exception object
	exception (Cause p_cause) throw()
			:cause(p_cause) {}
	///\brief Copy constructor
	exception (const exception& orig) throw()
			:cause(orig.cause) {}
	///\brief Destructor
	virtual ~exception() throw() {}

	///\brief Assignement
	///\param[in] orig exception to copy
	///\return *this
	exception& operator= (const exception& orig) throw()
			{cause=orig.cause; return *this;}

	///\brief Exception message
	///\return exception cause as string
	virtual const char* what() const throw()
	{
		// enumeration of exception causes as strings
		static const char* nameCause[ 10] = {
			"Unknown","DimOutOfRange","StateNumbersNotAscending","InvalidParam",
			"InvalidState","IllegalParam","IllegalAttributeName","OutOfMem",
			"ArrayBoundsReadWrite","NotAllowedOperation"
		};
		return nameCause[ (unsigned int) cause];
	}
};


/*! @} */
///\defgroup Utilities Some Helper Structures and Functions
///\brief Implements some helpers
/*! \addtogroup Utilities
 *  @{
*/
///
/// \class StaticBuffer
/// \brief Simple back insertion sequence for storing the outputs of textwolf in a contant size buffer
///
class StaticBuffer :public throws_exception
{
public:
	///\brief Constructor
	explicit StaticBuffer( std::size_t n)
		:m_pos(0),m_size(n),m_ar(0),m_allocated(true) {m_ar=new char[n];}

	///\brief Constructor
	StaticBuffer( char* p, std::size_t n, std::size_t i=0)
		:m_pos(i),m_size(n),m_ar(p),m_allocated(false),m_overflow(false) {}

	///\brief Destructor
	~StaticBuffer()
		{if (m_allocated) delete [] m_ar;}

	///\brief Clear the buffer content
	void clear()				{m_pos=0;m_overflow=false;}

	///\brief Append one character
	///\param[in] ch the character to append
	void push_back( char ch)
	{
		if (m_pos < m_size)
		{
			m_ar[m_pos++] = ch;
		}
		else
		{
			m_overflow = true;
		}
	}

	///\brief Append an array of characters
	///\param[in] cc the characters to append
	///\param[in] ccsize the number of characters to append
	void append( const char* cc, std::size_t ccsize)
	{
		if (m_pos+ccsize > m_size)
		{
			m_overflow = true;
			ccsize = m_size - m_pos;
		}
		std::memcpy( m_ar+m_pos, cc, ccsize);
		m_pos += ccsize;
	}

	///\brief Return the number of characters in the buffer
	///\return the number of characters (bytes)
	std::size_t size() const		{return m_pos;}

	///\brief Return the buffer content as 0-terminated string
	///\return the C-string
	const char* ptr() const			{return m_ar;}

	///\brief Shrinks the size of the buffer or expands it with c
	///\param [in] n new size of the buffer
	///\param [in] c fill character if n bigger than the current fill size
	void resize( std::size_t n, char c=0)
	{
		if (m_pos>n)
		{
			m_pos=n;
		}
		else
		{
			if (m_size<n) n=m_size;
			while (n>m_pos) push_back(c);
		}
	}

	///\brief random access of element
	///\param [in] ii
	///\return the character at this position
	char operator []( std::size_t ii) const
	{
		if (ii > m_pos) throw exception( DimOutOfRange);
		return m_ar[ii];
	}

	///\brief random access of element reference
	///\param [in] ii
	///\return the reference to the character at this position
	char& at( std::size_t ii) const
	{
		if (ii > m_pos) throw exception( DimOutOfRange);
		return m_ar[ii];
	}

	///\brief check for array bounds write
	///\return true if a push_back would have caused an array bounds write
	bool overflow() const			{return m_overflow;}
private:
	std::size_t m_pos;			///< current cursor position of the buffer (number of added characters)
	std::size_t m_size;			///< allocation size of the buffer in bytes
	char* m_ar;				///< buffer content
	bool m_allocated;			///< true, if the buffer is allocated by this class and not passed by constructor
	bool m_overflow;			///< true, if an array bounds write would have happened with push_back
};

/*! @} */
///\defgroup Charactersets Character Set Encodings
///\brief Character set encodings and character parsing tables
/*! \addtogroup Charactersets
 *  @{
*/

///\class CharMap
///\brief Character map for fast typing of a character byte
///\tparam RESTYPE result type of the map
///\tparam nullvalue_ default intitialization value of the map
///\tparam RANGE domain of the input values of the map
template <typename RESTYPE, RESTYPE nullvalue_, int RANGE=256>
class CharMap
{
public:
	typedef RESTYPE valuetype;
	enum Constant {nullvalue=nullvalue_};

private:
	RESTYPE ar[ RANGE];		///< the map elements
public:
	///\brief Constructor
	CharMap()									{for (unsigned int ii=0; ii<RANGE; ii++) ar[ii]=(valuetype)nullvalue;}
	///\brief Define the values of the elements in the interval [from,to]
	///\param[in] from start of the input intervall (belongs also to the input)
	///\param[in] to end of the input intervall (belongs also to the input)
	///\param[in] value value assigned to all elements in  [from,to]
	CharMap& operator()( unsigned char from, unsigned char to, valuetype value)	{for (unsigned int ii=from; ii<=to; ii++) ar[ii]=value; return *this;}
	///\brief Define the values of the single element at 'at'
	///\param[in] at the input element
	///\param[in] value value assigned to the element 'at'
	CharMap& operator()( unsigned char at, valuetype value)				{ar[at] = value; return *this;}
	///\brief Read the element assigned to 'ii'
	///\param[in] ii the input element queried
	///\return the element at 'ii'
	valuetype operator []( unsigned char ii) const					{return ar[ii];}
};

///\namespace charset
///\brief Predefined character set encodings
///
/// Predefined character set encoding definitions:
/// 1) Iso-Latin-1
/// 2) UCS2   (little or big endian)
/// 3) UCS4   (little or big endian)
/// 4) UTF-8  (see http://de.wikipedia.org/wiki/UTF-8 for algorithms)
/// 5) UTF-16 (little or big endian, no implicit BOM swapping) see http://en.wikipedia.org/wiki/UTF-16/UCS-2 for algorithms)
///
namespace charset {

struct Encoder
{
	///\brief Write the character 'chr' in encoded form  as nul-terminated string to a buffer
	///\param[in] chr unicode character to encode
	///\param[out] bufptr buffer to write to
	///\param[in] bufsize allocation size of buffer pointer by 'bufptr'
	static bool encode( UChar chr, char* bufptr, std::size_t bufsize)
	{
		static const char* HEX = "0123456789abcdef";
		StaticBuffer buf( bufptr, bufsize);
		char bb[ 32];
		unsigned int ii=0;
		while (chr > 0)
		{
			bb[ii++] = HEX[ chr & 0xf];
			chr /= 16;
		}
		buf.push_back( '&');
		buf.push_back( '#');
		buf.push_back( 'x');
		while (ii)
		{
			buf.push_back( bb[ --ii]);
		}
		buf.push_back( '\0');
		return !buf.overflow();
	}
};

///\class Interface
///\brief Interface that has to be implemented for a character set encoding
struct Interface
{
	///\brief Skip to start of the next character
	///\param [in] buf buffer for the character data
	///\param [in,out] bufpos position in 'buf'
	///\param [in,out] itr iterator to skip
	template <class Iterator>
	static void skip( char* buf, unsigned int& bufpos, Iterator& itr);

	///\brief Fetches the ascii char representation of the current character
	///\param [in] buf buffer for the parses character data
	///\param [in,out] bufpos position in 'buf'
	///\param [in,out] itr iterator on the source
	///\return the value of the ascii character or -1
	template <class Iterator>
	static char asciichar( char* buf, unsigned int& bufpos, Iterator& itr);

	///\brief Fetches the unicode character representation of the current character
	///\param [in] buf buffer for the parses character data
	///\param [in,out] bufpos position in 'buf'
	///\param [in,out] itr iterator on the source
	///\return the value of the unicode character
	template <class Iterator>
	static UChar value( char* buf, unsigned int& bufpos, Iterator& itr);

	///\brief Prints a unicode character to a buffer
	///\tparam Buffer_ STL back insertion sequence
	///\param [in] chr character to print
	///\param [out] buf buffer to print to
	template <class Buffer_>
	static void print( UChar chr, Buffer_& buf);
};

///\class IsoLatin1
///\brief Character set IsoLatin-1 (ISO-8859-1)
struct IsoLatin1
{
	enum {MaxChar=0xFF};

	///\brief See template<class Iterator>Interface::skip(char*,unsigned int&,Iterator&)
	template <class Iterator>
	static void skip( char*, unsigned int& bufpos, Iterator& itr)
	{
		if (bufpos==0)
		{
			++itr;
			++bufpos;
		}
	}

	///\brief See template<class Iterator>Interface::asciichar(char*,unsigned int&,Iterator&)
	template <class Iterator>
	static char asciichar( char* buf, unsigned int& bufpos, Iterator& itr)
	{
		if (bufpos==0)
		{
			buf[0] = *itr;
			++itr;
			++bufpos;
		}
		return ((unsigned char)(buf[0])>127)?-1:buf[0];
	}

	///\brief See template<class Iterator>Interface::value(char*,unsigned int&,Iterator&)
	template <class Iterator>
	static UChar value( char* buf, unsigned int& bufpos, Iterator& itr)
	{
		if (bufpos == 0)
		{
			buf[0] = *itr;
			++itr;
			++bufpos;
		}
		return (unsigned char)buf[0];
	}

	///\brief See template<class Buffer>Interface::print(UChar,Buffer&)
	template <class Buffer_>
	static void print( UChar chr, Buffer_& buf)
	{
		char chr_ = (char)(unsigned char)chr;
		if (chr > 255)
		{
			char tb[ 32];
			char* cc = tb;
			Encoder::encode( chr, tb, sizeof(tb));
			while (*cc) buf.push_back( *cc++);
		}
		buf.push_back( chr_);
	}
};

///\class ByteOrder
///\brief Order of bytes for wide char character sets
struct ByteOrder
{
	enum
	{
		LE=0,		///< little endian
		BE=1		///< big endian
	};
};

///\class UCS2
///\brief Character set UCS-2 (little/big endian)
///\tparam encoding charset::ByteOrder::LE or charset::ByteOrder::BE
///\remark UCS-2 encoding is defined to be big-endian only. Although the similar designations UCS-2BE and UCS-2LE 
///  imitate the UTF-16 labels, they do not represent official encoding schemes. (http://en.wikipedia.org/wiki/UTF-16/UCS-2)
///  therefore we take encoding=ByteOrder::BE as default.
template <int encoding=ByteOrder::BE>
struct UCS2
{
	enum
	{
		LSB=(encoding==ByteOrder::BE),			///< least significant byte index (0 or 1)
		MSB=(encoding==ByteOrder::LE),			///< most significant byte index (0 or 1)
		Print1shift=(encoding==ByteOrder::BE)?8:0,	///< value to shift with to get the 1st character to print
		Print2shift=(encoding==ByteOrder::LE)?8:0,	///< value to shift with to get the 2nd character to print
		MaxChar=0xFFFF
	};

	///\brief See template<class Iterator>Interface::skip(char*,unsigned int&,Iterator&)
	template <class Iterator>
	static void skip( char*, unsigned int& bufpos, Iterator& itr)
	{
		for (;bufpos < 2; ++bufpos)
		{
			++itr;
		}
	}

	///\brief See template<class Iterator>Interface::value(char*,unsigned int&,Iterator&)
	template <class Iterator>
	static UChar value( char* buf, unsigned int& bufpos, Iterator& itr)
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
		UChar res = (unsigned char)buf[MSB];
		return (res << 8) + (unsigned char)buf[LSB];
	}

	///\brief See template<class Iterator>Interface::value(char*,unsigned int&,Iterator&)
	template <class Iterator>
	static char asciichar( char* buf, unsigned int& bufpos, Iterator& itr)
	{
		UChar ch = value( buf, bufpos, itr);
		return (ch > 127)?-1:(char)ch;
	}

	///\brief See template<class Buffer>Interface::print(UChar,Buffer&)
	template <class Buffer_>
	static void print( UChar chr, Buffer_& buf)
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
			buf.push_back( chr >> Print1shift);
			buf.push_back( chr >> Print2shift);
		}
	}
};

///\class UCS4
///\brief Character set UCS-4 (little/big endian)
///\tparam encoding ByteOrder::LE or ByteOrder::BE
template <int encoding>
struct UCS4
{
	enum
	{
		B0=(encoding==ByteOrder::BE)?3:0,
		B1=(encoding==ByteOrder::BE)?2:1,
		B2=(encoding==ByteOrder::BE)?1:2,
		B3=(encoding==ByteOrder::BE)?0:3,
		Print1shift=(encoding==ByteOrder::BE)?24:0,	///< value to shift with to get the 1st character to print
		Print2shift=(encoding==ByteOrder::BE)?16:8,	///< value to shift with to get the 2nd character to print
		Print3shift=(encoding==ByteOrder::BE)?8:16,	///< value to shift with to get the 3rd character to print
		Print4shift=(encoding==ByteOrder::BE)?0:24,	///< value to shift with to get the 4th character to print
		MaxChar=0xFFFFFFFF
	};

	///\brief See template<class Iterator>Interface::value(char*,unsigned int&,Iterator&)
	template <class Iterator>
	static UChar value( char* buf, unsigned int& bufpos, Iterator& itr)
	{
		for (;bufpos < 4; ++bufpos)
		{
			buf[ bufpos] = *itr;
			++itr;
		}
		UChar res = (unsigned char)buf[B3];
		res = (res << 8) + (unsigned char)buf[B2];
		res = (res << 8) + (unsigned char)buf[B1];
		return (res << 8) + (unsigned char)buf[B0];
	}

	///\brief See template<class Iterator>Interface::skip(char*,unsigned int&,Iterator&)
	template <class Iterator>
	static void skip( char*, unsigned int& bufpos, Iterator& itr)
	{
		for (;bufpos < 4; ++bufpos)
		{
			++itr;
		}
	}

	///\brief See template<class Iterator>Interface::asciichar(char*,unsigned int&,Iterator&)
	template <class Iterator>
	static char asciichar( char* buf, unsigned int& bufpos, Iterator& itr)
	{
		UChar ch = value( buf, bufpos, itr);
		return (ch > 127)?-1:(char)ch;
	}

	///\brief See template<class Buffer>Interface::print(UChar,Buffer&)
	template <class Buffer_>
	static void print( UChar chr, Buffer_& buf)
	{
		buf.push_back( (unsigned char)((chr >> Print1shift) & 0xFF));
		buf.push_back( (unsigned char)((chr >> Print2shift) & 0xFF));
		buf.push_back( (unsigned char)((chr >> Print3shift) & 0xFF));
		buf.push_back( (unsigned char)((chr >> Print4shift) & 0xFF));
	}
};

///\class UCS2LE
///\brief UCS-2 little endian character set encoding
struct UCS2LE :public UCS2<ByteOrder::LE> {};
///\class UCS2BE
///\brief UCS-2 big endian character set encoding
struct UCS2BE :public UCS2<ByteOrder::BE> {};
///\class UCS4BE
///\brief UCS-4 little endian character set encoding
struct UCS4LE :public UCS4<ByteOrder::LE> {};
///\class UCS4BE
///\brief UCS-4 big endian character set encoding
struct UCS4BE :public UCS4<ByteOrder::BE> {};

///\class UTF8
///\brief character set encoding UTF-8
struct UTF8
{
	enum {MaxChar=0xFFFFFFFF};
	enum {
		B11111111=0xFF,
		B01111111=0x7F,
		B00111111=0x3F,
		B00011111=0x1F,
		B00001111=0x0F,
		B00000111=0x07,
		B00000011=0x03,
		B00000001=0x01,
		B00000000=0x00,
		B10000000=0x80,
		B11000000=0xC0,
		B11100000=0xE0,
		B11110000=0xF0,
		B11111000=0xF8,
		B11111100=0xFC,
		B11111110=0xFE,

		B11011111=B11000000|B00011111,
		B11101111=B11100000|B00001111,
		B11110111=B11110000|B00000111,
		B11111011=B11111000|B00000011,
		B11111101=B11111100|B00000001
	};

	struct CharLengthTab	:public CharMap<unsigned char, 0>
	{
		CharLengthTab()
		{
			(*this)
			(B00000000,B01111111,1)
			(B11000000,B11011111,2)
			(B11100000,B11101111,3)
			(B11110000,B11110111,4)
			(B11111000,B11111011,5)
			(B11111100,B11111101,6)
			(B11111110,B11111110,7)
			(B11111111,B11111111,8);
		};
	};

	///\brief Get the size of the current character in bytes (variable length encoding)
	///\param [in] buf buffer for the character data
	///\param [in,out] bufpos position in 'buf'
	///\param [in,out] itr iterator to skip
	template <class Iterator>
	static unsigned int size( char* buf, unsigned int& bufpos, Iterator& itr)
	{
		static CharLengthTab charLengthTab;
		if (bufpos==0)
		{
			buf[0] = *itr;
			++itr;
			++bufpos;
		}
		return charLengthTab[ (unsigned char)buf[ 0]];
	}

	///\brief See template<class Iterator>Interface::skip(char*,unsigned int&,Iterator&)
	template <class Iterator>
	static void skip( char* buf, unsigned int& bufpos, Iterator& itr)
	{
		unsigned int bufsize = size( buf, bufpos, itr);
		for (;bufpos < bufsize; ++bufpos)
		{
			++itr;
		}
	}

	///\brief See template<class Iterator>Interface::asciichar(char*,unsigned int&,Iterator&)
	template <class Iterator>
	static char asciichar( char* buf, unsigned int& bufpos, Iterator& itr)
	{
		if (bufpos==0)
		{
			buf[0] = *itr;
			++itr;
			++bufpos;
		}
		return ((unsigned char)(buf[0])>127)?-1:buf[0];
	}

	///\brief See template<class Iterator>Interface::value(char*,unsigned int&,Iterator&)
	template <class Iterator>
	static UChar value( char* buf, unsigned int& bufpos, Iterator& itr)
	{
		if (bufpos==0)
		{
			buf[0] = *itr;
			++itr;
			++bufpos;
		}
		unsigned int bufsize = size( buf, bufpos, itr);
		for (;bufpos < bufsize; ++bufpos)
		{
			buf[ bufpos] = *itr;
			++itr;
		}
		UChar res = (unsigned char)buf[0];
		if (res > 127)
		{
			int gg = bufsize-2;
			if (gg < 0) return MaxChar;

			res = ((unsigned char)buf[0])&(B00011111>>gg);
			for (int ii=0; ii<=gg; ii++)
			{
				unsigned char xx = (unsigned char)buf[ii+1];
				res = (res<<6) | (xx & B00111111);
				if ((unsigned char)(xx & B11000000) != B10000000)
				{
					return MaxChar;
				}
			}
		}
		return res;
	}

	///\brief See template<class Buffer>Interface::print(UChar,Buffer&)
	template <class Buffer_>
	static void print( UChar chr, Buffer_& buf)
	{
		unsigned int rt;
		if (chr <= 127)
		{
			buf.push_back( (char)(unsigned char)chr);
			return;
		}
		unsigned int pp,sf;
		for (pp=1,sf=5; pp<5; pp++,sf+=5)
		{
			if (chr < (unsigned int)((1<<6)<<sf)) break;
		}
		rt = pp+1;
		unsigned char HB = (unsigned char)(B11111111 << (8-rt));
		unsigned char shf = (unsigned char)(pp*6);
		unsigned int ii;
		buf.push_back( (char)(((unsigned char)(chr >> shf) & (~HB >> 1)) | HB));
		for (ii=1,shf-=6; ii<=pp; shf-=6,ii++)
		{
			buf.push_back( (char)(unsigned char) (((chr >> shf) & B00111111) | B10000000));
		}
	}
};

///\class UTF16
///\brief Character set UTF16 (little/big endian)
///\tparam encoding ByteOrder::LE or ByteOrder::BE
///\remark BOM character sequences are not interpreted as such and byte swapping is not done implicitely
///	It is left to the caller to detect BOM or its inverse and to switch the iterator.
///\remark See http://en.wikipedia.org/wiki/UTF-16/UCS-2: ... If the endian architecture of the decoder 
///	matches that of the encoder, the decoder detects the 0xFEFF value, but an opposite-endian decoder 
///	interprets the BOM as the non-character value U+FFFE reserved for this purpose. This incorrect
///	result provides a hint to perform byte-swapping for the remaining values. If the BOM is missing,
///	the standard says that big-endian encoding should be assumed....
template <int encoding=ByteOrder::BE>
class UTF16
{
private:
	enum
	{
		LSB=(encoding==ByteOrder::BE),			///< least significant byte index (0 or 1)
		MSB=(encoding==ByteOrder::LE),			///< most significant byte index (0 or 1)
		Print1shift=(encoding==ByteOrder::BE)?8:0,	///< value to shift with to get the 1st character to print
		Print2shift=(encoding==ByteOrder::LE)?8:0,	///< value to shift with to get the 2nd character to print
		MaxChar=0x10FFFF
	};
public:
	///\brief Get the size of the current character in bytes (variable length encoding)
	///\param [in] buf buffer for the character data
	///\param [in,out] bufpos position in 'buf'
	///\param [in,out] itr iterator to skip
	template <class Iterator>
	static unsigned int size( char* buf, unsigned int& bufpos, Iterator& itr)
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
		UChar rt = (unsigned char)buf[ MSB];
		if ((rt - 0xD8) > 0x03)
		{
			return 2;
		}
		else
		{
			return 4;
		}
	}

	///\brief See template<class Iterator>Interface::skip(char*,unsigned int&,Iterator&)
	template <class Iterator>
	static void skip( char* buf, unsigned int& bufpos, Iterator& itr)
	{
		unsigned int bufsize = size( buf, bufpos, itr);
		for (;bufpos < bufsize; ++bufpos)
		{
			++itr;
		}
	}

	///\brief See template<class Iterator>Interface::asciichar(char*,unsigned int&,Iterator&)
	template <class Iterator>
	static char asciichar( char* buf, unsigned int& bufpos, Iterator& itr)
	{
		UChar ch = value( buf, bufpos, itr);
		return (ch > 127)?-1:(char)ch;
	}

	///\brief See template<class Iterator>Interface::value(char*,unsigned int&,Iterator&)
	template <class Iterator>
	static UChar value( char* buf, unsigned int& bufpos, Iterator& itr)
	{
		unsigned int bufsize = size( buf, bufpos, itr);
		UChar rt = (unsigned char)buf[ MSB];
		rt = (rt << 8) + (unsigned char)buf[ LSB];

		if (bufsize == 4)
		{
			// 2 teilig
			while (bufpos < bufsize)
			{
				buf[bufpos] = *itr;
				++itr;
				++bufpos;
			}
			rt -= 0xD800;
			rt *= 0x400;
			unsigned short lo = (unsigned char)buf[ 2+MSB];
			if ((lo - 0xD8) > 0x03) return 0xFFFF;
			lo = (lo << 8) + (unsigned char)buf[ 2+LSB];
			return rt + lo - 0xDC00 + 0x010000;
		}
		return rt;
	}

	///\brief See template<class Buffer>Interface::print(UChar,Buffer&)
	template <class Buffer_>
	static void print( UChar ch, Buffer_& buf)
	{
		if (ch <= 0xFFFF)
		{
			buf.push_back( (char)(unsigned char)((ch >> Print1shift) & 0xFF));
			buf.push_back( (char)(unsigned char)((ch >> Print2shift) & 0xFF));
		}
		else if (ch <= 0x10FFFF)
		{
			ch -= 0x10000;
			unsigned short hi = (ch / 0x400) + 0xD800;
			unsigned short lo = (ch % 0x400) + 0xDC00;
			buf.push_back( (char)(unsigned char)((hi >> Print1shift) & 0xFF));
			buf.push_back( (char)(unsigned char)((hi >> Print2shift) & 0xFF));
			buf.push_back( (char)(unsigned char)((lo >> Print1shift) & 0xFF));
			buf.push_back( (char)(unsigned char)((lo >> Print2shift) & 0xFF));
		}
		else
		{
			char tb[ 32];
			char* cc = tb;
			Encoder::encode( ch, tb, sizeof(tb));
			while (*cc)
			{
				buf.push_back( (char)(unsigned char)(((UChar)*cc >> Print1shift) & 0xFF));
				buf.push_back( (char)(unsigned char)(((UChar)*cc >> Print2shift) & 0xFF));
				++cc;
			}
		}
	}
};

///\class UTF16LE
///\brief UTF-16 little endian character set encoding
struct UTF16LE :public UTF16<ByteOrder::LE> {};
///\class UTF16BE
///\brief UTF-16 big endian character set encoding
struct UTF16BE :public UTF16<ByteOrder::BE> {};

}//namespace charset

///\enum ControlCharacter
///\brief Enumeration of control characters needed as events for XML scanner statemachine
enum ControlCharacter
{
	Undef=0,		///< not defined (beyond ascii)
	EndOfText,		///< end of data (EOF,EOD,.)
	EndOfLine,		///< end of line
	Cntrl,			///< control character
	Space,			///< space, tab, etc..
	Amp,			///< ampersant ('&')
	Lt,			///< lesser than '<'
	Equal,			///< equal '='
	Gt,			///< greater than '>'
	Slash,			///< slash '/'
	Exclam,			///< exclamation mark '!'
	Questm,			///< question mark '?'
	Sq,			///< single quote
	Dq,			///< double quote
	Osb,			///< open square bracket '['
	Csb,			///< close square bracket ']'
	Any,			///< any ascii character with meaning
	NofControlCharacter=17	///< total number of control characters
};

///\class ControlCharacterM
///\brief Map of the enumeration of control characters to their names for debug messages
struct ControlCharacterM
{
	///\brief Get the name of a control character as string
	///\param [in] c the control character to map
	static const char* name( ControlCharacter c)
	{
		static const char* name[ NofControlCharacter] = {"Undef", "EndOfText", "EndOfLine", "Cntrl", "Space", "Amp", "Lt", "Equal", "Gt", "Slash", "Exclam", "Questm", "Sq", "Dq", "Osb", "Csb", "Any"};
		return name[ (unsigned int)c];
	}
};

/*! @} */
///\defgroup Textscanner Text Scanner
///\brief Preliminary scanning of the input providing a unified view on the input character stream
/*! \addtogroup Textscanner
 *  @{
*/

///\class TextScanner
///\brief Reader for scanning the input character by character
///\tparam Iterator source iterator type (implements preincrement and '*' input byte access indirection)
///\tparam CharSet character set of the source stream
template <class Iterator, class CharSet>
class TextScanner
{
private:
	Iterator input;			///< source iterator
	char buf[8];			///< buffer for one character (the current character parsed)
	UChar val;			///< Unicode character representation of the current character parsed
	char cur;			///< ASCII character representation of the current character parsed
	unsigned int state;		///< current state of the text scanner

public:
	///\class ControlCharMap
	///\brief Map of ASCII characters to control character identifiers used in the XML scanner automaton
	struct ControlCharMap  :public CharMap<ControlCharacter,Undef>
	{
		ControlCharMap()
		{
			(*this)
			(0,EndOfText)
			(1,31,Cntrl)
			(5,Undef)
			(33,127,Any)
			(128,255,Undef)
			('\t',Space)
			('\r',Space)
			('\n',EndOfLine)
			(' ',Space)
			('&',Amp)
			('<',Lt)
			('=',Equal)
			('>',Gt)
			('/',Slash)
			('!',Exclam)
			('?',Questm)
			('\'',Sq)
			('\"',Dq)
			('[',Osb)
			(']',Csb);
		};
	};

	///\brief Constructor
	TextScanner()
		:val(0),cur(0),state(0)
	{
		for (unsigned int ii=0; ii<sizeof(buf); ii++) buf[ii] = 0;
	}

	TextScanner( const Iterator& p_iterator)
			:input(p_iterator),val(0),cur(0),state(0)
	{
		for (unsigned int ii=0; ii<sizeof(buf); ii++) buf[ii] = 0;
	}

	///\brief Copy constructor
	///\param [in] orig textscanner to copy
	TextScanner( const TextScanner& orig)
			:val(orig.val),cur(orig.cur),state(orig.state)
	{
		for (unsigned int ii=0; ii<sizeof(buf); ii++) buf[ii]=orig.buf[ii];
	}

	///\brief Initialize a new source iterator while keeping the state
	///\param [in] p_iterator source iterator
	void setSource( const Iterator& p_iterator)
	{
		input = p_iterator;
	}

	///\brief Get the unicode character of the current character
	///\return the unicode character
	UChar chr()
	{
		if (val == 0)
		{
			val = CharSet::value( buf, state, input);
		}
		return val;
	}

	///\brief Fill the internal buffer with as many current character bytes needed for reading the ASCII representation
	void getcur()
	{
		cur = CharSet::asciichar( buf, state, input);
	}

	///\brief Get the control character representation of the current character 
	///\return the control character
	ControlCharacter control()
	{
		static ControlCharMap controlCharMap;
		getcur();
		return controlCharMap[ (unsigned char)cur];
	}

	///\brief Get the ASCII character representation of the current character 
	///\return the ASCII character
	char ascii()
	{
		getcur();
		return cur>=0?cur:0;
	}

	///\brief Skip to the next character of the source
	///\return *this
	TextScanner& skip()
	{
		CharSet::skip( buf, state, input);
		state = 0;
		cur = 0;
		val = 0;
		return *this;
	}

	///\brief see TextScanner::chr()
	UChar operator*()
	{
		return chr();
	}

	///\brief Preincrement: Skip to the next character of the source
	///\return *this
	TextScanner& operator ++()	{return skip();}

	///\brief Postincrement: Skip to the next character of the source
	///\return *this
	TextScanner operator ++(int)	{TextScanner tmp(*this); skip(); return tmp;}
};

/*! @} */
///\defgroup XMLscanner XML Scanner
///\brief Structures for iterating on the XML elements
/*! \addtogroup XMLscanner
 *  @{
*/

///\class ScannerStatemachine
///\brief Class to build up the XML element scanner state machine in a descriptive way
class ScannerStatemachine :public throws_exception
{
public:
	enum
	{
		MaxNofStates=64			///< maximum number of states (fixed allocated array for state machine)
	};
	///\class Element
	///\brief One state in the state machine
	struct Element
	{
		int fallbackState;		///< state transition if the event does not match (it belongs to the next state = fallbackState)
		int missError;			///< error code in case of an event that does not match and there is no fallback

		///\class Action
		///\brief Definition of action fired by the state machine
		struct Action
		{
			int op;			///< action operand
			int arg;		///< action argument
		};
		Action action;			///< action executed after entering this state
		char nofnext;			///< number of follow states defined
		char next[ NofControlCharacter];///< follow state fired by an event (control character type parsed)

		///\brief Constructor
		Element() :fallbackState(-1),missError(-1),nofnext(0)
		{
			action.op = -1;
			action.arg = 0;
			for (unsigned int ii=0; ii<NofControlCharacter; ii++) next[ii] = -1;
		}
	};
	///\brief Get state addressed by its index
	///\param [in] stateIdx index of the state
	///\return state defintion reference
	Element* get( int stateIdx) throw(exception)
	{
		if ((unsigned int)stateIdx>size) throw exception(InvalidState);
		return tab + stateIdx;
	}

private:
	Element tab[ MaxNofStates];	///< states of the STM
	unsigned int size;		///< number of states defined in the STM

	///\brief Create a new state
	///\param [in] stateIdx index of the state (must be the size of the STM array, so that state identifiers can be named by enumeration constants for better readability)
	void newState( int stateIdx) throw(exception)
	{
		if (size != (unsigned int)stateIdx) throw exception( StateNumbersNotAscending);
		if (size >= MaxNofStates) throw exception( DimOutOfRange);
		size++;
	}

	///\brief Define a transition for all control character types not firing yet in the last state defined
	///\param [in] nextState the follow state index defined for these transitions
	void addOtherTransition( int nextState) throw(exception)
	{
		if (size == 0) throw exception( InvalidState);
		if (nextState < 0 || nextState > MaxNofStates) throw exception( InvalidParam);
		for (unsigned int inputchr=0; inputchr<NofControlCharacter; inputchr++)
		{
			if (tab[ size-1].next[ inputchr] == -1) tab[ size-1].next[ inputchr] = (unsigned char)nextState;
		}
		tab[ size-1].nofnext = NofControlCharacter;
	}

	///\brief Define a transition for inputchr in the last state defined
	///\param [in] inputchr the firing input control character type
	///\param [in] nextState the follow state index defined for this transition
	void addTransition( ControlCharacter inputchr, int nextState) throw(exception)
	{
		if (size == 0) throw exception( InvalidState);
		if ((unsigned int)inputchr >= (unsigned int)NofControlCharacter)  throw exception( InvalidParam);
		if (nextState < 0 || nextState > MaxNofStates)  throw exception( InvalidParam);
		if (tab[ size-1].next[ inputchr] != -1)  throw exception( InvalidParam);
		if (size == 0)  throw exception( InvalidState);
		tab[ size-1].next[ inputchr] = (unsigned char)nextState;
		tab[ size-1].nofnext += 1;
	}

	///\brief Define a self directing transition for inputchr in the last state defined (the state remains the same for this input)
	///\param [in] inputchr the firing input control character type
	void addTransition( ControlCharacter inputchr) throw(exception)
	{
		addTransition( inputchr, size-1);
	}

	///\brief Define an action in the last state defined (to be executed when entering the state)
	///\param [in] action_op action operand
	///\param [in] action_arg action argument
	void addAction( int action_op, int action_arg=0) throw(exception)
	{
		if (size == 0) throw exception( InvalidState);
		if (tab[ size-1].action.op != -1) throw exception( InvalidState);
		tab[ size-1].action.op = action_op;
		tab[ size-1].action.arg = action_arg;
	}

	///\brief Define an error in the last state defined to be reported when no fallback is defined and no firing input character parsed
	///\param [in] error code to be reported
	void addMiss( int error) throw(exception)
	{
		if (size == 0) throw exception( InvalidState);
		if (tab[ size-1].missError != -1) throw exception( InvalidState);
		tab[ size-1].missError = error;
	}

	///\brief Define in the last state defined a fallback state transition that is fired when no firing input character parsed
	///\param [in] stateIdx follow state index
	void addFallback( int stateIdx) throw(exception)
	{
		if (size == 0) throw exception( InvalidState);
		if (tab[ size-1].fallbackState != -1) throw exception( InvalidState);
		if (stateIdx < 0 || stateIdx > MaxNofStates) throw exception( InvalidParam);
		tab[ size-1].fallbackState = stateIdx;
	}
public:
	///\brief Constructor
	ScannerStatemachine() :size(0){}

	///\brief See ScannerStatemachine::newState(int)
	ScannerStatemachine& operator[]( int stateIdx)									{newState(stateIdx); return *this;}
	///\brief See ScannerStatemachine::addTransition(ControlCharacter,int)
	ScannerStatemachine& operator()( ControlCharacter inputchr, int ns)						{addTransition(inputchr,ns); return *this;}
	///\brief See ScannerStatemachine::addTransition(ControlCharacter,int)
	ScannerStatemachine& operator()( ControlCharacter i1, ControlCharacter i2, int ns)				{addTransition(i1,ns); addTransition(i2,ns); return *this;}
	///\brief See ScannerStatemachine::addTransition(ControlCharacter,int)
	ScannerStatemachine& operator()( ControlCharacter i1, ControlCharacter i2, ControlCharacter i3, int ns)		{addTransition(i1,ns); addTransition(i2,ns); addTransition(i3,ns); return *this;}
	///\brief See ScannerStatemachine::addTransition(ControlCharacter)
	ScannerStatemachine& operator()( ControlCharacter inputchr)							{addTransition(inputchr); return *this;}
	///\brief See ScannerStatemachine::addAction(int,int)
	ScannerStatemachine& action( int aa, int arg=0)									{addAction(aa,arg); return *this;}
	///\brief See ScannerStatemachine::addMiss(int)
	ScannerStatemachine& miss( int ee)										{addMiss(ee); return *this;}
	///\brief See ScannerStatemachine::addFallback(int)
	ScannerStatemachine& fallback( int stateIdx)									{addFallback(stateIdx); return *this;}
	///\brief See ScannerStatemachine::addOtherTransition(int)
	ScannerStatemachine& other( int stateIdx)									{addOtherTransition(stateIdx); return *this;}
};

///\class XMLScannerBase
///\brief XML scanner base class for things common for all XML scanners
class XMLScannerBase
{
public:
	///\enum ElementType
	///\brief Enumeration of XML element types returned by an XML scanner
	enum ElementType
	{
		None,					///< empty (NULL)
		ErrorOccurred,				///< XML scanning error error reported
		HeaderStart,				///< open XML header tag
		HeaderAttribName,			///< tag attribute name in the XML header
		HeaderAttribValue,			///< tag attribute value in the XML header
		HeaderEnd,				///< end of XML header event (after parsing '?&gt;')
		TagAttribName,				///< tag attribute name (e.g. "id" in &lt;person id='5'&gt;
		TagAttribValue,				///< tag attribute value (e.g. "5" in &lt;person id='5'&gt;
		OpenTag,				///< open tag (e.g. "bla" for "&lt;bla...")
		CloseTag,				///< close tag (e.g. "bla" for "&lt;/bla&gt;")
		CloseTagIm,				///< immediate close tag (e.g. "bla" for "&lt;bla /&gt;")
		Content,				///< content element string (separated by spaces or end of line)
		Exit					///< end of document
	};
	enum
	{
		NofElementTypes=Exit+1		///< number of XML element types defined
	};

	///\brief Get the XML element type as string
	///\param [in] ee XML element type
	///\return XML element type as string
	static const char* getElementTypeName( ElementType ee)
	{
		static const char* names[ NofElementTypes] = {0,"ErrorOccurred","HeaderStart","HeaderAttribName","HeaderAttribValue","HeaderEnd","TagAttribName","TagAttribValue","OpenTag","CloseTag","CloseTagIm","Content","Exit"};
		return names[ (unsigned int)ee];
	}

	///\enum Error
	///\brief Enumeration of XML scanner error codes
	enum Error
	{
		Ok,					///< no error, everything is OK
		ErrExpectedOpenTag,			///< expected an open tag in this state
		ErrExpectedXMLTag,			///< expected an <?xml tag in this state
		ErrUnexpectedEndOfText,			///< unexpected end of text in the middle of the XML definition
		ErrOutputBufferTooSmall,		///< scaned element in XML to big to fit in the buffer provided for it
		ErrSyntaxToken,				///< a specific string expected as token in XML but does not match
		ErrStringNotTerminated,			///< attribute string in XML not terminated on the same line
		ErrUndefinedCharacterEntity,		///< named entity is not defined in the entity map 
		ErrExpectedTagEnd,			///< expected end of tag
		ErrExpectedEqual,			///< expected equal in tag attribute definition
		ErrExpectedTagAttribute,		///< expected tag attribute
		ErrExpectedCDATATag,			///< expected CDATA tag definition
		ErrInternal,				///< internal error (textwolf implementation error)
		ErrUnexpectedEndOfInput,		///< unexpected end of input stream
		ErrExpectedEndOfLine			///< expected mandatory end of line (after XML header)
	};

	///\brief Get the error code as string
	///\param [in] ee error code
	///\return the error code as string
	static const char* getErrorString( Error ee)
	{
		enum {NofErrors=15};
		static const char* sError[NofErrors]
			= {0,"ExpectedOpenTag", "ExpectedXMLTag","UnexpectedEndOfText",
				"OutputBufferTooSmall","SyntaxToken","StringNotTerminated",
				"UndefinedCharacterEntity","ExpectedTagEnd",
				"ExpectedEqual", "ExpectedTagAttribute","ExpectedCDATATag","Internal",
				"UnexpectedEndOfInput", "ExpectedEndOfLine"
		};
		return sError[(unsigned int)ee];
	}

	///\enum STMState
	///\brief Enumeration of states of the XML scanner state machine
	enum STMState
	{
		START, STARTTAG, XTAG, PITAG, PITAGEND, XTAGEND, XTAGEOLN, XTAGDONE, XTAGAISK, XTAGANAM, XTAGAESK, XTAGAVSK, XTAGAVID, XTAGAVSQ, XTAGAVDQ, XTAGAVQE,
		CONTENT, TOKEN, XMLTAG, OPENTAG, CLOSETAG, TAGCLSK, TAGAISK, TAGANAM, TAGAESK, TAGAVSK, TAGAVID, TAGAVSQ, TAGAVDQ, TAGAVQE,
		TAGCLIM, ENTITYSL, ENTITY, ENTITYLC, CDATA, CDATA1, CDATA2, CDATA3, EXIT
	};

	///\brief Get the scanner state machine state as string
	///\param [in] s the state
	///\return the state as string
	static const char* getStateString( STMState s)
	{
		enum Constant {NofStates=39};
		static const char* sState[NofStates]
		= {
			"START", "STARTTAG", "XTAG", "PITAG", "PITAGEND",
			"XTAGEND", "XTAGEOLN", "XTAGDONE", "XTAGAISK", "XTAGANAM",
			"XTAGAESK", "XTAGAVSK", "XTAGAVID", "XTAGAVSQ", "XTAGAVDQ",
			"XTAGAVQE", "CONTENT", "TOKEN", "XMLTAG", "OPENTAG",
			"CLOSETAG", "TAGCLSK", "TAGAISK", "TAGANAM", "TAGAESK",
			"TAGAVSK", "TAGAVID", "TAGAVSQ", "TAGAVDQ", "TAGAVQE",
			"TAGCLIM", "ENTITYSL", "ENTITY", "ENTITYLC", "CDATA",
			"CDATA1", "CDATA2", "CDATA3", "EXIT"
		};
		return sState[(unsigned int)s];
	}

	///\enum STMAction
	///\brief Enumeration of actions in the XML scanner state machine
	enum STMAction
	{
		Return, ReturnWord, ReturnContent, ReturnIdentifier, ReturnSQString, ReturnDQString, ExpectIdentifierXML, ExpectIdentifierCDATA, ReturnEOF,
		NofSTMActions = 9
	};

	///\brief Get the scanner state machine action as string
	///\param [in] a the action
	///\return the action as string
	static const char* getActionString( STMAction a)
	{
		static const char* name[ NofSTMActions] = {"Return", "ReturnWord", "ReturnContent", "ReturnIdentifier", "ReturnSQString", "ReturnDQString", "ExpectIdentifierXML", "ExpectIdentifierCDATA", "ReturnEOF"};
		return name[ (unsigned int)a];
	};

	///\class Statemachine
	///\brief XML scanner state machine implementation
	struct Statemachine :public ScannerStatemachine
	{
		///\brief Constructor (defines the state machine completely)
		Statemachine( bool doTokenize)
		{
			(*this)
			[ START    ](EndOfText,EXIT)(EndOfLine)(Cntrl)(Space)(Lt,STARTTAG).miss(ErrExpectedOpenTag)
			[ STARTTAG ](EndOfLine)(Cntrl)(Space)(Questm,XTAG )(Exclam,ENTITYSL).fallback(OPENTAG)
			[ XTAG     ].action(ExpectIdentifierXML)(EndOfLine,Cntrl,Space,XTAGAISK)(Questm,XTAGEND).miss(ErrExpectedXMLTag)
			[ PITAG    ](Questm,PITAGEND).other(PITAG)
			[ PITAGEND ](Gt,CONTENT).miss(ErrExpectedTagEnd)
			[ XTAGEND  ](Gt,XTAGEOLN)(EndOfLine)(Cntrl)(Space).miss(ErrExpectedTagEnd)
			[ XTAGEOLN ](EndOfLine,XTAGDONE)(Cntrl)(Space).miss(ErrExpectedEndOfLine)
			[ XTAGDONE ].action(Return,HeaderEnd).fallback(CONTENT)
			[ XTAGAISK ](EndOfLine)(Cntrl)(Space)(Questm,XTAGEND).fallback(XTAGANAM)
			[ XTAGANAM ].action(ReturnIdentifier,HeaderAttribName)(EndOfLine,Cntrl,Space,XTAGAESK)(Equal,XTAGAVSK).miss(ErrExpectedEqual)
			[ XTAGAESK ](EndOfLine)(Cntrl)(Space)(Equal,XTAGAVSK).miss(ErrExpectedEqual)
			[ XTAGAVSK ](EndOfLine)(Cntrl)(Space)(Sq,XTAGAVSQ)(Dq,XTAGAVDQ).fallback(XTAGAVID)
			[ XTAGAVID ].action(ReturnIdentifier,HeaderAttribValue)(EndOfLine,Cntrl,Space,XTAGAISK)(Questm,XTAGEND).miss(ErrExpectedTagAttribute)
			[ XTAGAVSQ ].action(ReturnSQString,HeaderAttribValue)(Sq,XTAGAVQE).miss(ErrStringNotTerminated)
			[ XTAGAVDQ ].action(ReturnDQString,HeaderAttribValue)(Dq,XTAGAVQE).miss(ErrStringNotTerminated)
			[ XTAGAVQE ](EndOfLine,Cntrl,Space,XTAGAISK)(Questm,XTAGEND).miss(ErrExpectedTagAttribute);
			if (doTokenize)
			{
				(*this)
				[ CONTENT  ](EndOfText,EXIT)(EndOfLine)(Cntrl)(Space)(Lt,XMLTAG).fallback(TOKEN)
				[ TOKEN    ].action(ReturnWord,Content)(EndOfText,EXIT)(EndOfLine,Cntrl,Space,CONTENT)(Lt,XMLTAG).fallback(CONTENT);
			}
			else
			{
				(*this)
				[ CONTENT  ](EndOfText,EXIT)(Lt,XMLTAG).fallback(TOKEN)
				[ TOKEN    ].action(ReturnContent,Content)(EndOfText,EXIT)(EndOfLine,Cntrl,Space,CONTENT)(Lt,XMLTAG).fallback(CONTENT);
			}
			(*this)
			[ XMLTAG   ](EndOfLine)(Cntrl)(Space)(Questm,PITAG)(Slash,CLOSETAG).fallback(OPENTAG)
			[ OPENTAG  ].action(ReturnIdentifier,OpenTag)(EndOfLine,Cntrl,Space,TAGAISK)(Slash,TAGCLIM)(Gt,CONTENT).miss(ErrExpectedTagAttribute)
			[ CLOSETAG ].action(ReturnIdentifier,CloseTag)(EndOfLine,Cntrl,Space,TAGCLSK)(Gt,CONTENT).miss(ErrExpectedTagEnd)
			[ TAGCLSK  ](EndOfLine)(Cntrl)(Space)(Gt,CONTENT).miss(ErrExpectedTagEnd)
			[ TAGAISK  ](EndOfLine)(Cntrl)(Space)(Gt,CONTENT)(Slash,TAGCLIM).fallback(TAGANAM)
			[ TAGANAM  ].action(ReturnIdentifier,TagAttribName)(EndOfLine,Cntrl,Space,TAGAESK)(Equal,TAGAVSK).miss(ErrExpectedEqual)
			[ TAGAESK  ](EndOfLine)(Cntrl)(Space)(Equal,TAGAVSK).miss(ErrExpectedEqual)
			[ TAGAVSK  ](EndOfLine)(Cntrl)(Space)(Sq,TAGAVSQ)(Dq,TAGAVDQ).fallback(TAGAVID)
			[ TAGAVID  ].action(ReturnIdentifier,TagAttribValue)(EndOfLine,Cntrl,Space,TAGAISK)(Slash,TAGCLIM)(Gt,CONTENT).miss(ErrExpectedTagAttribute)
			[ TAGAVSQ  ].action(ReturnSQString,TagAttribValue)(Sq,TAGAVQE).miss(ErrStringNotTerminated)
			[ TAGAVDQ  ].action(ReturnDQString,TagAttribValue)(Dq,TAGAVQE).miss(ErrStringNotTerminated)
			[ TAGAVQE  ](EndOfLine,Cntrl,Space,TAGAISK)(Slash,TAGCLIM)(Gt,CONTENT).miss(ErrExpectedTagAttribute)
			[ TAGCLIM  ].action(Return,CloseTagIm)(EndOfLine)(Cntrl)(Space)(Gt,CONTENT).miss(ErrExpectedTagEnd)
			[ ENTITYSL ](Osb,CDATA).fallback(ENTITY)
			[ ENTITY   ](Gt,CONTENT)(Osb,ENTITYLC).other( ENTITY)
			[ ENTITYLC ](Csb,ENTITY).other( ENTITYLC)
			[ CDATA    ].action(ExpectIdentifierCDATA)(Osb,CDATA1).miss(ErrExpectedCDATATag)
			[ CDATA1   ](Csb,CDATA2).other(CDATA1)
			[ CDATA2   ](Csb,CDATA3).other(CDATA1)
			[ CDATA3   ](Gt,CONTENT).other(CDATA1)
			[ EXIT     ].action(Return,Exit);
		}
	};

	///\typedef IsTokenCharMap
	///\brief Forms a set of characters by assigning (true/false) to the whole domain
	typedef CharMap<bool,false,NofControlCharacter> IsTokenCharMap;

	///\class IsTagCharMap
	///\brief Defines the set of tag characters
	struct IsTagCharMap :public IsTokenCharMap
	{
		IsTagCharMap()
		{
			(*this)(Undef,true)(Any,true);
		}
	};

	///\class IsWordCharMap
	///\brief Defines the set of content word characters (tokenization switched on)
	struct IsWordCharMap :public IsTokenCharMap
	{
		IsWordCharMap()
		{
			(*this)(Undef,true)(Equal,true)(Gt,true)(Slash,true)(Exclam,true)(Questm,true)(Sq,true)(Dq,true)(Osb,true)(Csb,true)(Any,true);
		}
	};

	///\class IsContentCharMap
	///\brief Defines the set of content token characters
	struct IsContentCharMap :public IsTokenCharMap
	{
		IsContentCharMap()
		{
			(*this)(Cntrl,true)(Space,true)(EndOfLine,true)(Undef,true)(Equal,true)(Gt,true)(Slash,true)(Exclam,true)(Questm,true)(Sq,true)(Dq,true)(Osb,true)(Csb,true)(Any,true);
		}
	};

	///\class IsSQStringCharMap
	///\brief Defines the set characters belonging to a single quoted string
	struct IsSQStringCharMap :public IsContentCharMap
	{
		IsSQStringCharMap()
		{
			(*this)(Sq,false)(Space,true);
		}
	};

	///\class IsDQStringCharMap
	///\brief Defines the set characters belonging to a double quoted string
	struct IsDQStringCharMap :public IsContentCharMap
	{
		IsDQStringCharMap()
		{
			(*this)(Dq,false)(Space,true);
		}
	};
};


///\class XMLScanner
///\brief XML scanner template that adds the functionality to the statemachine base definition
///\tparam InputIterator input iterator with ++ and read only * returning 0 als last character of the input
///\tparam InputCharSet_ character set encoding of the input, read as stream of bytes
///\tparam OutputCharSet_ character set encoding of the output, printed as string of the item type of the character set,
///\tparam OutputBuffer_ buffer for output with STL back insertion sequence interface (e.g. std::string,std::vector<char>,textwolf::StaticBuffer)
///\tparam EntityMap_ STL like map from ASCII const char* to UChar
template
<
		class InputIterator,
		class InputCharSet_,
		class OutputCharSet_,
		class OutputBuffer_,
		class EntityMap_=std::map<const char*,UChar>
>
class XMLScanner :public XMLScannerBase
{
private:
	///\class TokState
	///\brief Token state variables
	struct TokState
	{
		///\enum Id
		///\brief Enumeration of token parser states.
		///\remark These states define where the scanner has to continue parsing when it was interrupted by an EoD exception and reentered again with more input to process.
		enum Id
		{
			Start,				///< start state (no parsing action performed at the moment)
			ParsingDone,			///< scanner war interrupted after parsing something when accessing the follow character
			ParsingKey,			///< scanner was interrupted when parsing a key
			ParsingEntity,			///< scanner was interrupted when parsing an XML character entity
			ParsingNumericEntity,		///< scanner was interrupted when parsing an XML numeric character entity
			ParsingNumericBaseEntity,	///< scanner was interrupted when parsing an XML basic character entity (apos,amp,etc..)
			ParsingNamedEntity,		///< scanner was interrupted when parsing an XML named character entity
			ParsingToken			///< scanner was interrupted when parsing a token (not in entity cotext)
		};
		Id id;					///< the scanner token parser state

		enum EolnState				///< end of line state to fulfill the W3C requirements for end of line mapping (see http://www.w3.org/TR/xml/: 2.11 End-of-Line Handling)
		{
			SRC,CR
		};
		EolnState eolnState;			///< the scanner end of line state

		unsigned int pos;			///< entity buffer position (buf)
		unsigned int base;			///< numeric entity base (10 for decimal/16 for hexadecimal)
		EChar value;				///< parsed entity value
		char buf[ 16];				///< parsed entity buffer
		UChar curchr_saved;			///< save current character parsed for the case we cannot print it (output buffer too small)

		///\brief Constructor
		TokState()				:id(Start),eolnState(SRC),pos(0),base(0),value(0),curchr_saved(0) {}

		///\brief Reset this state variables (after succesful exit with a new token parsed)
		///\param [in] id_ the new entity parse state
		///\param [in] eolnState_ the end of line mapping state
		void init(Id id_=Start, EolnState eolnState_=SRC)
		{
			id=id_;eolnState=eolnState_;pos=0;base=0;value=0;curchr_saved=0;
		}
	};
	TokState tokstate;								///< the entity parsing state of this XML scanner

public:
	typedef InputCharSet_ InputCharSet;
	typedef OutputCharSet_ OutputCharSet;
	class iterator;

public:
	typedef TextScanner<InputIterator,InputCharSet_> InputReader;
	typedef XMLScanner<InputIterator,InputCharSet_,OutputCharSet_,OutputBuffer_,EntityMap_> ThisXMLScanner;
	typedef EntityMap_ EntityMap;
	typedef typename EntityMap::const_iterator EntityMapIterator;
	typedef OutputBuffer_ OutputBuffer;

	///\brief Print a character to the output token buffer
	///\param [in] ch unicode character to print
	void push( UChar ch)
	{
		OutputCharSet::print( ch, *m_outputBuf);
	}

	///\brief Map a hexadecimal digit to its value
	///\param [in] ch hexadecimal digit to map to its decimal value
	static unsigned char HEX( unsigned char ch)
	{
		struct HexCharMap :public CharMap<unsigned char, 0xFF>
		{
			HexCharMap()
			{
				(*this)
					('0',0) ('1', 1)('2', 2)('3', 3)('4', 4)('5', 5)('6', 6)('7', 7)('8', 8)('9', 9)
					('A',10)('B',11)('C',12)('D',13)('E',14)('F',15)('a',10)('b',11)('c',12)('d',13)('e',14)('f',15);
			}
		};
		static HexCharMap hexCharMap;
		return hexCharMap[ch];
	}

	///\brief Parse a numeric entity value for a table definition (map it to the target character set)
	///\param [in] ir input reader
	///\return the value of the entity parsed
	static UChar parseStaticNumericEntityValue( InputReader& ir)
	{
		EChar value = 0;
		unsigned char ch = ir.ascii();
		unsigned int base;
		if (ch != '#') return 0;
		ir.skip();
		ch = ir.ascii();
		if (ch == 'x')
		{
			ir.skip();
			ch = ir.ascii();
			base = 16;
		}
		else
		{
			base = 10;
		}
		while (ch != ';')
		{
			unsigned char chval = HEX(ch);
			if (value >= base) return 0;
			value = value * base + chval;
			if (value >= 0xFFFFFFFF) return 0;
			ir.skip();
			ch = ir.ascii();
		}
		return (UChar)value;
	}

	///\brief Print the characters of a sequence that was thought to form an entity but did not
	///\return true on success
	void fallbackEntity()
	{
		switch (tokstate.id)
		{
			case TokState::Start:
			case TokState::ParsingDone:
			case TokState::ParsingKey:
			case TokState::ParsingToken:
				break;
			case TokState::ParsingEntity:
				push('&');
				break;
			case TokState::ParsingNumericEntity:
				push('&');
				push('#');
				break;
			case TokState::ParsingNumericBaseEntity:
				push('&');
				push('#');
				for (unsigned int ii=0; ii<tokstate.pos; ii++) push( tokstate.buf[ii]);
				break;
			case TokState::ParsingNamedEntity:
				push('&');
				for (unsigned int ii=0; ii<tokstate.pos; ii++) push( tokstate.buf[ii]);
				break;
		}
	}

	///\brief Try to parse an entity (we got '&')
	///\return true on success
	bool parseEntity()
	{
		unsigned char ch;
		tokstate.id = TokState::ParsingEntity;
		ch = m_src.ascii();
		if (ch == '#')
		{
			m_src.skip();
			return parseNumericEntity();
		}
		else
		{
			return parseNamedEntity();
		}
	}

	///\brief Try to parse a numeric entity (we got '&#')
	///\return true on success
	bool parseNumericEntity()
	{
		unsigned char ch;
		tokstate.id = TokState::ParsingNumericEntity;
		ch = m_src.ascii();
		if (ch == 'x')
		{
			tokstate.base = 16;
			m_src.skip();
			return parseNumericBaseEntity();
		}
		else
		{
			tokstate.base = 10;
			return parseNumericBaseEntity();
		}
	}

	///\brief Try to parse a numeric entity with known base (we got '&#' and we know the base 10/16 of it)
	///\return true on success
	bool parseNumericBaseEntity()
	{
		unsigned char ch;
		tokstate.id = TokState::ParsingNumericBaseEntity;

		while (tokstate.pos < sizeof(tokstate.buf))
		{
			tokstate.buf[tokstate.pos++] = ch = m_src.ascii();
			if (ch == ';')
			{
				if (tokstate.value > 0xFFFFFFFF)
				{
					fallbackEntity();
					return true;
				}
				push( (UChar)tokstate.value);
				tokstate.init( TokState::ParsingToken);
				m_src.skip();
				return true;
			}
			else
			{
				unsigned char chval = HEX(ch);
				if (tokstate.value >= tokstate.base)
				{
					fallbackEntity();
					return true;
				}
				tokstate.value = tokstate.value * tokstate.base + chval;
				m_src.skip();
			}
		}
		fallbackEntity();
		return true;
	}

	///\brief Try to parse a named entity
	///\return true on success
	bool parseNamedEntity()
	{
		unsigned char ch;
		tokstate.id = TokState::ParsingNamedEntity;
		ch = m_src.ascii();
		while (tokstate.pos < sizeof(tokstate.buf)-1 && ch != ';' && m_src.control() == Any)
		{
			tokstate.buf[ tokstate.pos] = ch;
			m_src.skip();
			tokstate.pos++;
			ch = m_src.ascii();
		}
		if (ch == ';')
		{
			tokstate.buf[ tokstate.pos] = '\0';
			if (!pushEntity( tokstate.buf)) return false;
			tokstate.init( TokState::ParsingToken);
			m_src.skip();
			return true;
		}
		else
		{
			fallbackEntity();
			return true;
		}
	}

	///\brief Try to recover from an interrupted token parsing state (end of input exception)
	///\return true on success
	bool parseTokenRecover()
	{
		bool rt = false;
		if (tokstate.curchr_saved)
		{
			push( tokstate.curchr_saved);
			tokstate.curchr_saved = 0;
		}
		switch (tokstate.id)
		{
			case TokState::Start:
			case TokState::ParsingDone:
			case TokState::ParsingKey:
			case TokState::ParsingToken:
				error = ErrInternal;
				return false;
			case TokState::ParsingEntity: rt = parseEntity(); break;
			case TokState::ParsingNumericEntity: rt = parseNumericEntity(); break;
			case TokState::ParsingNumericBaseEntity: rt = parseNumericBaseEntity(); break;
			case TokState::ParsingNamedEntity: rt = parseNamedEntity(); break;
		}
		tokstate.init( TokState::ParsingToken);
		return rt;
	}

	///\brief Parse a token defined by the set of valid token characters
	///\param [in] isTok set of valid token characters
	///\return true on success
	bool parseToken( const IsTokenCharMap& isTok)
	{
		if (tokstate.id == TokState::Start)
		{
			tokstate.id = TokState::ParsingToken;
			m_outputBuf->clear();
		}
		else if (tokstate.id != TokState::ParsingToken)
		{
			if (!parseTokenRecover())
			{
				tokstate.init();
				return false;
			}
		}
		for (;;)
		{
			ControlCharacter ch;
			while (isTok[ (unsigned char)(ch=m_src.control())])
			{
				UChar chr = m_src.chr();
				if (chr <= 0xD)
				{
					//handling W3C requirements for end of line translation in XML:
					char aa = m_src.ascii();
					if (aa == '\r')
					{
						push( (unsigned char)'\n');
						tokstate.eolnState = TokState::CR;
					}
					else if (aa == '\n')
					{
						if (tokstate.eolnState != TokState::CR)
						{
							push( (unsigned char)'\n');
						}
						tokstate.eolnState = TokState::SRC;
					}
					else
					{
						push( chr);
						tokstate.eolnState = TokState::SRC;
					}
				}
				else
				{
					push( chr);
					tokstate.eolnState = TokState::SRC;
				}
				m_src.skip();
			}
			if (ch == Amp)
			{
				m_src.skip();
				if (!parseEntity()) break;
				tokstate.init( TokState::ParsingToken);
				continue;
			}
			else
			{
				tokstate.init( TokState::ParsingDone);
				return true;
			}
		}
		tokstate.init();
		return false;
	}

	///\brief Static version of parse a token for parsing table definition elements
	///\tparam OutputBufferType type buffer for output
	///\param [in] isTok set of valid token characters
	///\param [in] ir input reader iterator
	///\param [out] buf buffer where to write the result to
	///\return true on success
	template <class OutputBufferType>
	static bool parseStaticToken( const IsTokenCharMap& isTok, InputReader ir, OutputBufferType& buf)
	{
		buf.clear();
		for (;;)
		{
			ControlCharacter ch;
			for (;;)
			{
				UChar pc;
				if (isTok[ (unsigned char)(ch=ir.control())])
				{
					pc = ir.chr();
				}
				else if (ch == Amp)
				{
					pc = parseStaticNumericEntityValue( ir);
				}
				else
				{
					return true;
				}
				OutputCharSet::print( pc, buf);
				ir.skip();
			}
		}
	}

	///\brief Skip a token defined by the set of valid token characters (same as parseToken but nothing written to the output buffer)
	///\param [in] isTok set of valid token characters
	///\return true on success
	bool skipToken( const IsTokenCharMap& isTok)
	{
		for (;;)
		{
			ControlCharacter ch;
			while (isTok[ (unsigned char)(ch=m_src.control())] || ch == Amp)
			{
				m_src.skip();
			}
			if (m_src.control() != Any) return true;
		}
	}

	///\brief Parse a token that must be the same as a given string
	///\param [in] str string expected
	///\return true on success
	bool expectStr( const char* str)
	{
		bool rt = true;
		tokstate.id = TokState::ParsingKey;
		for (; str[tokstate.pos] != '\0'; m_src.skip(),tokstate.pos++)
		{
			if (m_src.ascii() == str[ tokstate.pos]) continue;
			ControlCharacter ch = m_src.control();
			if (ch == EndOfText)
			{
				error = ErrUnexpectedEndOfText;
			}
			else
			{
				error = ErrSyntaxToken;
			}
			rt = false;
			break;
		}
		tokstate.init( TokState::ParsingDone);
		return rt;
	}

	///\brief Parse an entity defined by name (predefined)
	///\param [in] str pointer to the buffer with the entity name
	///\return true on success
	bool pushPredefinedEntity( const char* str)
	{
		switch (str[0])
		{
			case 'q':
				if (str[1] == 'u' && str[2] == 'o' && str[3] == 't' && str[4] == '\0')
				{
					push( '\"');
					return true;
				}
				break;

			case 'a':
				if (str[1] == 'm')
				{
					if (str[2] == 'p' && str[3] == '\0')
					{
						push( '&');
						return true;
					}
				}
				else if (str[1] == 'p')
				{
					if (str[2] == 'o' && str[3] == 's' && str[4] == '\0')
					{
						push( '\'');
						return true;
					}
				}
				break;

			case 'l':
				if (str[1] == 't' && str[2] == '\0')
				{
					push( '<');
					return true;
				}
				break;

			case 'g':
				if (str[1] == 't' && str[2] == '\0')
				{
					push( '>');
					return true;
				}
				break;

			case 'n':
				if (str[1] == 'b' && str[2] == 's' && str[3] == 'p' && str[4] == '\0')
				{
					push( ' ');
					return true;
				}
				break;
		}
		return false;
	}

	///\brief Parse an entity defined by name (predefined or in defined in entity table)
	///\param [in] str pointer to the buffer with the entity name
	///\return true on success
	bool pushEntity( const char* str)
	{
		if (pushPredefinedEntity( str))
		{
			return true;
		}
		else if (m_entityMap)
		{
			EntityMapIterator itr = m_entityMap->find( str);
			if (itr == m_entityMap->end())
			{
				error = ErrUndefinedCharacterEntity;
				return false;
			}
			else
			{
				UChar ch = itr->second;
				push( ch);
				return true;
			}
		}
		else
		{
			error = ErrUndefinedCharacterEntity;
			return false;
		}
	}

private:
	STMState state;			///< current state of the XML scanner
	bool m_doTokenize;		///< true, if we do tokenize the input, false if we get the content according the W3C default (see http://www.w3.org/TR/xml: 2.10 White Space Handling)
	Error error;			///< last error code
	InputReader m_src;		///< source input iterator
	const EntityMap* m_entityMap;	///< map with entities defined by the caller
	OutputBuffer* m_outputBuf;	///< buffer to use for output

public:
	///\brief Constructor
	///\param [in] p_src source iterator
	///\param [in] p_outputBuf buffer to use for output
	///\param [in] p_entityMap read only map of named entities defined by the user
	XMLScanner( InputIterator& p_src, OutputBuffer& p_outputBuf, const EntityMap& p_entityMap)
			:state(START),m_doTokenize(false),error(Ok),m_src(p_src),m_entityMap(&p_entityMap),m_outputBuf(&p_outputBuf)
	{}
	XMLScanner( InputIterator& p_src, OutputBuffer& p_outputBuf)
			:state(START),m_doTokenize(false),error(Ok),m_src(p_src),m_entityMap(0),m_outputBuf(&p_outputBuf)
	{}

	///\brief Copy constructor
	///\param [in] o scanner to copy
	XMLScanner( XMLScanner& o)
			:state(o.state),m_doTokenize(o.m_doTokenize),error(o.error),m_src(o.m_src),m_entityMap(o.m_entityMap),m_outputBuf(o.m_outputBuf)
	{}

	///\brief Redefine the buffer to use for output
	///\param [in] p_outputBuf buffer to use for output
	void setOutputBuffer( OutputBuffer& p_outputBuf)
	{
		m_outputBuf = &p_outputBuf;
	}

	///\brief Initialize a new source iterator while keeping the state
	///\param [in] itr source iterator
	void setSource( const InputIterator& itr)
	{
		m_src.setSource( itr);
	}

	///\brief Get the current parsed XML element string, if it was not masked out, see nextItem(unsigned short)
	///\return the item string
	const char* getItem() const {return m_outputBuf->size()?&m_outputBuf->at(0):"\0\0\0\0";}

	///\brief Get the size of the current parsed YML element string in bytes
	///\return the item string
	std::size_t getItemSize() const {return m_outputBuf->size();}

	///\brief Get the current XML scanner state machine state
	///\return pointer to the state variables
	ScannerStatemachine::Element* getState()
	{
		static Statemachine STMtok(true);
		static Statemachine STMW3C(false);
		static Statemachine* stm[2] = {&STMW3C,&STMtok};
		return stm[ m_doTokenize]->get( state);
	}

	///\brief Set the tokenization behaviour
	///\param [out] v the tokenization behaviour flag
	void doTokenize( bool v)
	{
		m_doTokenize = v;
	}

	///\brief Get the last error
	///\param [out] str the error as string
	///\return the error code
	Error getError( const char** str=0)
	{
		Error rt = error;
		error = Ok;
		if (str) *str=getErrorString(rt);
		return rt;
	}

	///\brief Scan the next XML element
	///\param [in] mask element types that should be printed to the output buffer (1 -> print, 0 -> mask out, just return the element as event)
	///\return the type of the XML element
	ElementType nextItem( unsigned short mask=0xFFFF)
	{
		static const IsWordCharMap wordC;
		static const IsContentCharMap contentC;
		static const IsTagCharMap tagC;
		static const IsSQStringCharMap sqC;
		static const IsDQStringCharMap dqC;
		static const IsTokenCharMap* tokenDefs[ NofSTMActions] = {0,&wordC,&contentC,&tagC,&sqC,&dqC,0,0,0};
		static const char* stringDefs[ NofSTMActions] = {0,0,0,0,0,0,"xml","CDATA",0};

		ElementType rt = None;
		ControlCharacter ch;
		do
		{
			ScannerStatemachine::Element* sd = getState();
			if (sd->action.op != -1)
			{
				if (tokenDefs[sd->action.op])
				{
					if (tokstate.id != TokState::ParsingDone)
					{
						if ((mask&(1<<sd->action.arg)) != 0)
						{
							if (!parseToken( *tokenDefs[ sd->action.op])) return ErrorOccurred;
						}
						else
						{
							if (!skipToken( *tokenDefs[ sd->action.op])) return ErrorOccurred;
						}
					}
					rt = (ElementType)sd->action.arg;
				}
				else if (stringDefs[sd->action.op])
				{
					if (tokstate.id != TokState::ParsingDone)
					{
						if (!expectStr( stringDefs[sd->action.op])) return ErrorOccurred;
						if (sd->action.op == ExpectIdentifierXML)
						{
							//... special treatement for xml header for not
							//    enforcing the model too much just for this case
							push( '?'); push( 'x'); push( 'm'); push( 'l');
							rt = HeaderStart;
						}
					}
					else if (sd->action.op == ExpectIdentifierXML)
					{
						//... special treatement for xml header for not  
						//    enforcing the model too much just for this case
						rt = HeaderStart;
					}
				}
				else
				{
					rt = (ElementType)sd->action.arg;
				}
				if (sd->nofnext == 0)
				{
					if (sd->fallbackState != -1)
					{
						state = (STMState)sd->fallbackState;
					}
					return rt;
				}
			}
			ch = m_src.control();
			tokstate.id = TokState::Start;

			if (sd->next[ ch] != -1)
			{
				state = (STMState)sd->next[ ch];
				m_src.skip();
			}
			else if (sd->fallbackState != -1)
			{
				state = (STMState)sd->fallbackState;
			}
			else if (sd->missError != -1)
			{
				error = (Error)sd->missError;
				return ErrorOccurred;
			}
			else if (ch == EndOfText)
			{
				error = ErrUnexpectedEndOfText;
				return ErrorOccurred;
			}
			else
			{
				error = ErrInternal;
				return ErrorOccurred;
			}
		}
		while (rt == None);
		return rt;
	}

	///\class End
	///\brief end of input tag
	struct End {};

	///\class iterator
	///\brief input iterator for iterating on the output of an XML scanner
	class iterator
	{
	public:
		///\class Element
		///\brief Iterator element visited
		class Element
		{
		private:
			friend class iterator;
			ElementType m_type;		///< type of the element
			const char* m_content;		///< value string of the element
			std::size_t m_size;		///< size of the value string in bytes
		public:
			///\brief Type of the current element as string
			const char* name() const	{return getElementTypeName( m_type);}
			///\brief Type of the current element
			ElementType type() const	{return m_type;}
			///\brief Value of the current element
			const char* content() const	{return m_content;}
			///\brief Size of the value of the current element in bytes
			std::size_t size() const	{return m_size;}
			///\brief Constructor
			Element()			:m_type(None),m_content(0),m_size(0) {}
			///\brief Constructor
			Element( const End&)		:m_type(Exit),m_content(0),m_size(0) {}
			///\brief Copy constructor
			///\param [in] orig element to copy
			Element( const Element& orig)	:m_type(orig.m_type),m_content(orig.m_content),m_size(orig.m_size) {}
		};
		// input iterator traits
		typedef Element value_type;
		typedef std::size_t difference_type;
		typedef std::size_t size_type;
		typedef Element* pointer;
		typedef Element& reference;
		typedef std::input_iterator_tag iterator_category;

	private:
		Element element;						///< currently visited element
		ThisXMLScanner* input;				///< XML scanner

		///\brief Skip to the next element
		///\param [in] mask element types that should be printed to the output buffer (1 -> print, 0 -> mask out, just return the element as event)
		///\return iterator pointing to the next element
		iterator& skip( unsigned short mask=0xFFFF)
		{
			if (input != 0)
			{
				element.m_type = input->nextItem(mask);
				element.m_content = input->getItem();
				element.m_size = input->getItemSize();
			}
			return *this;
		}

		///\brief Compare iterator with another
		///\param [in] iter iterator to compare with
		///\return true if they are equal
		bool compare( const iterator& iter) const
		{
			if (element.type() == iter.element.type())
			{
				if (element.type() == Exit || element.type() == None) return true;  //equal only at beginning and end
			}
			return false;
		}
	public:
		///\brief Assign an iterator to another
		///\param [in] orig iterator to copy
		void assign( const iterator& orig)
		{
			input = orig.input;
			element = orig.element;
		}
		///\brief Copy constructor
		///\param [in] orig iterator to copy
		iterator( const iterator& orig)
		{
			assign( orig);
		}
		///\brief Constructor
		///\param [in] p_input XML scanner to use for iteration
		///\param [in] doSkipToFirst true, if the iterator should skip to the first character of the input (default behaviour of STL conform iterators but maybe not exception save)
		iterator( ThisXMLScanner& p_input, bool doSkipToFirst=true)
				:input( &p_input)
		{
			if (doSkipToFirst)
			{
				element.m_type = input->nextItem();
				element.m_content = input->getItem();
				element.m_size = input->getItemSize();
			}
		}
		///\brief Constructor
		iterator( const End& et)  :element(et),input(0) {}
		///\brief Constructor
		iterator()  :input(0) {}
		///\brief Assignement operator
		///\param [in] orig iterator to assign to this
		iterator& operator = (const iterator& orig)
		{
			assign( orig);
			return *this;
		}
		///\brief Element dereference operator
		const Element& operator*()
		{
			return element;
		}
		///\brief Element dereference operator
		const Element* operator->()
		{
			return &element;
		}
		///\brief Preincrement
		///\return *this
		iterator& operator++()				{return skip();}
		///\brief Postincrement
		///\return *this
		iterator operator++(int)			{iterator tmp(*this); skip(); return tmp;}

		///\brief Compare to check for equality
		///\return true, if equal
		bool operator==( const iterator& iter) const	{return compare( iter);}
		///\brief Compare to check for unequality
		///\return true, if not equal
		bool operator!=( const iterator& iter) const	{return !compare( iter);}
	};

	///\brief Get begin iterator
	///\return iterator
	///\param [in] doSkipToFirst true, if the iterator should skip to the first character of the input (default behaviour of STL conform iterators but maybe not exception save)
	iterator begin( bool doSkipToFirst=true)
	{
		return iterator( *this, doSkipToFirst);
	}
	///\brief Get the pointer to the end of content
	///\return iterator
	iterator end()
	{
		return iterator( End());
	}
};

/*! @} */
///\defgroup XMLpathselect XML Path Select
///\brief Structures for iterating on the elements typed by XML path selections
/*! \addtogroup XMLpathselect
 *  @{
*/

///\class XMLPathSelectAutomaton
///\tparam CharSet_ character set of the token defintions of the automaton
///\brief Automaton to define XML path expressions and assign types (int values) to them
template <class CharSet_=charset::UTF8>
class XMLPathSelectAutomaton :public throws_exception
{
public:
	enum
	{
		defaultMemUsage=3*1024,		///< default memory usage of the XML path select process, if not specified else
		defaultMaxDepth=32		///< default max tag stack depth, if not specified else
	};
	std::size_t memUsage;			///< total memory usage
	unsigned int maxDepth;			///< max tag stack depth
	std::size_t maxScopeStackSize;		///< max scope stack depth
	unsigned int maxFollows;		///< maximum number of tokens searched in depth
	unsigned int maxTriggers;		///< maximum number of open triggers
	unsigned int maxTokens;			///< maximum number of open tokens

public:
	///\brief Constructor
	XMLPathSelectAutomaton()
			:memUsage(defaultMemUsage),maxDepth(defaultMaxDepth),maxScopeStackSize(0),maxFollows(0),maxTriggers(0),maxTokens(0)
	{
		if (!setMemUsage( memUsage, maxDepth)) throw exception( DimOutOfRange);
	}
	typedef CharSet_ CharSet;
	typedef int Hash;
	typedef XMLPathSelectAutomaton<CharSet> ThisXMLPathSelectAutomaton;

public:
	///\enum Operation
	///\brief Enumeration of operation types in the automaton definition
	enum Operation
	{
		Content,			///< searching content token
		Tag,				///< searching a tag
		Attribute,			///< searching an attribute
		ThisAttributeValue,		///< checking the value of the attribute just parsed (not an arbitrary but this one)
		AttributeValue,			///< searching a value of an attribute
		ContentStart			///< looking for the start of content (to signal the end of the XML header)
	};

	///\brief Get the name of the operation as string
	///\return the operation as string
	static const char* operationName( Operation op)
	{
		static const char* name[ 6] = {"Content", "Tag", "Attribute", "ThisAttributeValue", "AttributeValue", "ContentStart"};
		return name[ (unsigned int)op];
	}

	///\class Mask
	///\brief Mask to query for element types, if they match or not
	struct Mask
	{
		unsigned short pos;			///< positively selected elements bitmask
		unsigned short neg;			///< negatively selected elements bitmask that determines when a search pattern is given up copletely

		///\brief Tells if mask does not select anything anymore
		///\return true if it is not active anymore
		bool empty() const								{return (pos==0);}

		///\brief Constructor by values
		///\param [in] p_pos positively selected elements bitmask
		///\param [in] p_neg negatively selected elements bitmask that determines when a search pattern is given up copletely
		Mask( unsigned short p_pos=0, unsigned short p_neg=0):pos(p_pos),neg(p_neg) {}

		///\brief Copy constructor
		///\param[in] orig mask to copy
		Mask( const Mask& orig)								:pos(orig.pos),neg(orig.neg) {}

		///\brief Constructor by operation type
		Mask( Operation op)								:pos(0),neg(0) {this->match(op);}

		///\brief Reset operation (deactivate)
		void reset()									{pos=0; neg=0;}

		///\brief Deactivate operation for a certain element type
		void reject( XMLScannerBase::ElementType e)					{neg |= (1<<(unsigned short)e);}

		///\brief Declare an operation to match on an element type
		void match( XMLScannerBase::ElementType e)					{pos |= (1<<(unsigned short)e);}

		///\brief Declare an operation as seek operation
		void seekop( Operation op)
		{
			switch (op)
			{
				case Tag:
					this->match( XMLScannerBase::OpenTag);
					this->match( XMLScannerBase::HeaderStart);
					break;
				case Attribute:
					this->match( XMLScannerBase::TagAttribName);
					this->match( XMLScannerBase::HeaderAttribName);
					this->reject( XMLScannerBase::Content);
					break;
				case ThisAttributeValue:
					this->match( XMLScannerBase::TagAttribValue);
					this->match( XMLScannerBase::HeaderAttribValue);
					this->reject( XMLScannerBase::TagAttribName);
					this->reject( XMLScannerBase::HeaderAttribName);
					this->reject( XMLScannerBase::Content);
					this->reject( XMLScannerBase::OpenTag);
					break;
				case AttributeValue:
					this->match( XMLScannerBase::TagAttribValue);
					this->match( XMLScannerBase::HeaderAttribValue);
					this->reject( XMLScannerBase::Content);
					break;
				case Content:
					this->match( XMLScannerBase::Content);
					break;
				case ContentStart:
					this->match( XMLScannerBase::HeaderEnd);
					break;
			}
		}
		///\brief Join two mask definitions
		///\param[in] mask definition of mask to join this with
		void join( const Mask& mask)				{pos |= mask.pos; neg |= mask.neg;}

		///\brief Check if an element type matches the mask
		///\param[in] e element type to check
		bool matches( XMLScannerBase::ElementType e) const	{return (0 != (pos & (1<<(unsigned short)e)));}

		///\brief Check if an element type should reset a mask
		///\param[in] e element type to check
		bool rejects( XMLScannerBase::ElementType e) const	{return (0 != (neg & (1<<(unsigned short)e)));}
	};

	///\class Core
	///\brief Core of an automaton state definition that is used during XML processing
	struct Core
	{
		Mask mask;			///< mask definiting what tokens are matching this state
		bool follow;			///< true, if the state is seeking tokens in all follow scopes in the XML tree
		int typeidx;			///< type of the element emitted by this state on a match
		int cnt_start;			///< lower bound of the element index matching (for index ranges)
		int cnt_end;			///< upper bound of the element index matching (for index ranges)

		///\brief Constructor
		Core()			:follow(false),typeidx(0),cnt_start(0),cnt_end(-1) {}
		///\brief Copy constructor
		///\param [in] o element to copy
		Core( const Core& o)	:mask(o.mask),follow(o.follow),typeidx(o.typeidx),cnt_start(o.cnt_start),cnt_end(o.cnt_end) {}
	};

	///\class State
	///\brief State of an automaton in its definition
	struct State
	{
		Core core;			///< core of the state (the part used in processing)
		unsigned int keysize;		///< key size of the element
		char* key;			///< key of the element
		char* srckey;			///< key of the element as in source (for debugging or reporting, etc.)
		int next;			///< follow state
		int link;			///< alternative state to check

		///\brief Constructor
		State()
				:keysize(0),key(0),srckey(0),next(-1),link(-1) {}

		///\brief Copy constructor
		///\param [in] orig element to copy
		State( const State& orig)		:core(orig.core),keysize(orig.keysize),key(0),srckey(0),next(orig.next),link(orig.link)
		{
			defineKey( orig.keysize, orig.key, orig.srckey);
		}

		///\brief Destructor
		~State()
		{
			if (key) delete [] key;
			if (srckey) delete [] srckey;
		}

		///\brief Check it the state definition is empty
		///\return true for an empty state
		bool isempty()				{return key==0&&core.typeidx==0;}

		///\brief Define the matching key of this state
		///\param[in] p_keysize size of the key in bytes 
		///\param[in] p_key pointer to the key
		///\param[in] p_srckey the source form of the key (ASCII with encoded entities for everything else)
		void defineKey( unsigned int p_keysize, const char* p_key, const char* p_srckey)
		{
			unsigned int ii;
			if (key)
			{
				delete [] key;
				key = 0;
			}
			if (srckey)
			{
				delete [] srckey;
				srckey = 0;
			}
			if (p_key)
			{
				key = new char[ keysize=p_keysize];
				for (ii=0; ii<keysize; ii++) key[ii]=p_key[ii];
			}
			if (p_srckey)
			{
				for (ii=0; p_srckey[ii]!=0; ii++);
				srckey = new char[ ii+1];
				for (ii=0; p_srckey[ii]!=0; ii++) srckey[ii]=p_srckey[ii];
				srckey[ ii] = 0;
			}
		}

		///\brief Define a state transition by key and operation
		///\param[in] op operation type
		///\param[in] p_keysize size of the key in bytes 
		///\param[in] p_key pointer to the key
		///\param[in] p_srckey the source form of the key (ASCII with encoded entities for everything else)
		///\param[in] p_next follow state on a match
		///\param[in] p_follow true if the search reaches all included follow scopes of the definition scope
		void defineNext( Operation op, unsigned int p_keysize, const char* p_key, const char* p_srckey, int p_next, bool p_follow=false)
		{
			core.mask.seekop( op);
			defineKey( p_keysize, p_key, p_srckey);
			next = p_next;
			core.follow = p_follow;
		}

		///\brief Define an element output operation
		///\param[in] mask mask defining the element types to output
		///\param[in] p_typeidx the type of the element produced
		///\param[in] p_follow true if the output reaches all included follow scopes of the definition scope
		///\param[in] p_start start index of the element range produced
		///\param[in] p_end upper bound index of the element range produced
		void defineOutput( const Mask& mask, int p_typeidx, bool p_follow, int p_start, int p_end)
		{
			core.mask = mask;
			core.typeidx = p_typeidx;
			core.cnt_end = p_end;
			core.cnt_start = p_start;
			core.follow = p_follow;
		}

		///\brief Link another state to check to the current state
		///\param[in] p_link the index of the state to link
		void defLink( int p_link)
		{
			link = p_link;
		}
	};
	std::vector<State> states;							///< the states of the statemachine

	///\class Token
	///\brief Active or passive but still valid token of the XML processing (this is a trigger waiting to match)
	struct Token
	{
		Core core;											///< core of the state
		int stateidx;										///< index into the automaton, poiting to the state

		///\brief Constructor
		Token()						:stateidx(-1) {}
		///\brief Copy constructor
		Token( const Token& orig)			:core(orig.core),stateidx(orig.stateidx) {}
		///\brief Constructor by value
		///\param [in] state state that generated this token
		///\param [in] p_stateidx index of the state that generated this token
		Token( const State& state, int p_stateidx)	:core(state.core),stateidx(p_stateidx) {}
	};

	///\class Scope
	///\brief Tag scope definition
	struct Scope
	{
		Mask mask;					///< joined mask of all tokens active in this scope
		Mask followMask;				///< joined mask of all tokens active in this and all sub scopes of this scope

		///\class Range
		///\brief Range on the token stack with all tokens that belong to this scope
		struct Range
		{
			unsigned int tokenidx_from;		///< lower bound token index
			unsigned int tokenidx_to;		///< upper bound token index
			unsigned int followidx;			///< pointer to follow token stack with tokens active in this and all sub scopes of this scope

			///\brief Constructor
			Range()				:tokenidx_from(0),tokenidx_to(0),followidx(0) {}
			///\brief Copy constructor
			///\param[in] orig scope to copy
			Range( const Scope& orig)	:tokenidx_from(orig.tokenidx_from),tokenidx_to(orig.tokenidx_to),followidx(orig.followidx) {}
		};
		Range range;							///< valid (active) token range of this scope (on the token stacks)

		///\brief Copy constructor
		///\param[in] orig scope to copy
		Scope( const Scope& orig)		:mask(orig.mask),followMask(orig.followMask),range(orig.range) {}
		///\brief Assignement operator
		///\param[in] orig scope to copy
		Scope& operator =( const Scope& orig)	{mask=orig.mask; followMask=orig.followMask; range=orig.range; return *this;}
		///\brief Constructor
		Scope()					{}
	};

	///\brief Defines the usage of memory
	///\param [in] p_memUsage size of the memory block in bytes 
	///\param [in] p_maxDepth maximum depht of the scope stack
	///\return true, if everything is OK
	bool setMemUsage( std::size_t p_memUsage, unsigned int p_maxDepth)
	{
		memUsage = p_memUsage;
		maxDepth = p_maxDepth;
		maxScopeStackSize = maxDepth;
		if (p_memUsage < maxScopeStackSize * sizeof(Scope))
		{
			maxScopeStackSize = 0;
		}
		else
		{
			p_memUsage -= maxScopeStackSize * sizeof(Scope);
		}
		maxFollows = (p_memUsage / sizeof(std::size_t)) / 32 + 2;
		maxTriggers = (p_memUsage / sizeof(std::size_t)) / 32 + 3;
		p_memUsage -= sizeof(std::size_t) * maxFollows + sizeof(std::size_t) * maxTriggers;
		maxTokens = p_memUsage / sizeof(Token);
		return (maxScopeStackSize != 0 && maxTokens != 0 && maxFollows != 0 && maxTriggers != 0);
	}

private:
	///\brief Defines a state transition
	///\param [in] stateidx from what source state
	///\param [in] op operation firing the state transition
	///\param [in] keysize length of the key firing the state transition in bytes
	///\param [in] key the key string firing the state transition in bytes	
	///\param [in] srckey the ASCII encoded representation in the source
	///\param [in] follow true, uf the state transition is active for all sub scopes of the activation state
	///\return the target state of the transition defined
	int defineNext( int stateidx, Operation op, unsigned int keysize, const char* key, const char* srckey, bool follow=false) throw(exception)
	{
		try
		{
			State state;
			if (states.size() == 0)
			{
				stateidx = states.size();
				states.push_back( state);
			}
			for (int ee=stateidx; ee != -1; stateidx=ee,ee=states[ee].link)
			{
				if (states[ee].key != 0 && keysize == states[ee].keysize && states[ee].core.follow == follow)
				{
					unsigned int ii;
					for (ii=0; ii<keysize && states[ee].key[ii]==key[ii]; ii++);
					if (ii == keysize) return states[ee].next;
				}
			}
			if (!states[stateidx].isempty())
			{
				stateidx = states[stateidx].link = states.size();
				states.push_back( state);
			}
			states.push_back( state);
			unsigned int lastidx = states.size()-1;
			states[ stateidx].defineNext( op, keysize, key, srckey, lastidx, follow);
			return stateidx=lastidx;
		}
		catch (std::bad_alloc)
		{
			throw exception( OutOfMem);
		}
		catch (...)
		{
			throw exception( Unknown);
		}
	}

	///\brief Defines an output print action and output type for a state
	///\param [in] stateidx from what source state
	///\param [in] printOpMask mask for elements printed
	///\param [in] typeidx type identifier
	///\param [in] follow true, uf the state transition is active for all sub scopes of the activation state
	///\param [in] start start of index range where this state transition fires
	///\param [in] end end of index range where this state transition fires
	///\return index of the state where this output action was defined
	int defineOutput( int stateidx, const Mask& printOpMask, int typeidx, bool follow, int start, int end) throw(exception)
	{
		try
		{
			State state;
			if (states.size() == 0)
			{
				stateidx = states.size();
				states.push_back( state);
			}
			if ((unsigned int)stateidx >= states.size()) throw exception( IllegalParam);

			if (!states[stateidx].isempty())
			{
				stateidx = states[stateidx].link = states.size();
				states.push_back( state);
			}
			states[ stateidx].defineOutput( printOpMask, typeidx, follow, start, end);
			return stateidx;
		}
		catch (std::bad_alloc)
		{
			throw exception( OutOfMem);
		}
		catch (...)
		{
			throw exception( Unknown);
		}
	}

public:
	///\class PathElement
	///\brief Defines one node in the XML Path element tree in the construction phase. 
	///\remark This is just a construct for building the tree with cascading operators forming a path representation
	struct PathElement :throws_exception
	{
	private:
		XMLPathSelectAutomaton* xs;		///< XML Path select automaton where this node is an element of
		int stateidx;							///< state of this element in the automaton
		
		///\class Range
		///\brief Element counting range defining what are indices of valid elements
		struct Range
		{
			int start;							///< index of starting element starting with 0
			int end;								///< index of upper boundary element (not belonging to range anymore). -1 if undefined (unlimited)

			///\brief Copy constructor
			///\param [in] o range element to copy
			Range( const Range& o)		:start(o.start),end(o.end){}
			///\brief Constructor by value
			///\param [in] p_start index of starting element
			///\param [in] p_end index of upper boundary element (not belonging to range anymore). -1 if undefined (unlimited)
			Range( int p_start, int p_end)	:start(p_start),end(p_end){}
			///\brief Constructor by value
			///\param [in] count number of elements starting with the first one (with index 0)
			Range( int count)		:start(0),end(count){}
			///\brief Constructor
			Range()				:start(0),end(-1){}
		};
		Range range;			///< Index range of this XML path element
		bool follow;			///< true, if this element is active (firing) for all sub scopes of the activation scope
		Mask pushOpMask;		///< mask for firing element actions
		Mask printOpMask;		///< mask for printing element actions

	private:
		///\brief Define an output operation for a certain element type in this state
		///\param [in] op XML operation type of this output
		///\return *this
		PathElement& defineOutput( Operation op)
		{
			printOpMask.reset();
			printOpMask.seekop( op);
			return *this;
		}

		///\brief Define a state transition operation for a token of a certain element type in this state
		///\param [in] op XML operation type of this state transition
		///\param [in] value key value as ASCII with encoded entities for higher unicode characters of this state transition
		///\return *this
		PathElement& doSelect( Operation op, const char* value) throw(exception)
		{
			static XMLScannerBase::IsTagCharMap isTagCharMap;
			if (xs != 0)
			{
				if (value)
				{
					char buf[ 1024];
					StaticBuffer pb( buf, sizeof(buf));
					char* itr = const_cast<char*>(value);
					typedef XMLScanner<char*,CharSet,CharSet,StaticBuffer> StaticXMLScanner;
					if (!StaticXMLScanner::parseStaticToken( isTagCharMap, itr, pb))
					{
						throw exception( IllegalAttributeName);
					}
					stateidx = xs->defineNext( stateidx, op, pb.size(), pb.ptr(), value, follow);
				}
				else
				{
					stateidx = xs->defineNext( stateidx, op, 0, 0, 0, follow);
				}
			}
			return *this;
		}

		///\brief Define this element as active (firing,printing) for all sub scopes of the activation scope
		///\return *this
		PathElement& doFollow()
		{
			follow = true;
			return *this;
		}

		///\brief Define a valid range of token count for this element to be active
		///\param [in] p_start index of starting element starting with 0
		///\param [in] p_end index of upper boundary element (not belonging to range anymore). -1 if undefined (unlimited)
		///\return *this
		PathElement& doRange( int p_start, int p_end)
		{
			if (range.end == -1)
			{
				range = Range( p_start, p_end);
			}
			else if (p_end < range.end)
			{
				range.end = p_end;
			}
			else if (p_start > range.start)
			{
				range.start = p_start;
			}
			return *this;
		}

		///\brief Define a valid range of token count for this element to be active by the number of elements
		///\param [in] p_count number of elements starting with 0
		///\return *this
		PathElement& doCount( int p_count)
		{
			return doRange( 0, p_count);
		}

		///\brief Define the start of the range of token count for this element to be active
		///\param [in] p_start index of starting element starting with 0
		///\return *this
		PathElement& doStart( int p_start)
		{
			return doRange( p_start, std::numeric_limits<int>::max());
		}

		///\brief Define the output of the current element
		///\param [in] typeidx type of the element produced
		///\return *this
		PathElement& push( int typeidx) throw(exception)
		{
			if (xs != 0) stateidx = xs->defineOutput( stateidx, printOpMask, typeidx, follow, range.start, range.end);
			return *this;
		}

	public:
		///\brief Constructor
		PathElement()							:xs(0),stateidx(0),follow(false),pushOpMask(0),printOpMask(0){}
		///\brief Constructor by values
		///\param [in] p_xs automaton of this element
		///\param [in] p_si state index of this element in the automaton definition
		PathElement( XMLPathSelectAutomaton* p_xs, int p_si=0)		:xs(p_xs),stateidx(p_si),follow(false),pushOpMask(0),printOpMask(0){}
		///\brief Copy constructor
		///\param [in] orig element to copy
		PathElement( const PathElement& orig)				:xs(orig.xs),stateidx(orig.stateidx),range(orig.range),follow(orig.follow),pushOpMask(orig.pushOpMask),printOpMask(orig.printOpMask) {}

		///\brief Corresponds to "//" in abbreviated syntax of XPath
		///\return *this
		PathElement& operator --(int)							{return doFollow();}
		///\brief Find tag by name
		///\param [in] name name of the tag
		///\return *this		
		PathElement& operator []( const char* name) throw(exception)			{return doSelect( Tag, name);}
		///\brief Find tag by name
		///\remark same as selectTag(const char*)
		///\param [in] name name of the tag
		///\return *this		
		PathElement& selectTag( const char* name) throw(exception)			{return doSelect( Tag, name);}
		///\brief Find tag with one attribute
		///\param [in] name name of the attribute
		///\return *this		
		PathElement& operator ()( const char* name) throw(exception)			{return doSelect( Attribute, name).defineOutput( ThisAttributeValue);}
		///\brief Find tag with one attribute
		///\remark same as selectAttribute(const char*)
		///\param [in] name name of the attribute
		///\return *this
		PathElement& selectAttribute( const char* name) throw(exception)		{return doSelect( Attribute, name).defineOutput( ThisAttributeValue);}

		//\brief Find tag with one attribute,value condition
		///\remark same as ifAttribute(const char*,const char*)
		///\param [in] name name of the attribute
		///\param [in] value value of the attribute
		///\return *this
		PathElement& operator ()( const char* name, const char* value) throw(exception)	{return doSelect( Attribute, name).doSelect( ThisAttributeValue, value);}
		//\brief Find tag with one attribute,value condition
		///\param [in] name name of the attribute
		///\param [in] value value of the attribute
		///\return *this
		PathElement& ifAttribute( const char* name, const char* value) throw(exception)	{return doSelect( Attribute, name).doSelect( ThisAttributeValue, value);}

		///\brief Define maximum element index to push
		///\param [in] idx maximum element index
		///\return *this
		PathElement& TO(int idx) throw(exception)					{return doCount((idx>=0)?(idx+1):-1);}
		///\brief Define minimum element index to push
		///\param [in] idx minimum element index
		///\return *this
		PathElement& FROM(int idx) throw(exception)					{return doStart(idx); return *this;}
		///\brief Define minimum and maximum element index to push
		///\param [in] idx1 minimum element index
		///\param [in] idx2 maximum element index
		///\return *this
		PathElement& RANGE(int idx1, int idx2) throw(exception)		{return doRange(idx1,(idx2>=0)?(idx2+1):-1); return *this;}
		///\brief Define element type to push
		///\remark same as assignType(int)
		///\param [in] type element type
		///\return *this
		PathElement& operator =(int type) throw(exception)				{return push( type);}
		///\brief Define element type to push
		///\param [in] type element type
		///\return *this
		PathElement& assignType(int type) throw(exception)				{return push( type);}
		///\brief Define grab content
		///\remark same as selectContent()
		///\return *this
		PathElement& operator ()()  throw(exception)					{return defineOutput(Content);}
		///\brief Define grab content
		///\return *this
		PathElement& selectContent()  throw(exception)					{return defineOutput(Content);}
	};

	///\brief Get automaton root element to start an XML path definition
	///\return the automaton root element
	PathElement operator*()
	{
		return PathElement( this);
	}
};

///\brief XML path select template
///\tparam InputIterator input iterator with ++ and read only * returning 0 als last character of the input
///\tparam InputCharSet_ character set encoding of the input, read as stream of bytes
///\tparam OutputCharSet_ character set encoding of the output, printed as string of the item type of the character set,
///\tparam OutputBuffer_ buffer for output with STL back insertion sequence interface (e.g. std::string,std::vector<char>,textwolf::StaticBuffer)
///\tparam EntityMap_ STL like map from ASCII const char* to UChar
template <
		class InputIterator,
		class InputCharSet_,
		class OutputCharSet_,
		class OutputBuffer_,
		class EntityMap_=std::map<const char*,UChar>
>
class XMLPathSelect :public throws_exception
{
public:
	typedef OutputBuffer_ OutputBuffer;
	typedef XMLPathSelectAutomaton<OutputCharSet_> ThisXMLPathSelectAutomaton;
	typedef XMLScanner<InputIterator,InputCharSet_,OutputCharSet_,OutputBuffer,EntityMap_> ThisXMLScanner;
	typedef XMLPathSelect<InputIterator,InputCharSet_,OutputCharSet_,OutputBuffer,EntityMap_> ThisXMLPathSelect;
	typedef EntityMap_ EntityMap;

private:
	ThisXMLScanner scan;				///< XML Scanner for fetching elements for the automaton input
	const ThisXMLPathSelectAutomaton* atm;		///< XML select automaton used
	typedef typename ThisXMLPathSelectAutomaton::Mask Mask;
	typedef typename ThisXMLPathSelectAutomaton::Token Token;
	typedef typename ThisXMLPathSelectAutomaton::Hash Hash;
	typedef typename ThisXMLPathSelectAutomaton::State State;
	typedef typename ThisXMLPathSelectAutomaton::Scope Scope;

	///\class Array
	///\brief static array of POD types. I decided to implement it on my own though using boost::array would maybe be better.
	///\tparam Element element type of the array
	template <typename Element>
	class Array :public throws_exception
	{
		Element* m_ar;				///< pointer to elements
		std::size_t m_size;			///< fill size (number of elements inserted)
		std::size_t m_maxSize;			///< allocation size (space reserved for this number of elements)
	public:
		///\brief Constructor
		///\param [in] p_maxSize allocation size (number of elements) to reserve
		Array( std::size_t p_maxSize) :m_size(0),m_maxSize(p_maxSize)
		{
			m_ar = new (std::nothrow) Element[ m_maxSize];
			if (m_ar == 0) throw exception( OutOfMem);
		}

		///\brief Destructor
		~Array()
		{
			if (m_ar) delete [] m_ar;
		}

		///\brief Append one element
		///\param [in] elem element to append
		void push_back( const Element& elem)
		{
			if (m_size == m_maxSize) throw exception( OutOfMem);
			m_ar[ m_size++] = elem;
		}

		///\brief Remove one element from the end
		void pop_back()
		{
			if (m_size == 0) throw exception( NotAllowedOperation);
			m_size--;
		}

		///\brief Access element by index
		///\param [in] idx index of the element starting with 0
		///\return element reference
		Element& operator[]( std::size_t idx)
		{
			if (idx >= m_size) throw exception( ArrayBoundsReadWrite);
			return m_ar[ idx];
		}

		///\brief Get a reference of the element at the end of the array
		///\return element reference
		Element& back()
		{
			if (m_size == 0) throw exception( ArrayBoundsReadWrite);
			return m_ar[ m_size-1];
		}

		///\brief Resize of the array
		///\param [in] p_size new array size
		void resize( std::size_t p_size)
		{
			if (p_size > m_size) throw exception( ArrayBoundsReadWrite);
			m_size = p_size;
		}
		std::size_t size() const  {return m_size;}
		bool empty() const			{return m_size==0;}
	};

	Array<Scope> scopestk;		///< stack of scopes opened
	Array<unsigned int> follows;	///< indices of tokens active in all descendant scopes
	Array<int> triggers;		///< triggered elements
	Array<Token> tokens;		///< list of waiting tokens

	///\class Context
	///\brief State variables without stacks of the automaton
	struct Context
	{
		XMLScannerBase::ElementType type;	///< element type processed
		const char* key;			///< string value of element processed
		unsigned int keysize;			///< size of string value in bytes of element processed
		Scope scope;				///< active scope
		unsigned int scope_iter;		///< position of currently visited token in the active scope

		///\brief Constructor
		Context()				:type(XMLScannerBase::Content),key(0),keysize(0) {}

		///\brief Initialization
		///\param [in] p_type type of the current element processed
		///\param [in] p_key current element processed
		///\param [in] p_keysize size of the key in bytes
		void init( XMLScannerBase::ElementType p_type, const char* p_key, int p_keysize)
		{
			type = p_type;
			key = p_key;
			keysize = p_keysize;
			scope_iter = scope.range.tokenidx_from;
		}
	};
	Context context;		///< state variables without stacks of the automaton

	///\brief Activate a state by index
	///\param stateidx index of the state to activate
	void expand( int stateidx)
	{
		while (stateidx!=-1)
		{
			const State& st = atm->states[ stateidx];
			context.scope.mask.join( st.core.mask);
			if (st.core.mask.empty() && st.core.typeidx != 0)
			{
				triggers.push_back( st.core.typeidx);
			}
			else
			{
				if (st.core.follow)
				{
					context.scope.followMask.join( st.core.mask);
					follows.push_back( tokens.size());
				}
				tokens.push_back( Token( st, stateidx));
			}
			stateidx = st.link;
		}
	}

	///\brief Declares the currently processed element of the XMLScanner input. By calling fetch we get the output elements from it
	///\param [in] type type of the current element processed
	///\param [in] key current element processed
	///\param [in] keysize size of the key in bytes
	void initProcessElement( XMLScannerBase::ElementType type, const char* key, int keysize)
	{
		if (context.type == XMLScannerBase::OpenTag)
		{
			//last step of open scope has to be done after all tokens were visited,
			//e.g. with the next element initialization
			context.scope.range.tokenidx_from = context.scope.range.tokenidx_to;
		}
		context.scope.range.tokenidx_to = tokens.size();
		context.scope.range.followidx = follows.size();
		context.init( type, key, keysize);

		if (type == XMLScannerBase::OpenTag)
		{
			//first step of open scope saves the context context on stack
			scopestk.push_back( context.scope);
			context.scope.mask = context.scope.followMask;
			context.scope.mask.match( XMLScannerBase::OpenTag);
			//... we reset the mask but ensure that this 'OpenTag' is processed for sure
		}
		else if (type == XMLScannerBase::CloseTag || type == XMLScannerBase::CloseTagIm)
		{
			if (!scopestk.empty())
			{
				context.scope = scopestk.back();
				scopestk.pop_back();
				follows.resize( context.scope.range.followidx);
				tokens.resize( context.scope.range.tokenidx_to);
			}
		}
	}

	///\brief produce an element adressed by token index
	///\param [in] tokenidx index of the token in the list of active tokens
	///\param [in] st state from which the expand was triggered
	void produce( unsigned int tokenidx, const State& st)
	{
		const Token& tk = tokens[ tokenidx];
		if (tk.core.cnt_end == -1)
		{
			expand( st.next);
		}
		else
		{
			if (tk.core.cnt_end > 0)
			{
				if (--tokens[ tokenidx].core.cnt_end == 0)
				{
					tokens[ tokenidx].core.mask.reset();
				}
				if (tk.core.cnt_start <= 0)
				{
					expand( st.next);
				}
				else
				{
					--tokens[ tokenidx].core.cnt_start;
				}
			}
		}
	}

	///\brief check if an active token addressed by index matches to the currently processed element
	///\param [in] tokenidx index of the token in the list of active tokens
	///\return matching token type
	int match( unsigned int tokenidx)
	{
		int rt = 0;
		if (context.key != 0)
		{
			if (tokenidx >= context.scope.range.tokenidx_to) return 0;

			const Token& tk = tokens[ tokenidx];
			if (tk.core.mask.matches( context.type))
			{
				const State& st = atm->states[ tk.stateidx];
				if (st.key)
				{
					if (st.keysize == context.keysize)
					{
						unsigned int ii;
						for (ii=0; ii<context.keysize && st.key[ii] == context.key[ii]; ii++);
						if (ii==context.keysize)
						{
							produce( tokenidx, st);
						}
					}
				}
				else
				{
					produce( tokenidx, st);
				}
				if (tk.core.typeidx != 0)
				{
					if (tk.core.cnt_end == -1)
					{
						rt = tk.core.typeidx;
					}
					else if (tk.core.cnt_end > 0)
					{
						if (--tokens[ tokenidx].core.cnt_end == 0)
						{
							tokens[ tokenidx].core.mask.reset();
						}
						if (tk.core.cnt_start <= 0)
						{
							rt = tk.core.typeidx;
						}
						else
						{
							--tokens[ tokenidx].core.cnt_start;
						}
					}
				}
			}
			if (tk.core.mask.rejects( context.type))
			{
				//The token must not match anymore after encountering a reject item
				tokens[ tokenidx].core.mask.reset();
			}
		}
		return rt;
	}

	///\brief fetch the next matching element
	///\return type of the matching element
	int fetch()
	{
		int type = 0;

		if (context.scope.mask.matches( context.type))
		{
			while (!type)
			{
				if (context.scope_iter < context.scope.range.tokenidx_to)
				{
					type = match( context.scope_iter);
					++context.scope_iter;
				}
				else
				{
					unsigned int ii = context.scope_iter - context.scope.range.tokenidx_to;
					//we match all follows that are not yet been checked in the current scope
					if (ii < context.scope.range.followidx && context.scope.range.tokenidx_from > follows[ ii])
					{
						type = match( follows[ ii]);
						++context.scope_iter;
					}
					else if (!triggers.empty())
					{
						type = triggers.back();
						triggers.pop_back();
					}
					else
					{
						context.key = 0;
						context.keysize = 0;
						return 0; //end of all candidates
					}
				}
			}
		}
		else
		{
			context.key = 0;
			context.keysize = 0;
		}
		return type;
	}

public:
	///\brief Constructor
	///\param[in] p_atm read only ML path select automaton reference
	///\param[in] p_src source input iterator to process
	///\param[in] obuf reference to buffer to use for the output elements (STL back insertion sequence interface)
	///\param[in] entityMap read only map of named entities to expand
	XMLPathSelect( const ThisXMLPathSelectAutomaton* p_atm, InputIterator& p_src, OutputBuffer& obuf, const EntityMap& entityMap)
		:scan(p_src,obuf,entityMap),atm(p_atm),scopestk(p_atm->maxScopeStackSize),follows(p_atm->maxFollows),triggers(p_atm->maxTriggers),tokens(p_atm->maxTokens)
	{
		if (atm->states.size() > 0) expand(0);
	}
	///\brief Constructor
	///\param[in] p_atm read only ML path select automaton reference
	///\param[in] p_src source input iterator to process
	///\param[in] obuf reference to buffer to use for the output elements (STL back insertion sequence interface)
	XMLPathSelect( const ThisXMLPathSelectAutomaton* p_atm, InputIterator& p_src, OutputBuffer& obuf)
		:scan(p_src,obuf),atm(p_atm),scopestk(p_atm->maxScopeStackSize),follows(p_atm->maxFollows),triggers(p_atm->maxTriggers),tokens(p_atm->maxTokens)
	{
		if (atm->states.size() > 0) expand(0);
	}
	///\brief Copy constructor
	///\param [in] o element to copy
	XMLPathSelect( const XMLPathSelect& o)
		:scan(o.scan),atm(o.atm),scopestk(o.maxScopeStackSize),follows(o.maxFollows),follows(o.maxTriggers),tokens(o.maxTokens){}

	///\brief Redefine the buffer to use for output
	///\param [in] p_outputBuf buffer to use for output
	void setOutputBuffer( OutputBuffer& p_outputBuf)
	{
		scan.setOutputBuffer( p_outputBuf);
	}

	///\brief Set the tokenization behaviour
	///\param [out] v the tokenization behaviour flag
	void doTokenize( bool v)
	{
		scan.doTokenize(v);
	}

	///\class End
	///\brief end of input iterator for the output of this XMLScanner
	struct End {};

	///\class iterator
	///\brief input iterator for the output of this XMLScanner
	class iterator
	{
	public:
		///\class Element
		///\brief visited current element data of the iterator
		class Element
		{
		public:
			///\class State
			///\brief state of the iterator
			enum State
			{
				Ok,				///< normal
				EndOfOutput,	///< end of output triggered
				EndOfInput,		///< end of input triggered
				ErrorState		///< error occurred (identifier as string is in the output token buffer)
			};

			///\brief Constructor
			Element()				:m_state(Ok),m_type(0),m_content(0),m_size(0) {}
			///\brief Constructor for content end iterator
			Element( const End&)			:m_state(EndOfInput),m_type(0),m_content(0),m_size(0) {}
			///\brief Copy constructor
			///\param [in] orig element to copy
			Element( const Element& orig)		:m_state(orig.m_state),m_type(orig.m_type),m_content(orig.m_content),m_size(orig.m_size) {}
			///\brief Get the iterator state
			State state() const			{return m_state;}
			///\brief Get the currently visited element type
			int type() const			{return m_type;}
			///\brief Get the currently visited element content
			const char* content() const		{return m_content;}
			///\brief Get the size of the content of the currently visited element in bytes
			unsigned int size() const		{return m_size;}
		private:
			friend class iterator;		///< friend to intialize the elements
			State m_state;			///< current state
			int m_type;			///< currently visited element type
			const char* m_content;		///< currently visited element content
			unsigned int m_size;		///< size of the content of the currently visited element in bytes
		};
		typedef Element value_type;
		typedef std::size_t difference_type;
		typedef Element* pointer;
		typedef Element& reference;
		typedef std::input_iterator_tag iterator_category;

	private:
		Element element;		///< currently visited element
		ThisXMLPathSelect* input;		///< producing XML path selection stream

		///\brief Skip to next element
		///\return *this
		iterator& skip() throw(exception)
		{
			if (input != 0)
			{
				do
				{
					if (!input->context.key)
					{
						XMLScannerBase::ElementType et = input->scan.nextItem( input->context.scope.mask.pos);
						if (et == XMLScannerBase::Exit)
						{
							if (input->scopestk.size() == 0)
							{
								element.m_state = Element::EndOfInput;
							}
							else
							{
								element.m_state = Element::ErrorState;
								element.m_content = XMLScannerBase::getErrorString( XMLScannerBase::ErrUnexpectedEndOfInput);
							}
							return *this;
						}
						if (et == XMLScannerBase::ErrorOccurred)
						{
							XMLScannerBase::Error err = input->scan.getError( &element.m_content);
							if (err == XMLScannerBase::ErrOutputBufferTooSmall)
							{
								element.m_state = Element::EndOfOutput;
							}
							else
							{
								element.m_state = Element::ErrorState;
							}
							return *this;
						}
						input->initProcessElement( et, input->scan.getItem(), input->scan.getItemSize());
					}
					element.m_type = input->fetch();

				} while (element.m_type == 0);

				element.m_content = input->context.key;
				element.m_size = input->context.keysize;
			}
			return *this;
		}
		///\brief Iterator compare
		///\param [in] iter iterator to compare with
		///\return true, if the elements are equal
		bool compare( const iterator& iter) const
		{
			return (element.state() != Element::Ok && iter.element.state() != Element::Ok);
		}
	public:
		///\brief Assign iterator
		///\param [in] orig iterator to copy
		void assign( const iterator& orig)
		{
			input = orig.input;
			element = orig.element;
		}

		///\brief Copy constructor
		///\param [in] orig iterator to copy
		iterator( const iterator& orig)
		{
			assign( orig);
		}

		///\brief Constructor by values
		///\param [in] p_input XML path selection stream to iterate through
		///\param [in] skipToFirst true, if the iterator should skip to the first character of the input (default behaviour of STL conform iterators but maybe not exception save)
		iterator( ThisXMLPathSelect& p_input, bool skipToFirst=true)
				:input( &p_input)
		{
			if (skipToFirst) skip();
		}

		///\brief Constructor
		///\param [in] et end of input tag
		iterator( const End& et)	:element(et),input(0) {}

		///\brief Constructor
		iterator()			:input(0) {}

		///\brief Assignement
		///\param [in] orig iterator to copy
		///\return *this
		iterator& operator = (const iterator& orig)
		{
			assign( orig);
			return *this;
		}

		///\brief Element acceess
		///\return read only element reference
		const Element& operator*()
		{
			return element;
		}

		///\brief Element acceess
		///\return read only element reference
		const Element* operator->()
		{
			return &element;
		}

		///\brief Preincrement
		///\return *this
		iterator& operator++()		{return skip();}

		///\brief Postincrement
		///\return *this
		iterator operator++(int)	{iterator tmp(*this); skip(); return tmp;}

		///\brief Compare elements for equality
		///\return true, if they are equal
		bool operator==( const iterator& iter) const	{return compare( iter);}

		///\brief Compare elements for inequality
		///\return true, if they are not equal
		bool operator!=( const iterator& iter) const	{return !compare( iter);}
	};

	///\brief Get the start iterator
	///\return iterator pointing to the first of the selected XML path elements
	iterator begin( bool skipToFirst=true)
	{
		return iterator( *this, skipToFirst);
	}

	///\brief Get the end of content marker
	///\return iterator as end of content marker
	iterator end()
	{
		return iterator( End());
	}
};

/*! @} */

} //namespace textwolf
#endif

