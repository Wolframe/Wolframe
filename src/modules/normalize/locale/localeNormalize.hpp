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
///\file localeNormalize.hpp
///\brief Interface for normalization functions based on boost locale (ICU)
#ifndef _LANGBIND_LOCALE_NORMALIZE_HPP_INCLUDED
#define _LANGBIND_LOCALE_NORMALIZE_HPP_INCLUDED
#include "types/normalizeFunction.hpp"
#include <vector>
#include <boost/locale/generator.hpp>
#include <boost/locale.hpp>

namespace _Wolframe {
namespace langbind {

class LocaleResourceHandle
	:public types::NormalizeResourceHandle
{
public:
	LocaleResourceHandle()
	{
		m_gen.locale_cache_enabled( true);
	}
	virtual ~LocaleResourceHandle(){}

	std::locale getLocale( const std::string& name)
	{
		return m_gen( name);
	}
private:
	boost::locale::generator m_gen;
};

class LocaleConvNormalizeFunction
	:public types::NormalizeFunction
{
public:
	typedef std::string (*LocaleConv)( std::string const&, std::locale const&);

	LocaleConvNormalizeFunction( const LocaleConvNormalizeFunction& o);
	LocaleConvNormalizeFunction( LocaleResourceHandle* reshnd, const std::vector<types::Variant>& arg, const LocaleConv& func, const char* name_);

	virtual ~LocaleConvNormalizeFunction(){}

	virtual types::Variant execute( const types::Variant& inp) const;

	virtual const char* name() const
	{
		return m_name;
	}

	virtual types::NormalizeFunction* copy() const;

private:
	static std::string getLcFromArg( const std::vector<types::Variant>& arg);
	
private:
	LocaleResourceHandle* m_reshnd;
	LocaleConv m_func;
	std::string m_lc;
	const char* m_name;
};


class ToLowerNormalizeFunction
	:public LocaleConvNormalizeFunction
{
public:	ToLowerNormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::vector<types::Variant>& arg);
};
class ToUpperNormalizeFunction
	:public LocaleConvNormalizeFunction
{
public:	ToUpperNormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::vector<types::Variant>& arg);
};

class ToTitleNormalizeFunction
	:public LocaleConvNormalizeFunction
{
public:	ToTitleNormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::vector<types::Variant>& arg);
};

class ToFoldcaseNormalizeFunction
	:public LocaleConvNormalizeFunction
{
public:	ToFoldcaseNormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::vector<types::Variant>& arg);
};

class NFDNormalizeFunction
	:public LocaleConvNormalizeFunction
{
public:	NFDNormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::vector<types::Variant>& arg);
};

class NFCNormalizeFunction
	:public LocaleConvNormalizeFunction
{
public:	NFCNormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::vector<types::Variant>& arg);
};

class NFKDNormalizeFunction
	:public LocaleConvNormalizeFunction
{
public:	NFKDNormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::vector<types::Variant>& arg);
};

class NFKCNormalizeFunction
	:public LocaleConvNormalizeFunction
{
public:	NFKCNormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::vector<types::Variant>& arg);
};

class LatinwordNormalizeFunction
	:public LocaleConvNormalizeFunction
{
public:	LatinwordNormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::vector<types::Variant>& arg);
};

class AsciiDeNormalizeFunction
	:public LocaleConvNormalizeFunction
{
public:	AsciiDeNormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::vector<types::Variant>& arg);
};

class AsciiEuNormalizeFunction
	:public LocaleConvNormalizeFunction
{
public:	AsciiEuNormalizeFunction( types::NormalizeResourceHandle* reshnd, const std::vector<types::Variant>& arg);
};

}}//namespace
#endif

