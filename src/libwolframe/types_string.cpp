/************************************************************************
Copyright (C) 2011 - 2013 Project Wolframe.
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
///\file types_string.cpp
///\brief Implementation of strings in various encodings
#include "types/string.hpp"
#include <cstdlib>
#include <cstring>
#if WITH_TEXTWOLF
#include "textwolf/charset.hpp"
#include "textwolf/cstringiterator.hpp"
#include "textwolf/textscanner.hpp"
#include "textwolf/staticbuffer.hpp"
#endif
#include <boost/shared_ptr.hpp>

using namespace _Wolframe;
using namespace _Wolframe::types;

namespace {
struct StringReader
{
	virtual ~StringReader(){}
	virtual unsigned int read()=0;
};
typedef boost::shared_ptr<StringReader> StringReaderR;

struct StringWriter
{
	virtual ~StringWriter(){}
	virtual void write( unsigned int ch)=0;
};
typedef boost::shared_ptr<StringWriter> StringWriterR;
}//anonymous namespace


#if WITH_TEXTWOLF
namespace {
template <class ENC>
class StringReaderImpl
	:public StringReader
{
public:
	StringReaderImpl( const char* content, std::size_t contentsize, const ENC& enc=ENC())
		:m_reader( enc, textwolf::CStringIterator( content, contentsize))
	{}
	virtual ~StringReaderImpl(){}

	virtual unsigned int read()
	{
		textwolf::UChar rt;
		rt = m_reader.chr();
		++m_reader;
		return rt;
	}
private:
	typename textwolf::TextScanner<textwolf::CStringIterator,ENC> m_reader;
};

template <class ENCODING>
class StringWriterImpl
	:public StringWriter
{
public:
	StringWriterImpl( std::string& buf_, const ENCODING& encoding_=ENCODING())
		:m_encoding(encoding_),m_buf(buf_)
	{}
	virtual ~StringWriterImpl(){}

	virtual void write( unsigned int ch)
	{
		m_encoding.print( ch, m_buf);
	}

private:
	ENCODING m_encoding;
	std::string& m_buf;
};

}//anonymous namespace

static StringReaderR createStringReader( String::Encoding encoding, unsigned char codepage, const char* content, std::size_t contentsize)
{
	StringReaderR rt;
	switch (encoding)
	{
		case String::ISO8859:
		{
			typedef textwolf::charset::IsoLatin EC;
			return StringReaderR( new StringReaderImpl<EC>( content, contentsize, EC( codepage)));
		}
		case String::UTF8:
		{
			typedef textwolf::charset::UTF8 EC;
			return StringReaderR( new StringReaderImpl<EC>( content, contentsize));
		}
		case String::UTF16BE:
		{
			typedef textwolf::charset::UTF16BE EC;
			return StringReaderR( new StringReaderImpl<EC>( content, contentsize));
		}
		case String::UTF16LE:
		{
			typedef textwolf::charset::UTF16LE EC;
			return StringReaderR( new StringReaderImpl<EC>( content, contentsize));
		}
		case String::UCS4BE:
		{
			typedef textwolf::charset::UCS4BE EC;
			return StringReaderR( new StringReaderImpl<EC>( content, contentsize));
		}
		case String::UCS2LE:
		{
			typedef textwolf::charset::UCS2LE EC;
			return StringReaderR( new StringReaderImpl<EC>( content, contentsize));
		}
		case String::UCS2BE:
		{
			typedef textwolf::charset::UCS2BE EC;
			return StringReaderR( new StringReaderImpl<EC>( content, contentsize));
		}
		case String::UCS4LE:
		{
			typedef textwolf::charset::UCS4LE EC;
			return StringReaderR( new StringReaderImpl<EC>( content, contentsize));
		}
	}
	return rt;
}

static StringWriterR createStringWriter( std::string& buf, String::Encoding encoding, unsigned char codepage)
{
	switch (encoding)
	{
		case String::ISO8859:
		{
			typedef textwolf::charset::IsoLatin EC;
			return StringWriterR( new StringWriterImpl<EC>( buf, EC( codepage)));
		}
		case String::UTF8:
		{
			typedef textwolf::charset::UTF8 EC;
			return StringWriterR( new StringWriterImpl<EC>( buf));
		}
		case String::UTF16BE:
		{
			typedef textwolf::charset::UTF16BE EC;
			return StringWriterR( new StringWriterImpl<EC>( buf));
		}
		case String::UTF16LE:
		{
			typedef textwolf::charset::UTF16LE EC;
			return StringWriterR( new StringWriterImpl<EC>( buf));
		}
		case String::UCS2BE:
		{
			typedef textwolf::charset::UCS2BE EC;
			return StringWriterR( new StringWriterImpl<EC>( buf));
		}
		case String::UCS2LE:
		{
			typedef textwolf::charset::UCS2LE EC;
			return StringWriterR( new StringWriterImpl<EC>( buf));
		}
		case String::UCS4BE:
		{
			typedef textwolf::charset::UCS4BE EC;
			return StringWriterR( new StringWriterImpl<EC>( buf));
		}
		case String::UCS4LE:
		{
			typedef textwolf::charset::UCS4LE EC;
			return StringWriterR( new StringWriterImpl<EC>( buf));
		}
	}
	return StringWriterR();
}

#else
namespace {
class StringReaderUCS1
	:public StringReader
{
public:
	StringReaderUCS1( const char* content_, std::size_t contentsize_, unsigned char maxchar_)
		:m_content(content_),m_contentsize(contentsize_),m_pos(0),m_maxchar(maxchar_){}
	virtual ~StringReaderUCS1(){}

	virtual unsigned int read()
	{
		if (m_pos == m_contentsize) return 0;
		unsigned char ch = (unsigned char)m_content[ m_pos++];
		if (ch > m_maxchar)
		{
			if (m_maxchar == 127)
			{
				throw std::runtime_error( "cannot read string containing non ascii characters in this encoding (build with WITH_TEXTWOLF=1 needed)");
			}
			else
			{
				throw std::runtime_error( "cannot read string containing unicode characters beyond codepage 2 in this encoding (build with WITH_TEXTWOLF=1 needed)");
			}
		}
		return ch;
	}
private:
	const char* m_content;
	std::size_t m_contentsize;
	std::size_t m_pos;
	unsigned char m_maxchar;
};

class StringWriterUCS1
	:public StringWriter
{
public:
	StringWriterUCS1( std::string& buf_, unsigned char maxchar_)
		:m_buf(buf_),m_maxchar(maxchar_){}
	virtual ~StringWriterUCS1(){}

	virtual void write( unsigned int ch)
	{
		if (ch > m_maxchar)
		{
			if (m_maxchar == 127)
			{
				throw std::runtime_error( "cannot write string containing non ascii characters in this encoding (build with WITH_TEXTWOLF=1 needed)");
			}
			else
			{
				throw std::runtime_error( "cannot write string containing unicode characters beyond codepage 2 in this encoding (build with WITH_TEXTWOLF=1 needed)");
			}
		}
		m_buf.push_back( (char)(unsigned char)ch);
	}
private:
	std::string& m_buf;
	unsigned int m_maxchar;
};

class StringReaderUCS2BE_CP2
	:public StringReader
{
public:
	StringReaderUCS2BE_CP2( const char* content_, std::size_t contentsize_)
		:m_content(content_),m_contentsize(contentsize_),m_pos(0){}
	virtual ~StringReaderUCS2BE_CP2(){}

	virtual unsigned int read()
	{
		if (m_pos+2 > m_contentsize) return 0;
		unsigned char ch1 = (unsigned char)m_content[ m_pos++];
		unsigned char ch2 = (unsigned char)m_content[ m_pos++];
		if (ch1) throw std::runtime_error( "cannot read string with non ascii characters in this encoding (build flag WITH_TEXTWOLF=1 enables encoding in the core)");
		return ch2;
	}
private:
	const char* m_content;
	std::size_t m_contentsize;
	std::size_t m_pos;
};

class StringWriterUCS2BE_CP2
	:public StringWriter
{
public:
	StringWriterUCS2BE_CP2( std::string& buf_)
		:m_buf(buf_){}
	virtual ~StringWriterUCS2BE_CP2(){}

	virtual void write( unsigned int ch)
	{
		if (ch > 0xFF) throw std::runtime_error( "cannot write string with non ascii characters in this encoding (build flag WITH_TEXTWOLF=1 enables encoding in the core)");
		m_buf.push_back( 0);
		m_buf.push_back( (char)(unsigned char)ch);
	}
	virtual const void* content() const
	{
		return m_buf.c_str();
	}
	virtual std::size_t contentsize() const
	{
		return m_buf.size();
	}
private:
	std::string& m_buf;
};
}

static StringReaderR createStringReader( String::Encoding encoding, unsigned char codepage, const char* content, std::size_t contentsize)
{
	switch (encoding)
	{
		case String::ISO8859:
		{
			unsigned char maxchar = (codepage == 1)?256:127;
			return StringReaderR( new StringReaderUCS1( content, contentsize, maxchar));
		}
		case String::UTF8:
		{
			return StringReaderR( new StringReaderUCS1( content, contentsize, 127));
		}
		case String::UCS2BE:
		case String::UTF16BE:
		{
			return StringReaderR( new StringReaderUCS2BE_CP2( content, contentsize));
		}
		case String::UTF16LE:
		case String::UCS2LE:
		case String::UCS4BE:
		case String::UCS4LE:
			throw std::runtime_error( "no character transformations defined for this enoding (build with WITH_TEXTWOLF=1 needed)");
	}
	return StringReaderR();
}

static StringWriterR createStringWriter( std::string& buf, String::Encoding encoding, unsigned char codepage)
{
	switch (encoding)
	{
		case String::ISO8859:
		{
			unsigned char maxchar = (codepage == 1)?256:127;
			return StringWriterR( new StringWriterUCS1( buf, maxchar));
		}
		case String::UTF8:
		{
			return StringWriterR( new StringWriterUCS1( buf, 127));
		}
		case String::UCS2BE:
		case String::UTF16BE:
		{
			return StringWriterR( new StringWriterUCS2BE_CP2( buf));
		}
		case String::UTF16LE:
		case String::UCS2LE:
		case String::UCS4BE:
		case String::UCS4LE:
			throw std::runtime_error( "no character transformations defined for this enoding (build with WITH_TEXTWOLF=1 needed)");
	}
	return StringWriterR();
}
#endif




String::String( const std::string& val)
	:m_encoding(UTF8),m_codepage(0),m_isconst(false),m_size(val.size())
{
	m_ar = (unsigned char*)std::calloc( m_size+1, 1);
	if (!m_ar) throw std::bad_alloc();
	std::memcpy( m_ar, val.c_str(), m_size);
}

String::String( const std::wstring& val)
	:m_encoding(UTF16BE),m_codepage(0),m_isconst(false),m_size(val.size())
{
	m_ar = (unsigned char*)std::calloc( val.size()+1, 2);
	if (!m_ar) throw std::bad_alloc();
	std::memcpy( m_ar, val.c_str(), m_size * 2);
}

String::String( const void* content, std::size_t contentsize, Encoding encoding_, unsigned char codepage_)
	:m_encoding(encoding_),m_codepage(codepage_),m_isconst(false),m_size(contentsize/elementSize( encoding_))
{
	m_ar = (unsigned char*)std::calloc( m_size+1, elementSize( encoding_));
	if (!m_ar) throw std::bad_alloc();
	std::memcpy( m_ar, content, contentsize);
}

String::String()
	:m_encoding(UTF8),m_codepage(0),m_isconst(false),m_size(0),m_ar(0)
{}

String::String( const String& o)
	:m_encoding(o.m_encoding),m_codepage(o.m_codepage),m_isconst(o.m_isconst),m_size(o.m_size),m_ar(o.m_ar)
{
	if (!m_isconst)
	{
		m_ar = (unsigned char*)std::calloc( m_size+1, elementSize( encoding()));
		if (!m_ar) throw std::bad_alloc();
		std::memcpy( m_ar, o.m_ar, o.m_size * elementSize( encoding()));
	}
}

String String::translateEncoding( Encoding encoding_, unsigned char codepage_) const
{
	String rt;
	rt.m_encoding = encoding_;
	rt.m_codepage = codepage_;

	if (encoding_ == m_encoding)
	{
		rt.m_ar = (unsigned char*)std::calloc( m_size+1, elementSize( encoding_));
		rt.m_size = m_size;
		if (!rt.m_ar) throw std::bad_alloc();
		std::memcpy( rt.m_ar, m_ar, m_size * elementSize( encoding_));
	}
	else
	{
		std::string buf;
		StringReaderR src = createStringReader( encoding(), codepage(), (const char*)m_ar, m_size * elementSize());
		StringWriterR dst = createStringWriter( buf, encoding_, codepage_);
	
		unsigned int ch;
		while (0!=(ch = src->read()))
		{
			dst->write( ch);
		}
		std::size_t elemsize = elementSize( encoding_);
		rt.m_size = buf.size()/elemsize;

		rt.m_ar = (unsigned char*)std::calloc( rt.m_size+1, elemsize);
		if (!rt.m_ar) throw std::bad_alloc();
		std::memcpy( rt.m_ar, buf.c_str(), buf.size());
	}
	return rt;
}

String::~String()
{
	if (m_ar && !m_isconst) std::free( m_ar);
}


std::string String::tostring() const
{
	std::string rt;
	StringReaderR src = createStringReader( encoding(), codepage(), (const char*)m_ar, m_size * elementSize());
	StringWriterR dst = createStringWriter( rt, String::UTF8, 0);

	unsigned int ch;
	while (0!=(ch = src->read()))
	{
		dst->write( ch);
	}
	return rt;
}

std::wstring String::towstring() const
{
	std::string buf;
	StringReaderR src = createStringReader( encoding(), codepage(), (const char*)m_ar, m_size * elementSize());
	StringWriterR dst = createStringWriter( buf, String::UTF16BE, 0);

	unsigned int ch;
	while (0!=(ch = src->read()))
	{
		dst->write( ch);
	}
	return std::wstring( (const wchar_t*)(const void*)buf.c_str(), buf.size()/2);
}

static void parseEncodingName( std::string& dest, const std::string& src)
{
	dest.clear();
	std::string::const_iterator cc=src.begin();
	for (; cc != src.end(); ++cc)
	{
		if (*cc <= ' ') continue;
		if (*cc == '-') continue;
		if (*cc == ' ') continue;
		dest.push_back( ::tolower( *cc));
	}
}

String::EncodingAttrib String::getEncodingFromName( const std::string& name)
{
	std::string enc;
	parseEncodingName( enc, name);

	if ((enc.size() >= 8 && std::memcmp( enc.c_str(), "isolatin", enc.size())== 0)
	||  (enc.size() >= 7 && std::memcmp( enc.c_str(), "iso8859", enc.size()) == 0))
	{
		const char* codepage = enc.c_str() + ((enc.c_str()[4] == 'l')?8:7);
		if (std::strlen( codepage) > 1 || codepage[0] < '0' || codepage[0] > '9')
		{
			throw std::runtime_error( "unknown iso-latin code page index");
		}
		return EncodingAttrib( ISO8859, codepage[0] - '0');
	}
	else if (enc.size() == 0 || enc == "utf8")
	{
		return EncodingAttrib( UTF8);
	}
	else if (enc == "utf16" || enc == "utf16be")
	{
		return EncodingAttrib( UTF16BE);
	}
	else if (enc == "utf16le")
	{
		return EncodingAttrib( UTF16LE);
	}
	else if (enc == "ucs2" || enc == "ucs2be")
	{
		return EncodingAttrib( UCS2BE);
	}
	else if (enc == "ucs2le")
	{
		return EncodingAttrib( UCS2LE);
	}
	else if (enc == "utf32" || enc == "ucs4" || enc == "utf32be" || enc == "ucs4be")
	{
		return EncodingAttrib( UCS4BE);
	}
	else if (enc == "utf32le" || enc == "ucs4le")
	{
		return EncodingAttrib( UCS4LE);
	}
	else
	{
		throw std::runtime_error( "unknown character set encoding for converter");
	}
}

String::EncodingClass::Id String::guessEncoding( const char* content, std::size_t size)
{
	int maxll = 0;		//< maximum number of zeroes in a row
	int ll = 0;		//< number of zeroes in a row
	std::size_t idx = 0;	//< source index

	if (size == 0) return EncodingClass::NONE;

	for (; idx < size; ++idx)
	{
		if (content[idx] != '\0')
		{
			ll = 0;
		}
		else
		{
			ll++;
			if (ll > maxll)
			{
				maxll = ll;
				if (ll >= 3) break;
			}
		}
	}
	if (maxll == 2)
	{
		return EncodingClass::FAIL;
	}
	else if (maxll == 0)
	{
		return EncodingClass::UCS1;
	}
	if (size >= 2 && maxll == 1)
	{
		// ... UTF16 check BOM
		if ((unsigned char)content[0] == 0xFE && (unsigned char)content[1] == 0xFF) return EncodingClass::UCS2BE;
		if ((unsigned char)content[0] == 0xFF && (unsigned char)content[1] == 0xFE) return EncodingClass::UCS2LE;
	}
	// ... indians not set
	if (maxll == 3)
	{
		for (idx=0; idx < size; idx+=4)
		{
			if (content[idx] == '\0')
			{
				return EncodingClass::UCS4BE;
			}
			if (idx > 0 && content[idx-1] == '\0')
			{
				return EncodingClass::UCS4LE;
			}
		}
	}
	else if (maxll == 1)
	{
		for (idx=0; idx < size; idx+=2)
		{
			if (content[idx] == '\0')
			{
				return EncodingClass::UCS2BE;
			}
			if (idx > 0 && content[idx-1] == '\0')
			{
				return EncodingClass::UCS2LE;
			}
		}
	}
	return EncodingClass::NONE;
}


String::String( const ConstQualifier&, const void* content, std::size_t contentsize, Encoding encoding_, unsigned char codepage_)
	:m_encoding(encoding_),m_codepage(codepage_),m_isconst(true),m_size(contentsize/elementSize( encoding_)),m_ar((unsigned char*)content)
{}

