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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file pdfPrinterExpression.cpp
#include "pdfPrinterExpression.hpp"
#include "pdfPrinterVariable.hpp"
#include "utils/parseUtils.hpp"
#include "utils/conversions.hpp"
#include "types/integer.hpp"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <limits>

using namespace std;

using namespace _Wolframe;
using namespace _Wolframe::prnt;

void Expression::push_operator( Operator opr)
{
	Item ee;
	ee.m_type = Item::Operator;
	ee.value.m_op = opr;
	m_ar.push_back( ee);
}

void Expression::push_value( std::size_t idx)
{
	Item ee;
	ee.m_type = Item::Value;
	ee.value.m_idx = idx;
	m_ar.push_back( ee);
}

void Expression::push_variable( Variable::Id var)
{
	Item ee;
	ee.m_type = Item::Variable;
	ee.value.m_idx = (std::size_t)var;
	m_ar.push_back( ee);
}

void Expression::push_tagvariable( Variable::Id var)
{
	Item ee;
	ee.m_type = Item::TagVariable;
	ee.value.m_idx = (std::size_t)var;
	m_ar.push_back( ee);
}

void Expression::push_expression( const Expression& expr)
{
	m_ar.insert( m_ar.end(), expr.m_ar.begin(), expr.m_ar.end());
}

//\brief Number type stub. 
///TODO To be DELETED and replaced with custom data type
class Number
{
public:
	Number( const Number& o)
		:m_value(o.m_value),m_fracdigits(o.m_fracdigits){}

	void init( const std::string& v)
	{
		std::string::const_iterator vi = v.begin(), ve = v.end();
		std::string xx;
		for (; vi != ve && *vi != '.'; ++vi) xx.push_back(*vi);
		if (vi != ve)
		{
			for (++vi; vi != ve && *vi != '.'; ++vi,++m_fracdigits) xx.push_back(*vi);
		}
		if (vi != ve) throw std::runtime_error( "illegal variable value");
		m_value = utils::toint_cast( xx);
	}

	explicit Number( const std::string& v)
		:m_value(0),m_fracdigits(0)
	{
		init( v);
	}

	Number round( const Number& o) const
	{
		Number rt(*this);
		_WOLFRAME_INTEGER factor = 1;
		while (rt.m_fracdigits > o.m_fracdigits)
		{
			factor *= 10;
			rt.m_fracdigits -= 1;
		}
		rt.m_value = (rt.m_value + 5) / factor;
		return rt;
	}

	Number( const std::string& v, int precision)
		:m_value(0),m_fracdigits(0)
	{
		init( v);
		while (precision > m_fracdigits)
		{
			m_fracdigits += 1;
			m_value *= 10;
		}
	}

	Number equalizeFracDigits( const Number& o) const
	{
		Number rt(o);
		if (m_fracdigits < o.m_fracdigits)
		{
			do
			{
				rt.m_value /= 10;
				rt.m_fracdigits -= 1;
			}
			while (m_fracdigits < rt.m_fracdigits);
		}
		else if (m_fracdigits > o.m_fracdigits)
		{
			do
			{
				rt.m_value *= 10;
				rt.m_fracdigits += 1;
			}
			while (m_fracdigits > rt.m_fracdigits);
		}
		return rt;
	}

	int compare( const Number& o) const
	{
		if (m_fracdigits == o.m_fracdigits)
		{
			return -1 + (int)(m_value > o.m_value) + (int)(m_value >= o.m_value);
		}
		else if (m_fracdigits > o.m_fracdigits)
		{
			Number xx( equalizeFracDigits( o));
			return -1 + (int)(m_value > xx.m_value) + (int)(m_value >= xx.m_value);
		}
		else
		{
			Number xx( equalizeFracDigits( *this));
			return -1 + (int)(xx.m_value > o.m_value) + (int)(xx.m_value >= o.m_value);
		}
	}

	bool operator<(const Number& o) const	{return compare(o)<0;}
	bool operator<=(const Number& o) const	{return compare(o)<=0;}
	bool operator>(const Number& o) const	{return compare(o)>0;}
	bool operator>=(const Number& o) const	{return compare(o)>=0;}
	bool operator==(const Number& o) const	{return compare(o)==0;}
	bool operator!=(const Number& o) const	{return compare(o)!=0;}

