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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file prnt/pdfPrinterVariable.hpp
///\brief Defines Variables of the simple PDF print function
#ifndef _Wolframe_PRNT_SIMPLE_PDF_PRINT_VARIABLE_HPP_INCLUDED
#define _Wolframe_PRNT_SIMPLE_PDF_PRINT_VARIABLE_HPP_INCLUDED
#include <string>
#include <map>
#include <vector>

namespace _Wolframe {
namespace prnt {

///\enum Variable
///\brief Fixed list of usable Variables (Registers)
enum Variable
{
	R0,R1,R2,R3,R4,R5,R6,R7,R8,R9,		//< general purpose registers
	Text,					//< text to be printed
	Index,					//< element counting register for elements printed with index
	PositionX,				//< X coordinate position
	PositionY,				//< Y coordinate position
	SizeX,					//< Size on the X axis of the coordinate system
	SizeY					//< Size on the Y axis of the coordinate system
};
enum {NofVariables=(int)SizeY+1};

const char* variableName( Variable v);


class VariableScope
{
public:
	VariableScope();

	void push();
	void push( const std::string& tag);
	void pop();

	void push_marker( std::size_t mi)				{return m_ar.back().m_mrk.push_back( mi);}

	typedef std::vector<std::size_t>::const_iterator const_marker_iterator;
	typedef std::vector<std::size_t>::iterator marker_iterator;

	const_marker_iterator begin_marker() const			{return m_ar.back().m_mrk.begin();}
	const_marker_iterator end_marker() const			{return m_ar.back().m_mrk.end();}

	typedef std::map<Variable,std::size_t>::const_iterator const_iterator;
	typedef std::map<Variable,std::size_t>::iterator iterator;

	const_iterator begin() const					{return m_ar.back().m_map.begin();}
	const_iterator end() const					{return m_ar.back().m_map.end();}

	void define( Variable var, const std::string& value, bool passToSibling=false);
	void define( Variable var, Variable src, bool passToSibling=false);

	std::size_t getValueIdx( Variable var) const;
	bool isDefined( Variable var) const				{return getValueIdx( var) != 0;}

	std::string getValue( std::size_t idx) const;

private:
	struct Area
	{
		std::map<Variable,std::size_t> m_map;
		std::vector<std::size_t> m_mrk;
		std::size_t m_tagidx;

		explicit Area( std::size_t tagidx_=0)
			:m_tagidx(tagidx_){}

		Area( const Area& o, std::size_t tagidx_)
			:m_map(o.m_map)
			,m_mrk(o.m_mrk)
			,m_tagidx(tagidx_){}

		Area( const Area& o)
			:m_map(o.m_map)
			,m_mrk(o.m_mrk)
			,m_tagidx(o.m_tagidx){}
	};
	std::vector<Area> m_ar;
	std::string m_strings;
	std::string m_tag;
	std::map< std::string, std::map<Variable,std::size_t> > m_tagvarmap;
};


class ValueStack
{
public:
	ValueStack();

	void push( const std::string& value);
	void pop( std::size_t nof=1);

	std::string top( std::size_t idx=0) const;
	std::string dump() const;

private:
	std::vector<std::size_t> m_valuear;
	std::string m_strings;
};

std::map <std::string, std::size_t>* getVariablenameMap();

}}
#endif

