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
///\file prnt_pdfPrinterExpression.cpp
#include "prnt/pdfPrinterExpression.hpp"
#include "prnt/pdfPrinterVariable.hpp"
#include "utils/miscUtils.hpp"
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::prnt;

void Expression::push_operator( char chr)
{
	Item ee;
	ee.m_type = Item::Operator;
	ee.value.m_opchr = chr;
	m_ar.push_back( ee);
}

void Expression::push_value( std::size_t idx)
{
	Item ee;
	ee.m_type = Item::Value;
	ee.value.m_idx = idx;
	m_ar.push_back( ee);
}

void Expression::push_variable( std::size_t idx)
{
	Item ee;
	ee.m_type = Item::Variable;
	ee.value.m_idx = idx;
	m_ar.push_back( ee);
}

void Expression::push_expression( const Expression& expr)
{
	m_ar.insert( m_ar.end(), expr.m_ar.begin(), expr.m_ar.end());
}

#if 0
void Expression::evaluate_expression( VariableScope& vscope, const std::string& exprstrings) const
{
	ValueStack stk;
	Variable assignvar;

	std::vector<Item>::const_iterator itr=m_ar.begin(), end=m_ar.end();
	for (; itr != end; ++itr)
	{
		switch (itr->m_type)
		{
			case Variable:
			case Value:
				stkelem.item = *itr;
				stk.push_back( stkelem);
			break;
			case Operator:
				stkelem.item = *itr;
				switch (itr->m_opchr)
				{
					case  '=':
						if (stk.size() < 2) throw std::logic_error("internal: binary operator '=' with too few operands");
						StackElem* dest = &stk[ stk.size()-2];
						StackElem* src = &stk[ stk.size()-1];
						if (dest->m_type != Variable) throw std::logic_error("internal: assignment to non variable");
						if (src->m_type == Variable)
						{
						}
					break;
					case  '~':
						if (stk.size() < 1) throw std::logic_error("internal: unary operator '-' without operand");
					break;
					case  '*':
						if (stk.size() < 2) throw std::logic_error("internal: binary operator '=' with too few operands");
					break;
					case  '/':
						if (stk.size() < 2) throw std::logic_error("internal: binary operator '=' with too few operands");
					break;
					case  '+':
						if (stk.size() < 2) throw std::logic_error("internal: binary operator '=' with too few operands");
					break;
					case  '-':
						if (stk.size() < 2) throw std::logic_error("internal: binary operator '=' with too few operands");
					break;
					default: throw std::logic_error("internal: unknown operator in expression");
				}
				stk.push_back( stkelem);
			break;
		}
	}
}
#endif

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

static bool isSpace( char ch)
{
	return ((unsigned char)ch <= (unsigned char)' ');
}

static Expression parseOperand( std::string::const_iterator itr, const std::string::const_iterator& end, std::string& exprstrings);
static Expression parseSumExpression( const Expression& op1, std::string::const_iterator itr, const std::string::const_iterator& end, std::string& exprstrings);
static Expression parseFactorExpression( const Expression& op1, std::string::const_iterator itr, const std::string::const_iterator& end, std::string& exprstrings);
static Expression parseAssignExpression( const Expression& op1, std::string::const_iterator itr, const std::string::const_iterator& end, std::string& exprstrings);
static Expression parseAssignExpressionList( char separator, std::string::const_iterator itr, const std::string::const_iterator& end, std::string& exprstrings);
static StateDef::MethodCall parseMethodCall( std::string::const_iterator itr, const std::string::const_iterator& end, std::string& exprstrings);
static utils::OperatorTable g_operatorTable( "(){}=+-*/,;");