	Number operator+( const Number& o) const
	{
		Number aa( *this);
		Number bb( o);
		if (aa.m_fracdigits > bb.m_fracdigits)
		{
			aa.equalizeFracDigits( bb);
		}
		else if (aa.m_fracdigits < bb.m_fracdigits)
		{
			bb.equalizeFracDigits( aa);
		}
		aa.m_value += bb.m_value;
		return aa;
	}

	Number operator-( const Number& o) const
	{
		Number aa( *this);
		Number bb( o);
		if (aa.m_fracdigits > bb.m_fracdigits)
		{
			aa.equalizeFracDigits( bb);
		}
		else if (aa.m_fracdigits < bb.m_fracdigits)
		{
			bb.equalizeFracDigits( aa);
		}
		aa.m_value -= bb.m_value;
		return aa;
	}

	Number operator*( const Number& o) const
	{
		Number rt( o);
		rt.m_value *= m_value;
		rt.m_fracdigits += o.m_fracdigits;
		return rt;
	}

	Number operator/( const Number& o) const
	{
		Number rt( *this);
		rt.m_value /= o.m_value;
		rt.m_fracdigits -= o.m_fracdigits;
		return rt;
	}

	Number operator-() const
	{
		Number rt( *this);
		rt.m_value = -m_value;
		return rt;
	}

	std::string tostring() const
	{
		std::string dg( utils::tostring_cast( m_value));
		if (m_fracdigits < 0)
		{
			dg.append( (std::size_t)-m_fracdigits, '0');
			return dg;
		}
		else if (m_fracdigits > 0)
		{
			int nn = dg.size() - (int)(m_value < 0);
			if (m_fracdigits > nn)
			{
				std::string rt;
				if (m_value < 0)
				{
					rt.push_back('-');
					rt.append( "0.");
					rt.append( std::string( m_fracdigits-nn-1, '0'));
					rt.append( dg.c_str()+1);
				}
				else if (m_value > 0)
				{
					rt.append( "0.");
					rt.append( std::string( m_fracdigits-nn-1, '0'));
					rt.append( dg);
				}
				else
				{
					rt.append( "0");
				}
				return rt;
			}
			else
			{
				return dg.insert (dg.size()-m_fracdigits, 1, '.');
			}
		}
		else
		{
			return dg;
		}
	}

private:
	_WOLFRAME_INTEGER m_value;
	int m_fracdigits;
};


