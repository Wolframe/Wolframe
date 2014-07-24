/************************************************************************

 Copyright (C) 2011 - 2014 Project Wolframe.
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
///\file localeNormalize.cpp
///\brief Implementation of normalization functions based on boost locale (ICU)
#include "localeNormalize.hpp"
#include "types/variant.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>
#include <boost/locale/utf.hpp>
#include <boost/locale/boundary/index.hpp>
// not nice, but draws in auto-linking (see bug #84)
#include <boost/thread.hpp>

using namespace _Wolframe;
using namespace langbind;

namespace
{

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

}//anonymous namespace

LocaleConvNormalizeFunction::LocaleConvNormalizeFunction( const LocaleConvNormalizeFunction& o)
	:m_reshnd(o.m_reshnd)
	,m_func(o.m_func)
	,m_lc(o.m_lc)
	,m_name(o.m_name){}
LocaleConvNormalizeFunction::LocaleConvNormalizeFunction( LocaleResourceHandle* reshnd, const std::vector<types::Variant>& arg, const LocaleConv& func, const char* name_)
	:m_reshnd(reshnd)
	,m_func(func)
	,m_lc(getLcFromArg(arg))
	,m_name(name_){}

types::Variant LocaleConvNormalizeFunction::execute( const types::Variant& inp) const
{
	if (inp.type() == types::Variant::String)
	{
		std::locale loc = m_reshnd->getLocale( m_lc);
		return m_func( inp.tostring(), loc);
	}
	else
	{
		return inp;
	}
}

types::NormalizeFunction* LocaleConvNormalizeFunction::copy() const
{
	return new LocaleConvNormalizeFunction( *this);
}

std::string LocaleConvNormalizeFunction::getLcFromArg( const std::vector<types::Variant>& arg)
{
	if (arg.empty()) return std::string();
	if (arg.size() > 1) std::runtime_error( "too many arguments passed to locale conversion normalize function");
	return arg.at(0).tostring();
}

ToLowerNormalizeFunction::ToLowerNormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::vector<types::Variant>& arg)
	:LocaleConvNormalizeFunction( dynamic_cast<LocaleResourceHandle*>(reshnd), arg, &boost::locale::to_lower<char>, "tolower"){}
ToUpperNormalizeFunction::ToUpperNormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::vector<types::Variant>& arg)
	:LocaleConvNormalizeFunction( dynamic_cast<LocaleResourceHandle*>(reshnd), arg, &boost::locale::to_upper<char>, "toupper"){}
ToTitleNormalizeFunction::ToTitleNormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::vector<types::Variant>& arg)
	:LocaleConvNormalizeFunction( dynamic_cast<LocaleResourceHandle*>(reshnd), arg, &boost::locale::to_title<char>, "totitle"){}
ToFoldcaseNormalizeFunction::ToFoldcaseNormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::vector<types::Variant>& arg)
	:LocaleConvNormalizeFunction( dynamic_cast<LocaleResourceHandle*>(reshnd), arg, &boost::locale::fold_case<char>, "foldcase"){}
NFDNormalizeFunction::NFDNormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::vector<types::Variant>& arg)
	:LocaleConvNormalizeFunction( dynamic_cast<LocaleResourceHandle*>(reshnd), arg, &CompositionNormalizer<boost::locale::norm_nfd>::localeConv, "nfd"){}
NFCNormalizeFunction::NFCNormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::vector<types::Variant>& arg)
	:LocaleConvNormalizeFunction( dynamic_cast<LocaleResourceHandle*>(reshnd), arg, &CompositionNormalizer<boost::locale::norm_nfd>::localeConv, "nfc"){}
NFKDNormalizeFunction::NFKDNormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::vector<types::Variant>& arg)
	:LocaleConvNormalizeFunction( dynamic_cast<LocaleResourceHandle*>(reshnd), arg, &CompositionNormalizer<boost::locale::norm_nfkd>::localeConv, "nfkd"){}
NFKCNormalizeFunction::NFKCNormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::vector<types::Variant>& arg)
	:LocaleConvNormalizeFunction( dynamic_cast<LocaleResourceHandle*>(reshnd), arg, &CompositionNormalizer<boost::locale::norm_nfkc>::localeConv, "nfkc"){}
LatinwordNormalizeFunction::LatinwordNormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::vector<types::Variant>& arg)
	:LocaleConvNormalizeFunction( dynamic_cast<LocaleResourceHandle*>(reshnd), arg, &Tokenize::latinWord, "latinword"){}
AsciiDeNormalizeFunction::AsciiDeNormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::vector<types::Variant>& arg)
	:LocaleConvNormalizeFunction( dynamic_cast<LocaleResourceHandle*>(reshnd), arg, &GermanAsciiNormalizer::localeConv, "ascii_de"){}
AsciiEuNormalizeFunction::AsciiEuNormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::vector<types::Variant>& arg)
	:LocaleConvNormalizeFunction( dynamic_cast<LocaleResourceHandle*>(reshnd), arg, &EuropeanAsciiNormalizer::localeConv, "ascii_eu"){}

