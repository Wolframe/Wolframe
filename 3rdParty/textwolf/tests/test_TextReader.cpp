#include "textwolf.hpp"
#include <iostream>
#include <stdio.h>

//build gcc
//compile: g++ -c -o test_TextReader.o -g -I../include/ -pedantic -Wall -O4 test_TextReader.cpp
//link: g++ -lc -o test_TextReader test_TextReader.o
//build windows
//compile: cl.exe /wd4996 /Ob2 /O2 /EHsc /MT /W4 /nologo /I..\include /D "WIN32" /D "_WINDOWS" /Fo"test_TextReader.obj" test_TextReader.cpp 
//link: link.exe /out:.\test_TextReader test_TextReader.obj

unsigned int character( unsigned int idx, unsigned int maxchar)
{
	unsigned int rt = idx;
	if (idx < 1000) rt =  idx;
	else if (idx < 2000) rt =  idx + (idx-1000) * 3;
	else if (idx < 3000) rt =  idx + (idx-2000) * 17;
	else if (idx < 4000) rt =  idx + (idx-3000) * 123;
	else if (idx < 5000) rt =  idx + (idx-4000) * 376;
	else if (idx < 6000) rt =  idx + (idx-5000) * 377;
	else if (idx < 7000) rt =  idx + (idx-6000) * 7231;
	else if (idx < 8000) rt =  idx + (idx-7000) * 7232;
	else if (idx < 9000) rt =  idx + (idx-8000) * 33121;
	else if (idx < 10000) rt =  idx + (idx-9000) * 33122;
	rt = ((rt % (maxchar-1)) + 1);
	if (maxchar == 0x10FFFF/*UTF-16 HACK*/)
	{
		if (rt >= 0xD800 && rt < 0xDC00) rt = rt - 0xD800;
	}
	return rt;
}
#define NOF_TESTS 10000

using namespace textwolf;

template <class CharSet>
class EnumCharIterator
{
public:
	unsigned int ii;
	unsigned int pos;
	StaticBuffer buf;

public:
	bool eof() const
	{
		return (ii >= NOF_TESTS && pos == buf.size());
	}

	bool skip()
	{
		pos++;
		if (pos >= buf.size())
		{
			if (ii >= NOF_TESTS)
			{
				pos = buf.size();
				return false;
			}
			else
			{
				buf.clear();
				unsigned int tt = character( ++ii, CharSet::MaxChar);
				CharSet::print( tt, buf);
				pos = 0;
			}
		}
		return true;
	}

public:
	EnumCharIterator( const EnumCharIterator& o)
		:ii(o.ii),pos(o.pos),buf(o.buf){}

	explicit EnumCharIterator( unsigned int start=0)
		:pos(0),buf(16)
	{
		ii = start;
		skip();
	}

	char operator *() const
	{
		if (eof()) return 0;
		return buf[ pos];
	}
	EnumCharIterator& operator ++()
	{
		skip();
		return *this;
	}
};

template <class CharSet>
struct TextScannerTest
{
	typedef EnumCharIterator<CharSet> ThisEnumCharIterator;
	typedef TextScanner<ThisEnumCharIterator,CharSet> ThisTextScanner;
	unsigned int start;
	unsigned int rndSeed;
	const char* name;

	TextScannerTest( const char* nam)
		:start(0),rndSeed(0),name(nam) {};

	unsigned int rnd()
	{
		rndSeed = (rndSeed+1) * 2654435761u;
		return rndSeed;
	}

	unsigned int operator*()
	{
		ThisEnumCharIterator thisEnumCharIterator(start);
		ThisTextScanner tr( thisEnumCharIterator);
		unsigned int ii;
		UChar chr;

		for (ii=start; tr.control() != EndOfText && ii<NOF_TESTS; ++ii,++tr)
		{
			UChar echr = character( ii+1, CharSet::MaxChar);
			unsigned int rr;
			do
			{
				rr = rnd() % 3;
				char aa,ascii;

				switch (rr&1)
				{
					case 0:
						ascii = tr.ascii();
						aa = (echr > 127)?0:(char)(unsigned char)(echr & 0xff);
						if (ascii != aa)
						{
							printf ("ascii %d != %d unexpected\n", ascii, aa);
							return ii;
						}
					break;
					case 1:
						chr=tr.chr();
						if (chr != echr)
						{
							printf ("character %d != %d unexpected\n", chr, echr);
							return ii;
						}
					break;
				}
			} while (rr>1);
		}
		if (ii < NOF_TESTS) return ii+1;
		printf ("PASSED Test %s\n", name);
		return 0;
	}
};

static const char* testAll()
{
	struct Error
	{
		char buf[ 256];
		Error()
		{
			buf[0] = '\0';
		}
		const char* get( const char* testname, unsigned int pos)
		{
			if (pos) snprintf( buf, sizeof(buf), "test %s failed at character pos %u", testname, pos);
			return buf[0]==0?0:buf;
		}
		const char* operator*()
		{
			return buf[0]==0?0:buf;
		}
	};
	static Error error;
	if (error.get( "UTF8",		*TextScannerTest< charset::UTF8>( "UTF-8"))
	||  error.get( "IsoLatin1",	*TextScannerTest< charset::IsoLatin1>( "Isolatin"))
	||  error.get( "UCS2BE",	*TextScannerTest< charset::UCS2BE>( "UCS-2BE"))
	||  error.get( "UCS2LE",	*TextScannerTest< charset::UCS2LE>( "UCS-2LE"))
	||  error.get( "UCS4BE",	*TextScannerTest< charset::UCS4BE>( "UCS-4BE"))
	||  error.get( "UCS4LE",	*TextScannerTest< charset::UCS4LE>( "UCS-4LE"))
	||  error.get( "UTF16BE",	*TextScannerTest< charset::UTF16BE>( "UTF-16BE"))
	||  error.get( "UTF16LE",	*TextScannerTest< charset::UTF16LE>( "UTF-16LE"))
	) return *error;
	return 0;
}

int main( int, const char**)
{
	const char* res = testAll();
	if (!res)
	{
		std::cerr << "OK" << std::endl;
		return 0;
	}
	else
	{
		std::cerr << res << std::endl;
		return 1;
	}
}