void Expression::evaluate( VariableScope& vscope, const std::string& exprstrings) const
{
	ValueStack stk;
	std::vector<Item>::const_iterator next,itr=m_ar.begin(), end=m_ar.end();
	for (; itr != end; ++itr)
	{
		bool passToSibling = false;
		switch (itr->m_type)
		{
			case Item::TagVariable:
				passToSibling = true;
				/*no break here*/
			case Item::Variable:
				next = itr+1;
				if (next != end && next->m_type == Item::Operator && (next->value.m_op == Assign || next->value.m_op == AssignUndefined || next->value.m_op == AssignGreater || next->value.m_op == AssignSmaller))
				{
					// handle special case of variable on left side of assignment:
					Variable::Id var = (Variable::Id)itr->value.m_idx;
					if (next->value.m_op == AssignUndefined)
					{
						// conditional assigment (define if not defined yet)
						if (!vscope.isDefined( var))
						{
							vscope.define( var, stk.top(), passToSibling);
						}
						else if (passToSibling)
						{
							vscope.define( var, var, passToSibling);
						}
						stk.pop();
					}
					else if (next->value.m_op == AssignGreater)
					{
						// conditional assigment (define if not defined yet or greater)
						std::string stkval = stk.top();
						stk.pop();
						std::size_t varidx = vscope.getValueIdx( var);
						if (!varidx)
						{
							vscope.define( var, stkval, passToSibling);
							break;
						}
						Number varval( vscope.getValue( varidx));
						if (varval < Number(stkval))
						{
							vscope.define( var, stkval, passToSibling);
							break;
						}
						if (passToSibling)
						{
							vscope.define( var, var, passToSibling);
						}
					}
					else if (next->value.m_op == AssignSmaller)
					{
						// conditional assigment (define if not defined yet or smaller)
						std::string stkval = stk.top();
						stk.pop();
						std::size_t varidx = vscope.getValueIdx( var);
						if (!varidx)
						{
							vscope.define( var, stkval, passToSibling);
							break;
						}
						Number varval( vscope.getValue( varidx));
						if (varval < Number(stkval))
						{
							vscope.define( var, stkval, passToSibling);
							break;
						}
						if (passToSibling)
						{
							vscope.define( var, var, passToSibling);
						}
					}
					else if (next->value.m_op == Assign)
					{
						// normal assigment
						vscope.define( var, stk.top(), passToSibling);
						stk.pop();
					}
					++itr;
				}
				else
				{
					// for other variables are the referenced values expanded:
					Variable::Id var = (Variable::Id)itr->value.m_idx;
					std::size_t idx = vscope.getValueIdx( var);

					if (idx == 0) throw std::runtime_error( std::string( "variable not defined '") + variableName(var) + "'");
					stk.push( vscope.getValue( idx));
				}
				break;

			case Item::Value:
				stk.push( std::string( exprstrings.c_str() + itr->value.m_idx));
				break;

			case Item::Operator:
				switch (itr->value.m_op)
				{
					case Assign:
					case AssignUndefined:
					case AssignGreater:
					case AssignSmaller:
						throw std::logic_error("internal: assignement assignement to value");
					break;
					case UnaryMinus:
					{
						Number op( stk.top());
						stk.pop();
						stk.push( (-op).tostring());
						break;
					}
					case Multiply:
					{
						Number op1( stk.top(0), m_calc_precision);
						Number op2( stk.top(1), m_calc_precision);
						stk.pop( 2);
						stk.push( Number( op1 * op2).tostring());
						break;
					}
					case Divide:
					{
						Number op1( stk.top(0), m_calc_precision);
						Number op2( stk.top(1), m_calc_precision);
						stk.pop( 2);
						stk.push( Number( op1 / op2).tostring());
						break;
					}
					case Round:
					{
						Number op1( stk.top(0), m_calc_precision);
						Number op2( stk.top(1), m_calc_precision);
						stk.pop( 2);
						stk.push( op1.round( op2).tostring());
						break;
					}
					case Plus:
					{
						Number op1( stk.top(0), m_calc_precision);
						Number op2( stk.top(1), m_calc_precision);
						stk.pop( 2);
						stk.push( Number( op1 + op2).tostring());
						break;
					}
					case Minus:
					{
						Number op1( stk.top(0), m_calc_precision);
						Number op2( stk.top(1), m_calc_precision);
						stk.pop( 2);
						stk.push( Number( op1 - op2).tostring());
						break;
					}
					case Concat:
					{
						std::string op1( stk.top(0));
						std::string op2( stk.top(1));
						stk.pop( 2);
						stk.push( op1 + op2);
						break;
					}
					default:
						throw std::logic_error("internal: unknown operator in expression");
				}
			break;
		}
	}
}

static bool checkNumber( const std::string& tok)
{
	int state = 0;
	std::string::const_iterator ii = tok.begin(), ee = tok.end();
	for (; ii != ee; ++ii)
	{
		if (*ii <= '9' && *ii >= '0') continue;
		if (*ii == '.')
		{
			if (state != 0) return false;
			state = 1;
			continue;
		}
		return false;
	}
	return true;
}

static bool isNumberToken( char ch)
{
	return (ch >= '0' && ch <= '9');
}

static bool checkIdentifier( const std::string& tok)
{
	std::string::const_iterator ii = tok.begin(), ee = tok.end();
	for (; ii != ee; ++ii)
	{
		if ((*ii|32) <= 'z' && (*ii|32) >= 'a') continue;
		if (*ii == '_') continue;
		if (*ii <= '9' && *ii >= '0' && ii != tok.begin()) continue;
		return false;
	}
	return true;
}

static bool isIdentifierToken( char ch)
{
	return (((ch|32) >= 'a' && (ch|32) <= 'z') || ch == '_');
}


static Expression parseOperand( std::string::const_iterator& itr, const std::string::const_iterator& end, std::string& exprstrings);
static Expression parseSumExpression( const Expression& op1, std::string::const_iterator& itr, const std::string::const_iterator& end, std::string& exprstrings);
static Expression parseFactorExpression( const Expression& op1, std::string::const_iterator& itr, const std::string::const_iterator& end, std::string& exprstrings);
static Expression parseAssignExpression( const Expression& op1, std::string::const_iterator& itr, const std::string::const_iterator& end, std::string& exprstrings);
static Expression parseAssignExpressionList( char separator, std::string::const_iterator& itr, const std::string::const_iterator& end, std::string& exprstrings);
static StateDef::MethodCall parseMethodCall( std::string::const_iterator& itr, const std::string::const_iterator& end, std::string& exprstrings);
static utils::CharTable g_operatorTable( "<>#(){}[]=+-*/,;?");

