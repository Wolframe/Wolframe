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

enum Variable
{
	Text,
	Index,
	Value,
	PositionX,
	PositionY,
	SizeX,
	SizeY
};
enum {NofVariables=(int)SizeY+1};

const char* variableName( Variable v);


class VariableScope
{
public:
	VariableScope();
	void push();
	void pop();

	void push_marker( std::size_t mi)				{m_mrkar.back().push_back(mi);}
	std::vector<std::size_t>::const_iterator begin_marker()	const	{return m_mrkar.back().begin();}
	std::vector<std::size_t>::const_iterator end_marker() const	{return m_mrkar.back().end();}

	void define( Variable var, const std::string& value);
	void define( Variable var, Variable src);

	std::size_t getValueIdx( Variable var) const;
	std::string getValue( std::size_t idx) const;

	class const_iterator
	{
	public:
		const_iterator( const const_iterator& o)		:m_visited(o.m_visited),m_itr(o.m_itr){}
		const_iterator( const VariableScope* visited)		:m_visited(visited),m_itr(0){skip();}
		const_iterator()					:m_visited(0),m_itr(NofVariables){}
		void skip()						{while (m_itr < (int)NofVariables && !m_visited->getValueIdx( (Variable)m_itr)) ++m_itr;}
		const_iterator& operator++()				{skip(); return *this;}
		const_iterator operator++(int)				{const_iterator rt(*this); skip(); return rt;}
		bool operator ==( const const_iterator& o) const	{return !compare(o);}
		bool operator !=( const const_iterator& o) const	{return compare(o);}
		bool operator <=( const const_iterator& o) const	{return compare(o)<=0;}
		bool operator >=( const const_iterator& o) const	{return compare(o)>=0;}
		bool operator <( const const_iterator& o) const		{return compare(o)<0;}
		bool operator >( const const_iterator& o) const		{return compare(o)>0;}

		const char* name() const				{return (m_itr < NofVariables)?variableName((Variable)m_itr):"";}
		std::string value() const				{return (m_itr < NofVariables)?m_visited->getValue( m_visited->getValueIdx( (Variable)m_itr)):std::string();}

	private:
		int compare( const const_iterator& o) const		{return m_itr - o.m_itr;}

	private:
		const VariableScope* m_visited;
		unsigned int m_itr;
	};

	const_iterator begin() const					{return const_iterator(this);}
	const_iterator end() const					{return const_iterator();}

private:
	typedef std::map<std::size_t,std::size_t> Map;
	std::vector<Map> m_ar;
	std::vector<std::vector<std::size_t> > m_mrkar;
	std::string m_strings;
};


class ValueStack
{
public:
	ValueStack();

	void push( const std::string& value);
	void pop( std::size_t nof=1);

	std::string top( std::size_t idx=0) const;

private:
	std::vector<std::size_t> m_valuear;
	std::string m_strings;
};

std::map <std::string, std::size_t>* getVariablenameMap();

}}
#endif

