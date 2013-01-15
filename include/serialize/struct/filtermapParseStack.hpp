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
///\file serialize/struct/filtermapParseStack.hpp
///\brief Defines the parsing stack for deserialization

#ifndef _Wolframe_SERIALIZE_STRUCT_FILTERMAP_PARSE_STACK_HPP_INCLUDED
#define _Wolframe_SERIALIZE_STRUCT_FILTERMAP_PARSE_STACK_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include "serialize/mapContext.hpp"
#include <vector>
#include <stdexcept>

namespace _Wolframe {
namespace serialize {

///\brief Forward declaration
class FiltermapParseStateStack;

class FiltermapParseState
{
public:
	typedef bool (*Parse)( langbind::TypedInputFilter& inp, Context& ctx, FiltermapParseStateStack& stk);

public:
	FiltermapParseState( const FiltermapParseState& o);
	FiltermapParseState( const char* name_, Parse p, void* v);
	~FiltermapParseState();

	std::size_t selectElement( std::size_t idx, std::size_t size);
	std::size_t initCount( std::size_t idx) const;

	void* value() const		{return m_value;}
	const char* name() const	{return m_name;}
	Parse parse() const		{return m_parse;}
	std::size_t state() const	{return m_stateidx;}
	void state( std::size_t idx)	{m_stateidx = idx;}
private:
	Parse m_parse;
	int* m_initar;
	std::size_t m_size;
	void* m_value;
	const char* m_name;
	std::size_t m_stateidx;
};

class FiltermapParseStateStack :public std::vector<FiltermapParseState>
{
public:
	FiltermapParseStateStack(){}
	FiltermapParseStateStack( const FiltermapParseStateStack& o)
		:std::vector<FiltermapParseState>(o)
		,m_valuebuf(o.m_valuebuf){}

	bool hasbufvalue() const					{return !m_valuebuf.empty();}
	void bufvalue( const langbind::TypedFilterBase::Element& e)	{m_valuebuf = e;}
	void clearbuf()							{m_valuebuf.clear();}
	const langbind::TypedFilterBase::Element& valuebuf() const	{return m_valuebuf;}

private:
	langbind::TypedFilterBase::Element m_valuebuf;
};

}}//namespace
#endif