static bool parseOperator( Expression::Operator& opr, std::string::const_iterator& itr,  const std::string::const_iterator& end)
{
	char ch = utils::gotoNextToken( itr, end);
	switch (ch)
	{
		case '-': ++itr; opr = Expression::Minus; return true;
		case '+': ++itr; opr = Expression::Plus; return true;
		case '#': ++itr; opr = Expression::Concat; return true;
		case '*': ++itr; opr = Expression::Multiply; return true;
		case '/': ++itr; opr = Expression::Divide; return true;
		case ':': ++itr; opr = Expression::Round; return true;
		case '=': ++itr; opr = Expression::Assign; return true;
		case '?':
		{
			++itr;
			if (itr == end) throw std::runtime_error( "unexpected end of expression");
			if (*itr == '=') {++itr; opr = Expression::AssignUndefined; return true;}
			if (*itr == '<') {++itr; opr = Expression::AssignGreater; return true;}
			if (*itr == '>') {++itr; opr = Expression::AssignSmaller; return true;}
			throw std::runtime_error( "unexpected token in expression");
		}
	}
	return false;
}

static Expression parseOperand( std::string::const_iterator& itr, const std::string::const_iterator& end, std::string& exprstrings)
{
	Expression rt;
	Expression::Operator opr;

	if (parseOperator( opr, itr, end))
	{
		if (opr == Expression::Minus)
		{
			Expression subexpr = parseOperand( itr, end, exprstrings);
			if (!subexpr.size()) throw std::runtime_error( "unary operator without argument in expression");
			rt.push_expression( subexpr);
			rt.push_operator( Expression::UnaryMinus);
			return rt;
		}
		else if (opr == Expression::Plus)
		{
			Expression subexpr = parseOperand( itr, end, exprstrings);
			if (!subexpr.size()) throw std::runtime_error( "unary operator without argument in expression");
			rt.push_expression( subexpr);
			return rt;
		}
		throw std::runtime_error( "identifier, string, unary operator or number expected");
	}

	char ch = utils::gotoNextToken( itr, end);
	switch (ch)
	{
		case '\0': break;
		case ',':
		case ';':
		case '}':
		case ')': break;
		case '(':
		{
			++itr;
			Expression op1 = parseOperand( itr, end, exprstrings);
			if (!op1.size()) throw std::runtime_error( "empty subexpression");
			Expression subexpr = parseSumExpression( op1, itr, end, exprstrings);
			ch = utils::gotoNextToken( itr, end);
			if (ch != ')') throw std::runtime_error( "subexpression not terminated. ')' expected");
			rt.push_expression( subexpr);
			break;
		}
		case '\'':
		case '"':
		{
			std::string tok;
			ch = utils::parseNextToken( tok, itr, end, g_operatorTable);
			rt.push_value( exprstrings.size());
			exprstrings.append( tok);
			exprstrings.push_back( '\0');
			break;
		}
		default:
		{
			std::string tok;
			ch = utils::parseNextToken( tok, itr, end, g_operatorTable);
			if (isIdentifierToken(ch))
			{
				if (!checkIdentifier( tok)) throw std::runtime_error( std::string( "illegal identifier '") + tok + "'");
				rt.push_variable( variableId( tok));
			}
			else if (isNumberToken(ch))
			{
				if (!checkNumber( tok)) throw std::runtime_error( std::string( "illegal number '") + tok + "'");
				rt.push_value( exprstrings.size());
				exprstrings.append( tok);
				exprstrings.push_back( '\0');
			}
			else
			{
				throw std::runtime_error( std::string( "unexpected token '") + tok + "'");
			}
		}
	}
	return rt;
}

