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
	Position,
	SizeX,
	SizeY
};

const char* variableName( Variable v);

class VariableValue
{
public:
	enum Type {Number, String};

	static VariableValue string( std::size_t idx);
	static VariableValue number( double value);

	std::string asString( const std::string strvalar) const;
	double asNumber( const std::string strvalar) const;
	Type type() const	{return m_type;}

private:
	Type m_type;
	union
	{
		double m_number;
		std::size_t m_string;
	} value;
};

class VariableScope
{
public:
	VariableScope();
	void push();
	void pop();

	void define( Variable var, const std::string& value);
	void define( Variable var, double value);
	void define( Variable var, Variable src);

	std::size_t getValueIdx( Variable var);
	VariableValue::Type getType( std::size_t idx) const;

	std::string getString( std::size_t idx);
	double getNumber( std::size_t idx);

private:
	typedef std::map<std::size_t,std::size_t> Map;
	std::vector<Map> m_ar;
	std::vector<VariableValue> m_valuear;
	std::string m_strings;
};


class ValueStack
{
public:
	ValueStack();

	void push( const std::string& value);
	void push( double value);
	void pop();

	VariableValue::Type type( std::size_t idx) const;
	double asNumber( std::size_t idx) const;
	std::string asString( std::size_t idx) const;

private:
	std::vector<VariableValue> m_valuear;
	std::string m_strings;
};

std::map <std::string, std::size_t>* getVariablenameMap();

}}
#endif