static Expression parseOperand( std::string::const_iterator itr, const std::string::const_iterator& end, std::string& exprstrings)
{
	Expression rt;
	std::string tok;
	std::string::const_iterator prev;
	char ch;

	prev = itr; ch = parseNextToken( tok, itr, end, g_operatorTable);
	switch (ch)
	{
		case '\0': break;
		case ',':
		case ';':
		case '}':
		case ')': itr = prev; break;
		case '-':
		{
			Expression subexpr = parseOperand( itr, end, exprstrings);
			if (!subexpr.size()) throw std::runtime_error( "unary operator without argument in expression");
			rt.push_expression( subexpr);
			rt.push_operator( '~');
			break;
		}
		case '+':
		{
			Expression subexpr = parseOperand( itr, end, exprstrings);
			if (!subexpr.size()) throw std::runtime_error( "unary operator without argument in expression");
			rt.push_expression( subexpr);
			break;
		}
		case '(':
		{
			Expression op1 = parseOperand( itr, end, exprstrings);
			if (!op1.size()) throw std::runtime_error( "empty subexpression");
			Expression subexpr = parseSumExpression( op1, itr, end, exprstrings);
			prev = itr; ch = parseNextToken( tok, itr, end, g_operatorTable);
			if (ch != ')') throw std::runtime_error( "subexpression not terminated. ')' expected");
			rt.push_expression( subexpr);
			break;
		}
		case '=':
		case '/':
		case '*':
			throw std::runtime_error( "identifier string or number expected");
		case '\'':
		case '"':
			rt.push_value( exprstrings.size());
			exprstrings.append( tok);
			exprstrings.push_back( '\0');
			break;
		default:
			if (isIdentifierToken(ch))
			{
				if (!checkIdentifier( tok)) throw std::runtime_error( std::string( "illegal identifier '") + tok + "'");
				std::map <std::string, std::size_t>* varnamemap = getVariablenameMap();
				std::map <std::string,std::size_t>::const_iterator vi = varnamemap->find( tok);
				if (vi == varnamemap->end()) throw std::runtime_error( std::string( "unknown variable name '") + tok + "'");
				rt.push_variable( vi->second);
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
	return rt;
}

static Expression parseFactorExpression( const Expression& op1, std::string::const_iterator itr, const std::string::const_iterator& end, std::string& exprstrings)
{
	Expression rt;
	std::string tok;
	std::string::const_iterator prev;
	char ch;

	prev = itr; ch = parseNextToken( tok, itr, end, g_operatorTable);
	switch (ch)
	{
		case '\0':
			break;
		case ',':
		case ';':
		case ')':
		case '}':
		case '+':
		case '-':
			itr = prev;
			rt.push_expression( op1);
			break;
		case '/':
		case '*':
		{
			Expression op2 = parseOperand( itr, end, exprstrings);
			if (!op2.size()) throw std::runtime_error( "second operand expected for binary minus '-'");
			rt.push_expression( op1);
			rt.push_expression( op2);
			rt.push_operator( ch);
			break;
		}
		default:
		{
			throw std::runtime_error( "binary operator {'+','-','/','*'} expected");
		}
	}
	return rt;
}

static Expression parseSumExpression( const Expression& op1, std::string::const_iterator itr, const std::string::const_iterator& end, std::string& exprstrings)
{
	Expression rt;
	std::string tok;
	std::string::const_iterator prev;
	char ch;

	prev = itr; ch = parseNextToken( tok, itr, end, g_operatorTable);
	switch (ch)
	{
		case '\0':
			break;
		case ';':
		case ',':
		case ')':
		case '}':
			itr = prev;
			rt.push_expression( op1);
			break;
		case '-':
		case '+':
		{
			Expression op2 = parseOperand( itr, end, exprstrings);
			if (!op2.size()) throw std::runtime_error( "second operand expected for binary minus '-'");
			rt.push_expression( op1);
			rt.push_expression( op2);
			rt.push_operator( ch);
			break;
		}
		case '*':
		case '/':
		{
			Expression op2 = parseFactorExpression( op1, itr, end, exprstrings);
			if (!op2.size()) throw std::runtime_error( "second operand expected for binary minus '-'");
			rt.push_expression( op1);
			rt.push_expression( op2);
			rt.push_operator( ch);
			break;
		}
		default:
		{
			throw std::runtime_error( "binary operator {'+','-','/','*'} expected");
		}
	}
	return rt;
}

static Expression parseAssignExpression( const Expression& op1, std::string::const_iterator itr, const std::string::const_iterator& end, std::string& exprstrings)
{
	Expression rt;
	std::string tok;
	std::string::const_iterator prev;
	char ch;

	prev = itr; ch = parseNextToken( tok, itr, end, g_operatorTable);
	switch (ch)
	{
		case '=':
		{
			Expression op2 = parseOperand( itr, end, exprstrings);
			if (!op2.size()) throw std::runtime_error( "something expected right of an assignment '='");
			op2 = parseSumExpression( op2, itr, end, exprstrings);
			if (!op2.size()) throw std::runtime_error( "second operand expected for binary minus '-'");
			rt.push_expression( op2);
			rt.push_expression( op1);
			rt.push_operator( ch);
			break;
		}
		default:
		{
			throw std::runtime_error( "binary assignment operator '=' expected");
		}
	}
	return rt;
}

static Expression parseAssignExpressionList( char separator, std::string::const_iterator itr, const std::string::const_iterator& end, std::string& exprstrings)
{
	Expression rt;
	std::string tok;
	std::string::const_iterator prev;
	char ch;

	prev = itr; ch = parseNextToken( tok, itr, end, g_operatorTable);
	switch (ch)
	{
		case '\0':
			break;
		case ')':
		case '}':
			itr = prev;
			break;
	}
	for (;;)
	{
		if (ch == '\0' || ch == ')' || ch ) return rt;
		if (isIdentifierToken(ch))
		{
			Expression op1;
			if (!checkIdentifier( tok)) throw std::runtime_error( std::string( "illegal identifier '") + tok + "'");
			std::map <std::string, std::size_t>* varnamemap = getVariablenameMap();
			std::map <std::string,std::size_t>::const_iterator vi = varnamemap->find( tok);
			if (vi == varnamemap->end()) throw std::runtime_error( std::string( "value assigned to unknown variable '") + tok + "'");
			op1.push_variable( vi->second);
			rt.push_expression( parseAssignExpression( op1, itr, end, exprstrings));
			prev = itr; ch = parseNextToken( tok, itr, end, g_operatorTable);
			if (ch == separator) continue;
			switch (ch)
			{
				case '\0':
					return rt;
				case ')':
				case '}':
					itr = prev;
					return rt;
			}
			throw std::runtime_error( std::string( "separator or end of assignment list expected; unexpected token '") + tok + "'");
		}
		else
		{
			throw std::runtime_error( std::string( "identifier expected; unexpected token '") + tok + "'");
		}
	}
}

static StateDef::MethodCall parseMethodCall( std::string::const_iterator itr, const std::string::const_iterator& end, std::string& exprstrings)
{
	StateDef::MethodCall rt;
	std::string tok;
	std::string::const_iterator prev;
	char ch;

	ch = parseNextToken( tok, itr, end, g_operatorTable);
	if (!isIdentifierToken(ch)) throw std::runtime_error( std::string( "expected method identifier instead of ' ") + tok + "'");

	std::map <std::string, std::size_t>* methodnamemap = _Wolframe::prnt::getMethodnameMap();
	std::map <std::string, std::size_t>::const_iterator vi = methodnamemap->find( tok);
	if (vi == methodnamemap->end()) throw std::runtime_error( std::string( "unknown method called '") + tok + "'");
	rt.m_method = (Method)vi->second;
	ch = parseNextToken( tok, itr, end, g_operatorTable);
	if (ch == '(')
	{
		rt.m_param.push_expression( parseAssignExpressionList( ',', itr, end, exprstrings));
		ch = parseNextToken( tok, itr, end, g_operatorTable);
		if (ch != ')') throw std::runtime_error( "expected ')' at end of method parameter list");
		while (itr != end && isSpace( *itr)) ++itr;
		if (itr == end)
		{}
		else if (*itr == ';')
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
	std::string::const_iterator prev;
	Expression expr;
	char ch;

	while (itr != end && isSpace( *itr)) ++itr;
	if (itr != end)
	{
		if (*itr == '{')
		{
			m_expr.push_expression( parseAssignExpressionList( ';', itr, end, exprstrings));
			ch = parseNextToken( tok, itr, end, g_operatorTable);
			if (ch != '}') throw std::runtime_error( "expected '}' at end of assignment list");
		}
		while (itr != end && isSpace( *itr)) ++itr;
	}
	while (itr != end)
	{
		m_call.push_back( parseMethodCall( itr, end, exprstrings));
		while (itr != end && isSpace( *itr)) ++itr;
	}
}