static Expression parseFactorExpression( const Expression& op1, std::string::const_iterator& itr, const std::string::const_iterator& end, std::string& exprstrings)
{
	Expression rt;
	std::string tok;
	Expression::Operator opr;

	if (parseOperator( opr, itr, end))
	{
		if (opr == Expression::Minus || opr == Expression::Plus || opr == Expression::Concat)
		{
			rt.push_expression( op1);
			return rt;
		}
		else if (opr == Expression::Multiply || opr == Expression::Divide || opr == Expression::Round)
		{
			Expression op2 = parseOperand( itr, end, exprstrings);
			if (!op2.size()) throw std::runtime_error( "second operand expected for binary minus '-'");
			rt.push_expression( op1);
			rt.push_expression( op2);
			rt.push_operator( opr);
			return rt;
		}
		throw std::runtime_error( "binary operator {'+','-','#','/','*',':'} expected");
	}

	char ch = utils::gotoNextToken( itr, end);
	switch (ch)
	{
		case '\0':
			break;
		case ',':
		case ';':
		case ')':
		case '}':
			rt.push_expression( op1);
			break;
		default:
			throw std::runtime_error( "end of expression or binary operator {'+','-','#','/','*',':'} expected");
	}
	return rt;
}

static Expression parseSumExpression( const Expression& op1, std::string::const_iterator& itr, const std::string::const_iterator& end, std::string& exprstrings)
{
	Expression rt;
	Expression::Operator opr;

	if (parseOperator( opr, itr, end))
	{
		if (opr == Expression::Minus || opr == Expression::Plus || opr == Expression::Concat)
		{
			Expression op2 = parseOperand( itr, end, exprstrings);
			if (!op2.size()) throw std::runtime_error( "second operand expected for binary minus '-'");
			rt.push_expression( op1);
			rt.push_expression( op2);
			rt.push_operator( opr);
			return rt;
		}
		else if (opr == Expression::Multiply || opr == Expression::Divide || opr == Expression::Round)
		{
			Expression op2 = parseFactorExpression( op1, itr, end, exprstrings);
			if (!op2.size()) throw std::runtime_error( "second operand expected for binary minus '-'");
			rt.push_expression( op1);
			rt.push_expression( op2);
			rt.push_operator( opr);
			return rt;
		}
		throw std::runtime_error( "binary operator {'+','-','#','/','*',':'} expected");
	}

	char ch = utils::gotoNextToken( itr, end);
	switch (ch)
	{
		case '\0':
			break;
		case ';':
		case ',':
		case ')':
		case '}':
			rt.push_expression( op1);
			break;
		default:
			throw std::runtime_error( "end of expression or binary operator {'+','-','#','/','*',':'} expected");
	}
	return rt;
}

static Expression parseAssignExpression( const Expression& op1, std::string::const_iterator& itr, const std::string::const_iterator& end, std::string& exprstrings)
{
	Expression rt;
	Expression::Operator opr;

	if (parseOperator( opr, itr, end))
	{
		if (opr == Expression::Assign || opr == Expression::AssignUndefined || opr == Expression::AssignGreater || opr == Expression::AssignSmaller)
		{
			Expression op2 = parseOperand( itr, end, exprstrings);
			if (!op2.size()) throw std::runtime_error( "something expected right of an assignment '='");
			op2 = parseSumExpression( op2, itr, end, exprstrings);
			if (!op2.size()) throw std::runtime_error( "expression expected right of an assignment '='");
			rt.push_expression( op2);
			rt.push_expression( op1);
			rt.push_operator( opr);
			return rt;
		}
	}
	throw std::runtime_error( "binary assignment operator '=' or '?=' expected");
}

static Expression parseAssignExpressionList( char separator, std::string::const_iterator& itr, const std::string::const_iterator& end, std::string& exprstrings)
{
	Expression rt;

	char ch = utils::gotoNextToken( itr, end);
	switch (ch)
	{
		case ')':
		case '}':
		case '\0':
			return rt;
	}
	for (;;)
	{
		std::string identifier;
		bool passToSibling = false;
		if (ch == '[')
		{
			if (separator == ',')
			{
				throw std::runtime_error( "unexpected token '['. shared variable definition not allowed in parameter list");
			}
			++itr;
			ch = utils::parseNextToken( identifier, itr, end, g_operatorTable);
			if (!isIdentifierToken(ch)) throw std::runtime_error( std::string( "variable name expected after '[' instead of '") + identifier + "'");
			char eb = utils::gotoNextToken( itr, end);
			if (eb == ']')
			{
				++itr;
				passToSibling = true;
			}
			else
			{
				throw std::runtime_error( std::string( "']' expected after variable definition for shared variable in tag context instead of '") + eb + "'");
			}
		}
		else
		{
			ch = utils::parseNextToken( identifier, itr, end, g_operatorTable);
		}
		if (isIdentifierToken(ch))
		{
			Expression op1;
			if (!checkIdentifier( identifier)) throw std::runtime_error( std::string( "illegal identifier '") + identifier + "'");
			if (passToSibling)
			{
				op1.push_tagvariable( variableId( identifier));
			}
			else
			{
				op1.push_variable( variableId( identifier));
			}
			rt.push_expression( parseAssignExpression( op1, itr, end, exprstrings));
			ch = utils::gotoNextToken( itr, end);
			if (ch == separator)
			{
				++itr;
				ch = utils::gotoNextToken( itr, end);
				continue;
			}
			switch (ch)
			{
				case ')':
				case '}':
				case '\0':
					return rt;
			}
			throw std::runtime_error( std::string( "separator or end of assignment list expected; unexpected token '") + ch + "'");
		}
		else
		{
			throw std::runtime_error( std::string( "identifier or identifier in [ ] brackets expected instead of '") + ch + "'");
		}
	}
}

