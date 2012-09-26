/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file modules/normalize/locale/localeNormalize.cpp
///\brief Implementation of normalization functions based on boost locale (ICU)
#include "localeNormalize.hpp"
#include "utils/miscUtils.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>
#include <boost/locale/utf.hpp>
#include <boost/locale/boundary/index.hpp>

using namespace _Wolframe;
using namespace langbind;

namespace
{
typedef std::string (*LocaleConv)( std::string const&, std::locale const&);

template <boost::locale::norm_type NormType>
struct CompositionNormalizer
{
	static std::string localeConv( std::string const& str, std::locale const& loc)
	{
		return boost::locale::normalize( str, NormType, loc);
	}
};

struct EuropeanAsciiNormalizer
{
	static std::string localeConv( std::string const& str, std::locale const& loc)
	{
		std::string input_decomp = CompositionNormalizer<boost::locale::norm_nfkd>::localeConv( str, loc);
		std::string rt;
		std::string::const_iterator ii = input_decomp.begin(), ee = input_decomp.end();
		for (;ii != ee; ++ii)
		{
			if (*ii > 0) rt.push_back( *ii);
		}
		return rt;
	}
};

///\brief Same as EuropeanAsciiNormalizer but with special conversion for german umlauts
struct GermanAsciiNormalizer
{
	///\brief Unicode character for umlaut sign
	enum {UnicodeDiaresis=0xA8};

	static std::string localeConv( std::string const& str, std::locale const& loc)
	{
		std::string input_decomp = CompositionNormalizer<boost::locale::norm_nfkd>::localeConv( str, loc);
		std::string rt;
		std::string::const_iterator ii = input_decomp.begin(), ee = input_decomp.end();
		for (;ii != ee; ++ii)
		{
			if (*ii > 0) rt.push_back( *ii);
			if ((unsigned char)*ii == UnicodeDiaresis)
			{
				// Umlaut conversion:
				char lastchcase = rt[ rt.size()-1];
				char lastch = lastchcase | 32;
				if (lastch == 'a' || lastch == 'o' || lastch == 'u')
				{
					char ext = 'e';
					std::string::const_iterator nx = ii+1;
					if (lastch != lastchcase)
					{
						if (nx != ee)
						{
							if (*nx >= 'A' && *nx <= 'Z')
							{
								ext = 'E';
							}
							if (*nx <= 32 && *nx > 0)
							{
								ext = 'E';
							}
						}
					}
					rt.push_back( ext);
				}
			}
		}
		return rt;
	}
};

struct Tokenize
{
	static std::string latinWord( std::string const& str, std::locale const&)
	{
		std::string text = str;
		std::string rt;
		if (!str.empty())
		{
			boost::locale::boundary::ssegment_index index( boost::locale::boundary::word, text.begin(), text.end());
			boost::locale::boundary::ssegment_index::iterator ii = index.begin(), ee = index.end();
			for (; ii != ee; ++ii)
			{
				if(ii->rule() & boost::locale::boundary::word_any)
				{
					if (!rt.empty())
					{
						boost::locale::utf::utf_traits<char>::encode( 0x20, std::back_inserter( rt));
					}
					rt.append( *ii);
				}
			}
		}
		return rt;
	}
};

static std::vector<LocaleConv> parseFunc( const std::string& description)
{
	std::vector<LocaleConv> rt;
	std::string tok;
	utils::CharTable opTab( ",");
	std::string::const_iterator ii = description.begin(), ee = description.end();
	while (ii != ee)
	{
		char ch = utils::parseNextToken( tok, ii, ee, opTab);
		if (ch == '\0') break;
		if (ch == ',') throw std::runtime_error( "syntax error in normalize function description: unexpected ','");
		std::string name = boost::algorithm::to_lower_copy( tok);

		if (name == "toupper")
		{
			rt.push_back( &boost::locale::to_upper);
		}
		else if (name == "tolower")
		{
			rt.push_back( &boost::locale::to_lower);
		}
		else if (name == "totitle")
		{
			rt.push_back( &boost::locale::to_title);
		}
		else if (name == "foldcase")
		{
			rt.push_back( &boost::locale::fold_case);
		}
		else if (name == "nfd")
		{
			//Canonical decomposition
			rt.push_back( &CompositionNormalizer<boost::locale::norm_nfd>::localeConv);
		}
		else if (name == "nfc")
		{
			//Canonical decomposition followed by canonical composition.
			rt.push_back( &CompositionNormalizer<boost::locale::norm_nfc>::localeConv);
		}
		else if (name == "nfkd")
		{
			//Compatibility decomposition.
			rt.push_back( &CompositionNormalizer<boost::locale::norm_nfkd>::localeConv);
		}
		else if (name == "nfkc")
		{
			//Compatibility decomposition followed by canonical composition.
			rt.push_back( &CompositionNormalizer<boost::locale::norm_nfkc>::localeConv);
		}
		else if (name == "latinword")
		{
			rt.push_back( &Tokenize::latinWord);
		}
		else if (name == "ascii_de")
		{
			rt.push_back( &GermanAsciiNormalizer::localeConv);
		}
		else if (name == "ascii_eu")
		{
			rt.push_back( &EuropeanAsciiNormalizer::localeConv);
		}
		ch = utils::parseNextToken( tok, ii, ee, opTab);
		if (ch != ',' && ch != '\0') throw std::runtime_error( "comma ',' expected as normalization function separator");
	}
	return rt;
}
}//anonymous namespace

struct LocaleConvNormalizeFunction :public NormalizeFunction
{
	LocaleConvNormalizeFunction( ResourceHandle& reshnd, const std::string& lc, const std::vector<LocaleConv>& func)
		:m_reshnd(&reshnd)
		,m_func(func)
		,m_lc(lc){}

	virtual ~LocaleConvNormalizeFunction(){}

	virtual std::string execute( const std::string& i) const
	{
		std::locale loc = m_reshnd->getLocale( m_lc);
		if (m_func.empty()) return i;
		std::vector<LocaleConv>::const_iterator ii = m_func.begin(), ee = m_func.end();
		std::string rt = (*ii)( i, loc);
		for (++ii; ii != ee; ++ii)
		{
			rt = (*ii)( rt, loc);
		}
		return rt;
	}
private:
	ResourceHandle* m_reshnd;
	std::vector<LocaleConv> m_func;
	std::string m_lc;
};


NormalizeFunction* _Wolframe::langbind::createLocaleNormalizeFunction( ResourceHandle& reshnd, const std::string& description)
{
	std::vector<LocaleConv> func;
	utils::CharTable opTab( ":,");
	std::string lc,tok;
	std::string::const_iterator ii = description.begin(), ee = description.end();
	char ch = utils::parseNextToken( tok, ii, ee, opTab);
	if (ch != '\0' && ch != ':')
	{
		if (utils::parseNextToken( tok, ii, ee, opTab) == ':')
		{
			func = parseFunc( std::string( ii, ee));
			lc = tok;
		}
		else
		{
			func = parseFunc( description);
			lc = "";
		}
	}
	return new LocaleConvNormalizeFunction( reshnd, lc, func);
}

