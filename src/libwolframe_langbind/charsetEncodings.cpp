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
///\file charsetEncodings.cpp
///\brief Implementation of character set encoding transformations for modules that do support only one or some of them
#include "langbind/charsetEncodings.hpp"
#include "textwolf/charset.hpp"
#include "textwolf/cstringiterator.hpp"
#include "textwolf/textscanner.hpp"
#include <string>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

namespace {
template <class ENC>
struct CharsetEncodingInstanceBase
	:public CharsetEncodingStruct
{
	CharsetEncodingInstanceBase( const ENC& encoding_=ENC())
		:m_encoding(encoding_){}
	virtual ~CharsetEncodingInstanceBase(){}

	virtual void convertFromUTF8( std::string& dest, const char* content, std::size_t contentsize) const
	{
		textwolf::CStringIterator itr( content, contentsize);
		textwolf::TextScanner<textwolf::CStringIterator,textwolf::charset::UTF8> ts( itr);
		textwolf::UChar ch;
		for (; (ch = ts.chr()) != 0; ++ts)
		{
			m_encoding.print( ch, dest);
		}
	}
	virtual void convertToUTF8( std::string& dest, const char* content, std::size_t contentsize) const
	{
		textwolf::CStringIterator itr( content, contentsize);
		textwolf::TextScanner<textwolf::CStringIterator,ENC> ts( m_encoding, itr);
		textwolf::UChar ch;
		textwolf::charset::UTF8 utf8;
		for (; (ch = ts.chr()) != 0; ++ts)
		{
			utf8.print( ch, dest);
		}
	}
private:
	ENC m_encoding;
};

template <class ENC>
struct CharsetEncodingInstance
	:public CharsetEncodingInstanceBase<ENC>
{
	CharsetEncodingInstance( const ENC& encoding_=ENC())
		:CharsetEncodingInstanceBase<ENC>(encoding_){}
	virtual ~CharsetEncodingInstance(){}
};

template <>
struct CharsetEncodingInstance<textwolf::charset::UTF16BE>
	:public CharsetEncodingInstanceBase<textwolf::charset::UTF16BE>
{
	CharsetEncodingInstance( const textwolf::charset::UTF16BE& encoding_=textwolf::charset::UTF16BE())
		:CharsetEncodingInstanceBase<textwolf::charset::UTF16BE>(encoding_){}

	typedef CharsetEncodingInstanceBase<textwolf::charset::UTF16BE> Parent;

	virtual void convertToUTF8( std::string& dest, const char* content, std::size_t contentsize) const
	{
		std::size_t BOMofs = 0;
		if (contentsize >= 2)
		{
			if ((unsigned char)content[0] == 0xFF)
			{
				if ((unsigned char)content[1] == 0xFE) BOMofs = 2;
			}
			else if ((unsigned char)content[0] == 0xFE)
			{
				if ((unsigned char)content[1] == 0xFF) BOMofs = 2;
			}
		}
		Parent::convertToUTF8( dest, content+BOMofs, contentsize-BOMofs);
	}
};

template <>
struct CharsetEncodingInstance<textwolf::charset::UTF16LE>
	:public CharsetEncodingInstanceBase<textwolf::charset::UTF16LE>
{
	CharsetEncodingInstance( const textwolf::charset::UTF16LE& encoding_=textwolf::charset::UTF16LE())
		:CharsetEncodingInstanceBase<textwolf::charset::UTF16LE>(encoding_){}

	typedef CharsetEncodingInstanceBase<textwolf::charset::UTF16LE> Parent;

	virtual void convertToUTF8( std::string& dest, const char* content, std::size_t contentsize) const
	{
		std::size_t BOMofs = 0;
		if (contentsize >= 2)
		{
			if ((unsigned char)content[0] == 0xFF)
			{
				if ((unsigned char)content[1] == 0xFE) BOMofs = 2;
			}
			else if ((unsigned char)content[0] == 0xFE)
			{
				if ((unsigned char)content[1] == 0xFF) BOMofs = 2;
			}
		}
		Parent::convertToUTF8( dest, content+BOMofs, contentsize-BOMofs);
	}
};
}//anonymous namespace

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

