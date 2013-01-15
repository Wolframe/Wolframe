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
///\file utils/format.hpp
///\brief Defines formated parsing,printing
#ifndef _Wolframe_UTILS_FORMAT_HPP_INCLUDED
#define _Wolframe_UTILS_FORMAT_HPP_INCLUDED
#include <string>
#include <vector>
#include <cstdint>

namespace _Wolframe {
namespace serialize {

class FormatDescriptionElement
{
public:
	typedef void (*Parse)( std::const_iterator& itr, const std::const_iterator end, void* ref);
	typedef void (*Print)( std::string& dest, void* ref);

public:
	FormatDescriptionElement()
		:m_parse(0)
		,m_print(0)
		,m_ofs(0){}

	FormatDescriptionElement( Parse r, Print p, std::size_t i)
		:m_parse(r)
		,m_print(p)
		,m_ofs(i){}

	FormatDescriptionElement( const FormatDescriptionElement& o)
		:m_parse(o.m_parse)
		,m_print(o.m_print)
		,m_ofs(o.m_ofs){}

	void parse( std::const_iterator& itr, const std::const_iterator end, void* ref)
	{
		m_parse( itr, end, ref);
	}

	void print( std::string& dest, void* ref)
	{
		m_print( dest, ref);
	}

private:
	Parse m_parse;
	Print m_print;
	std::size_t m_ofs;
};

class FormatDescriptionBase
{
public:
	FormatDescriptionBase();
	void define( const std::string& tag, const FormatDescriptionElement& e)
	{
		if (m_ar.find( tag) != m_map.end()) throw std::runtime_error( "duplicate tag definition in format string");
		m_map[ tag] = e;
	}

private:
	std::map<std::string, FormatDescriptionElement> m_map;
};


template <class Structure>
struct FormatDescription :public FormatDescriptionBase
{
	///\brief Constructor
	FormatDescription()
		:FormatDescriptionBase( sizeof(Structure)){}

	template <typename Element>
	FormatDescription& operator()( const char* tag, Element Structure::*eptr)
	{
		std::size_t ofs = (std::size_t)&(((Structure*)0)->*eptr);
		define( tag, FormatDescriptionElement( &Element::parse, &Element::print, ofs));
	}

private:
	static std::string parsetag( std::string::const_iterator& itr, const std::string::const_iterator end)
	{
		std::string rt;
		while (itr != end && ((*itr|32) >= 'a' && (*itr|32) <= 'z') || *itr == '_')
		{
			rt.push_back( *itr);
			++itr;
		}
		if (rt.emtpy()) throw( std::runtime_error( "illegal element in format string");
		return rt;
	}
};
}}
#endif
