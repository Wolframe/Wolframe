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
///\file prnt/pdfPrinterExpression.hpp
///\brief Defines expressions to express variable assignements in simple PDF printing document descriptions
#ifndef _Wolframe_PRNT_SIMPLE_PDF_PRINT_EXPRESSION_HPP_INCLUDED
#define _Wolframe_PRNT_SIMPLE_PDF_PRINT_EXPRESSION_HPP_INCLUDED
#include "prnt/pdfPrinterMethod.hpp"
#include "prnt/pdfPrinterVariable.hpp"
#include <string>
#include <map>
#include <vector>
#include <cstdlib>

namespace _Wolframe {
namespace prnt {

class Expression
{
public:
	enum Operator
	{
		UnaryMinus,		//< operator '-'
		Minus,			//< operator '-'
		Plus,			//< operator '+'
		Concat,			//< operator '#'
		Multiply,		//< operator '*'
		Divide,			//< operator '/'
		Round,			//< operator '@'
		Assign,			//< operator '='
		AssignUndefined,	//< operator '?='
		AssignGreater,		//< operator '?<'
		AssignSmaller		//< operator '?>'
	};

	static const char* operatorString( Operator i)
	{
		static const char* ar[] = {"-","-","+","#","*","/",":","=","?=","?<","?>"};
		return ar[ (int)i];
	}

public:
	Expression()
		:m_calc_precision(5){}
	Expression( const Expression& o)
		:m_ar(o.m_ar)
		,m_calc_precision(o.m_calc_precision){}

	void push_operator( Operator chr);
	void push_value( std::size_t idx);
	void push_variable( Variable::Id var);
	void push_tagvariable( Variable::Id var);
	void push_expression( const Expression& expr);

	std::size_t size() const		{return m_ar.size();}
	void setPrecision( unsigned int p)	{m_calc_precision=p;}

	void evaluate( VariableScope& vs, const std::string& exprstrings) const;

	std::string tostring( const std::string& exprstrings) const;

private:
	struct Item
	{
		enum Type {Variable,TagVariable,Value,Operator};
		Type m_type;
		union
		{
			std::size_t m_idx;
			Expression::Operator m_op;
		} value;
	};

private:
	std::vector<Item> m_ar;
	unsigned int m_calc_precision;
};


struct StateDef
{
	StateDef(){}

	StateDef( const StateDef& o)
		:m_expr(o.m_expr)
		,m_call(o.m_call){}

	void parse( std::string::const_iterator itr, const std::string::const_iterator& end, std::string& exprstrings);

	std::string tostring( const std::string& exprstrings) const;

	struct MethodCall
	{
		Method::Id m_method;
		Expression m_param;

		std::string tostring( const std::string& exprstrings) const;
	};
	Expression m_expr;
	std::vector<MethodCall> m_call;
};

}}
#endif

