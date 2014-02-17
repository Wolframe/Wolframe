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
///\file modules/normalize/locale/localeNormalize.cpp
///\brief Implementation of normalization functions based on boost locale (ICU)
#include "localeNormalize.hpp"
#include "types/variant.hpp"
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

}//anonymous namespace

struct LocaleConvNormalizeFunction
	:public types::NormalizeFunction
{
	LocaleConvNormalizeFunction( LocaleResourceHandle& reshnd, const std::string& lc, const LocaleConv& func, const char* name_)
		:m_reshnd(&reshnd)
		,m_func(func)
		,m_lc(lc)
		,m_name(name_){}

	virtual ~LocaleConvNormalizeFunction(){}

	virtual types::Variant execute( const types::Variant& inp) const
	{
		std::locale loc = m_reshnd->getLocale( m_lc);
		return m_func( inp.tostring(), loc);
	}
	virtual const char* name() const {return m_name;}

private:
	LocaleResourceHandle* m_reshnd;
	LocaleConv m_func;
	std::string m_lc;
	const char* m_name;
};

namespace _Wolframe {
namespace langbind {

types::NormalizeFunction* create_tolower_NormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::string& arg)
{
	LocaleResourceHandle* myreshnd = dynamic_cast<LocaleResourceHandle*>(reshnd);
	return new LocaleConvNormalizeFunction( *myreshnd, arg, &boost::locale::to_lower, "tolower");
}

types::NormalizeFunction* create_toupper_NormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::string& arg)
{
	LocaleResourceHandle* myreshnd = dynamic_cast<LocaleResourceHandle*>(reshnd);
	return new LocaleConvNormalizeFunction( *myreshnd, arg, &boost::locale::to_upper, "toupper");
}

types::NormalizeFunction* create_totitle_NormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::string& arg)
{
	LocaleResourceHandle* myreshnd = dynamic_cast<LocaleResourceHandle*>(reshnd);
	return new LocaleConvNormalizeFunction( *myreshnd, arg, &boost::locale::to_title, "totitle");
}

types::NormalizeFunction* create_foldcase_NormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::string& arg)
{
	LocaleResourceHandle* myreshnd = dynamic_cast<LocaleResourceHandle*>(reshnd);
	return new LocaleConvNormalizeFunction( *myreshnd, arg, &boost::locale::fold_case, "foldcase");
}

types::NormalizeFunction* create_nfd_NormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::string& arg)
{
	LocaleResourceHandle* myreshnd = dynamic_cast<LocaleResourceHandle*>(reshnd);
	return new LocaleConvNormalizeFunction( *myreshnd, arg, &CompositionNormalizer<boost::locale::norm_nfd>::localeConv, "nfd");
}

types::NormalizeFunction* create_nfc_NormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::string& arg)
{
	LocaleResourceHandle* myreshnd = dynamic_cast<LocaleResourceHandle*>(reshnd);
	return new LocaleConvNormalizeFunction( *myreshnd, arg, &CompositionNormalizer<boost::locale::norm_nfd>::localeConv, "nfc");
}

types::NormalizeFunction* create_nfkd_NormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::string& arg)
{
	LocaleResourceHandle* myreshnd = dynamic_cast<LocaleResourceHandle*>(reshnd);
	return new LocaleConvNormalizeFunction( *myreshnd, arg, &CompositionNormalizer<boost::locale::norm_nfkd>::localeConv, "nfkd");
}

types::NormalizeFunction* create_nfkc_NormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::string& arg)
{
	LocaleResourceHandle* myreshnd = dynamic_cast<LocaleResourceHandle*>(reshnd);
	return new LocaleConvNormalizeFunction( *myreshnd, arg, &CompositionNormalizer<boost::locale::norm_nfkc>::localeConv, "nfkc");
}

types::NormalizeFunction* create_latinword_NormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::string& arg)
{
	LocaleResourceHandle* myreshnd = dynamic_cast<LocaleResourceHandle*>(reshnd);
	return new LocaleConvNormalizeFunction( *myreshnd, arg, &Tokenize::latinWord, "latinword");
}

types::NormalizeFunction* create_ascii_de_NormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::string& arg)
{
	LocaleResourceHandle* myreshnd = dynamic_cast<LocaleResourceHandle*>(reshnd);
	return new LocaleConvNormalizeFunction( *myreshnd, arg, &GermanAsciiNormalizer::localeConv, "ascii_de");
}

types::NormalizeFunction* create_ascii_eu_NormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::string& arg)
{
	LocaleResourceHandle* myreshnd = dynamic_cast<LocaleResourceHandle*>(reshnd);
	return new LocaleConvNormalizeFunction( *myreshnd, arg, &EuropeanAsciiNormalizer::localeConv, "ascii_eu");
}

}}//namespace