static StateDef::MethodCall parseMethodCall( std::string::const_iterator& itr, const std::string::const_iterator& end, std::string& exprstrings)
{
	StateDef::MethodCall rt;
	std::string tok;

	char ch = utils::parseNextToken( tok, itr, end, g_operatorTable);
	if (!isIdentifierToken(ch)) throw std::runtime_error( std::string( "expected method identifier instead of ' ") + tok + "'");

	rt.m_method = methodId( tok);
	ch = utils::gotoNextToken( itr, end);
	if (ch == '(')
	{
		++itr;
		rt.m_param.push_expression( parseAssignExpressionList( ',', itr, end, exprstrings));
		ch = utils::gotoNextToken( itr, end);
		if (ch != ')') throw std::runtime_error( "expected ')' at end of method parameter list");
		++itr;
		ch = utils::gotoNextToken( itr, end);
		if (!ch)
		{}
		else if (ch == ';')
		{
			++itr;
		}
		else
		{
			throw std::runtime_error( "';' or end of line expected after method call");
		}
	}
	else
	{
		if (ch != ';' && ch != '\0') throw std::runtime_error( std::string( "';' or end of line expected instead of '") + tok + "'");
	}
	return rt;
}

void StateDef::parse( std::string::const_iterator itr, const std::string::const_iterator& end, std::string& exprstrings)
{
	std::string tok;
	Expression expr;

	char ch = utils::gotoNextToken( itr, end);
	if (ch)
	{
		if (ch == '{')
		{
			++itr;
			m_expr.push_expression( parseAssignExpressionList( ';', itr, end, exprstrings));
			ch = utils::gotoNextToken( itr, end);
			if (ch != '}') throw std::runtime_error( "expected '}' at end of assignment list");
			++itr;
		}
	}
	while (utils::gotoNextToken( itr, end))
	{
		m_call.push_back( parseMethodCall( itr, end, exprstrings));
	}
}

std::string Expression::tostring( const std::string& exprstrings) const
{
	std::ostringstream out;
	Variable::Id var;
	std::vector<Item>::const_iterator itr=m_ar.begin(), end=m_ar.end();
	for (; itr != end; ++itr)
	{
		switch (itr->m_type)
		{
			case Item::TagVariable:
				var = (Variable::Id)itr->value.m_idx;
				out << " [" << variableName( var) << "]";
				break;

			case Item::Variable:
				var = (Variable::Id)itr->value.m_idx;
				out << " " << variableName( var);
				break;

			case Item::Value:
				out << " '" << std::string( exprstrings.c_str() + itr->value.m_idx) << "'";
				break;

			case Item::Operator:
				out << " " << Expression::operatorString( itr->value.m_op);
				break;
			break;
		}
	}
	return out.str();
}

std::string StateDef::MethodCall::tostring( const std::string& exprstrings) const
{
	std::ostringstream out;
	out << methodName( m_method) << "(" << m_param.tostring( exprstrings) << " )";
	return out.str();
}

std::string StateDef::tostring( const std::string& exprstrings) const
{
	std::ostringstream out;
	std::vector<MethodCall>::const_iterator ii = m_call.begin(), ee = m_call.end();
	out << "{" << m_expr.tostring( exprstrings) << " }";
	for (; ii != ee; ++ii)
	{
		out << " " << ii->tostring( exprstrings) << ";";
	}
	return out.str();
}