CharsetEncoding langbind::getCharsetEncoding( const std::string& name)
{
	CharsetEncodingStruct* st = 0;
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
		if (codepage[0] == '1')
		{
			st = new CharsetEncodingInstance<textwolf::charset::IsoLatin>();
		}
		else
		{
			st = new CharsetEncodingInstance<textwolf::charset::IsoLatin>( textwolf::charset::IsoLatin( codepage[0] - '0'));
		}
	}
	else if (enc.size() == 0 || enc == "utf8")
	{
		st = new CharsetEncodingInstance<textwolf::charset::UTF8>();
	}
	else if (enc == "utf16" || enc == "utf16be")
	{
		st = new CharsetEncodingInstance<textwolf::charset::UTF16BE>();
	}
	else if (enc == "utf16le")
	{
		st = new CharsetEncodingInstance<textwolf::charset::UTF16LE>();
	}
	else if (enc == "ucs2" || enc == "ucs2be")
	{
		st = new CharsetEncodingInstance<textwolf::charset::UCS2BE>();
	}
	else if (enc == "ucs2le")
	{
		st = new CharsetEncodingInstance<textwolf::charset::UCS2LE>();
	}
	else if (enc == "ucs4" || enc == "ucs4be")
	{
		st = new CharsetEncodingInstance<textwolf::charset::UCS4BE>();
	}
	else if (enc == "ucs4le")
	{
		st = new CharsetEncodingInstance<textwolf::charset::UCS4LE>();
	}
	else
	{
		throw std::runtime_error( "unknown character set encoding for converter");
	}
	return CharsetEncoding(st);
}


std::string langbind::convertStringCharsetToUTF8( const CharsetEncoding& encoding, const std::string& content)
{
	std::string rt;
	encoding->convertToUTF8( rt, content.c_str(), content.size());
	return rt;
}

std::string langbind::convertStringUTF8ToCharset( const CharsetEncoding& encoding, const std::string& content)
{
	std::string rt;
	encoding->convertFromUTF8( rt, content.c_str(), content.size());
	return rt;
}

CharsetClass::Id CharsetClass::guess( const char* content, std::size_t size)
{
	CharsetClass::Id rt = NONE;
	int maxll = 0;
	int ll = 0;			//< number of zeroes in a row
	std::size_t idx = 0;

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
				if (ll > 3) return FAIL;
			}
		}
	}
	if (maxll == 3)
	{
		rt = (Id)((int)rt | (int)U4);
	}
	else if (maxll == 1)
	{
		rt = (Id)((int)rt | (int)U2);
	}
	else
	{
		rt = (Id)((int)rt | (int)U1);
	}
	if (size > 2 && maxll == 1)
	{
		// ... UTF16 check BOM
		if ((unsigned char)content[0] == 0xFE && (unsigned char)content[1] == 0xFF) rt = (Id)((int)rt | (int)BE);
		if ((unsigned char)content[0] == 0xFF && (unsigned char)content[1] == 0xFE) rt = (Id)((int)rt | (int)LE);
	}
	if (((int)rt & ((int)LE | (int)BE)) == 0)
	{
		// ... indians not set
		for (idx=0; idx < size; ++idx)
		{
			if (content[idx] == '\0')
			{
				if (maxll == 3)
				{
					if (idx % 4 == 0)
					{
						//... zero at the start of a character, means allways BE
						rt = (Id)((int)rt | (int)BE);
					}
					else if (idx % 4 == 3)
					{
						//... zero at the end of a character, means allways LE
						rt = (Id)((int)rt | (int)LE);
					}
				}
				if (maxll == 1)
				{
					if (idx % 2 == 0)
					{
						//... zero at the start of a character, means allways BE
						rt = (Id)((int)rt | (int)BE);
					}
					else if (idx % 2 == 1)
					{
						//... zero at the end of a character, means allways LE
						rt = (Id)((int)rt | (int)LE);
					}
				}
			}
		}
	}
	if (((int)rt & ((int)LE | (int)BE)) == ((int)LE | (int)BE))
	{
		// ... indians contradicting
		return NONE;
	}
	return rt;
}


