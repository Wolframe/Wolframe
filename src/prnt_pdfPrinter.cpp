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
///\file prnt_pdfPrinter.hpp
///\brief Implements the prnt::PrintingInterface base on libhpdf and some form definition
#include "prnt/pdfPrinter.hpp"
#include "utils/miscUtils.hpp"
#include "textwolf/xmlpathautomatonparse.hpp"
#include <cstdlib>
#include <vector>
#include <map>
#include <boost/lexical_cast.hpp>

using namespace _Wolframe;
using namespace _Wolframe::prnt;

enum Variable
{
	Position,
	SizeX,
	SizeY
};
static const char* variableName( Variable v)
{
	static const char* ar[] = {
		"Position",
		"SizeX",
		"SizeY",
		0};
	return ar[ (int)v];
}
struct VarnameMap :public std::map <std::string, std::size_t>
{
	VarnameMap()
	{
		for (std::size_t ii=0; variableName( (Variable)ii); ++ii)
		{
			(*this)[ variableName( (Variable)ii)] = ii;
		}
	}
};

enum Method
{
	Nop,
	PrintText
};
static const char* methodName( Method m)
{
	static const char* ar[] = {
		"Nop",
		"PrintText",
		0};
	return ar[ (int)m];
}
struct MethodnameMap :public std::map <std::string, std::size_t>
{
	MethodnameMap()
	{
		for (std::size_t ii=0; methodName( (Method)ii); ++ii)
		{
			(*this)[ methodName( (Method)ii)] = ii;
		}
	}
};

static VarnameMap g_varnamemap;
static MethodnameMap g_methodnamemap;


class Expression
{
public:
	void push_operator( char chr)
	{
		Item ee;
		ee.m_type = Item::Operator;
		ee.value.m_opchr = chr;
		m_ar.push_back( ee);
	}

	void push_value( std::size_t idx)
	{
		Item ee;
		ee.m_type = Item::Value;
		ee.value.m_idx = idx;
		m_ar.push_back( ee);
	}

	void push_variable( std::size_t idx)
	{
		Item ee;
		ee.m_type = Item::Variable;
		ee.value.m_idx = idx;
		m_ar.push_back( ee);
	}

	void push_expression( const Expression& expr)
	{
		m_ar.insert( m_ar.end(), expr.m_ar.begin(), expr.m_ar.end());
	}

	std::size_t size() const
	{
		return m_ar.size();
	}

private:
	struct Item
	{
		enum Type {Variable,Value,Operator};
		Type m_type;
		union
		{
			std::size_t m_idx;
			char m_opchr;
		} value;
	};

private:
	std::vector<Item> m_ar;
};

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
				VarnameMap::const_iterator vi = g_varnamemap.find( tok);
				if (vi == g_varnamemap.end()) throw std::runtime_error( std::string( "unknown variable name '") + tok + "'");
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
			rt.push_expression( op1);
			rt.push_expression( op2);
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
			VarnameMap::const_iterator vi = g_varnamemap.find( tok);
			if (vi == g_varnamemap.end()) throw std::runtime_error( std::string( "value assigned to unknown variable '") + tok + "'");
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

struct StateDef
{
	StateDef()
		:m_method(Nop){}

	StateDef( const StateDef& o)
		:m_method(o.m_method)
		,m_param_expr(o.m_param_expr)
		,m_local_expr(o.m_local_expr){}

	Method m_method;
	Expression m_param_expr;
	Expression m_local_expr;
};

static StateDef parseStateDef( std::string::const_iterator itr, const std::string::const_iterator& end, std::string& exprstrings)
{
	StateDef rt;
	std::string tok;
	std::string::const_iterator prev;
	Expression expr;
	char ch;

	prev = itr; ch = parseNextToken( tok, itr, end, g_operatorTable);
	if (ch == '{')
	{
		rt.m_local_expr.push_expression( parseAssignExpressionList( ';', itr, end, exprstrings));
		ch = parseNextToken( tok, itr, end, g_operatorTable);
		if (ch != '}') throw std::runtime_error( "expected '}' at end of assignment list");
	}
	ch = parseNextToken( tok, itr, end, g_operatorTable);
	if (ch == '\0') return rt;
	if (!isIdentifierToken(ch)) throw std::runtime_error( std::string( "expected method identifier instead of ' ") + tok + "' after assignment list");

	MethodnameMap::const_iterator vi = g_methodnamemap.find( tok);
	if (vi == g_methodnamemap.end()) throw std::runtime_error( std::string( "unknown method called '") + tok + "'");
	rt.m_method = (Method)vi->second;
	ch = parseNextToken( tok, itr, end, g_operatorTable);
	if (ch == '(')
	{
		rt.m_param_expr.push_expression( parseAssignExpressionList( ',', itr, end, exprstrings));
		ch = parseNextToken( tok, itr, end, g_operatorTable);
		if (ch != ')') throw std::runtime_error( "expected ')' at end of named parameter list");
	}
	if (ch == '\0') return rt;
	throw std::runtime_error( std::string( "end of line expected instead of '") + tok + "'");
}


struct VariableValue
{
	enum Type {Number, String};
	Type m_type;
	union
	{
		double m_number;
		std::size_t m_string;
	} value;

	static VariableValue string( std::size_t idx)
	{
		VariableValue rt;
		rt.m_type = String;
		rt.value.m_string = idx;
		return rt;
	}

	static VariableValue number( double value)
	{
		VariableValue rt;
		rt.m_type = Number;
		rt.value.m_number = value;
		return rt;
	}
};

class VariableScope
{
public:
	VariableScope()
	{
		m_strings.push_back('\0');
		m_valuear.push_back( VariableValue::string(0));
		m_ar.push_back( Map());
	}

	void push()
	{
		m_ar.push_back( Map());
	}

	void pop()
	{
		m_ar.pop_back();
		if (m_ar.empty()) throw std::logic_error( "non existing variable scope closed");
	}

	void define( Variable var, const std::string& value)
	{
		if (m_ar.back().find( var) != m_ar.back().end()) throw std::runtime_error( std::string( "duplicate definition of variable '") + variableName(var) + "'");
		m_valuear.push_back( VariableValue::string( m_strings.size()));
		m_strings.append( value);
		m_strings.push_back('\0');
		m_ar.back()[var] = m_valuear.size()-1;
	}

	void define( Variable var, double value)
	{
		if (m_ar.back().find( var) != m_ar.back().end()) throw std::runtime_error( std::string( "duplicate definition of variable '") + variableName(var) + "'");
		m_valuear.push_back( VariableValue::number( value));
		m_ar.back()[var] = m_valuear.size()-1;
	}

	std::size_t getValueIdx( Variable var)
	{
		std::vector<Map>::const_iterator itr = m_ar.begin()+m_ar.size();
		while (itr != m_ar.begin())
		{
			--itr;
			Map::const_iterator vi = m_ar.back().find( var);
			if (vi != m_ar.back().end())
			{
				return vi->second;
			}
		}
		return 0;
	}

	bool getString( Variable var, std::string& value)
	{
		std::size_t validx = getValueIdx( var);
		if (!validx) return false;
		const VariableValue& val = m_valuear[ validx];
		switch (val.m_type)
		{
			case VariableValue::Number:
				value = boost::lexical_cast<std::string>( val.value.m_number);
				return true;

			case VariableValue::String:
				value = m_strings.c_str() + val.value.m_string;
				return true;
		}
		throw std::logic_error("illegal state");
	}

	bool getNumber( Variable var, double& value)
	{
		std::size_t validx = getValueIdx( var);
		if (!validx) return false;
		const VariableValue& val = m_valuear[ validx];
		switch (val.m_type)
		{
			case VariableValue::Number:
				value = val.value.m_number;
				return true;

			case VariableValue::String:
				std::string strval = m_strings.c_str() + val.value.m_string;
				value = boost::lexical_cast<double>( strval);
				return true;
		}
		throw std::logic_error("illegal state");
	}

private:
	typedef std::map<std::size_t,std::size_t> Map;
	std::vector<Map> m_ar;
	std::vector<VariableValue> m_valuear;
	std::string m_strings;
};

static std::string getLine( std::size_t& linecnt, std::string::const_iterator& itr, const std::string::const_iterator& end)
{
	std::string rt;
	++linecnt;
	for (; itr != end && *itr != '\n'; ++itr)
	{
		if (*itr == '\\')
		{
			++itr;
			if (itr == end) throw std::runtime_error( "found '\\' at end of input");
			if (*itr != '\n')
			{
				rt.push_back( '\\');
				rt.push_back( *itr);
				std::string::const_iterator next = itr;
				for (;next != end && *next != '\n' && isSpace(*next); ++next);
				if (next != end && *next == '\n') std::runtime_error( "found spaces after a '\\' at end of a line");
			}
			else
			{
				++linecnt;
			}
		}
		else
		{
			rt.push_back( *itr);
		}
	}
	return rt;
}

static std::string getSelectionExpression( std::string::const_iterator& itr, const std::string::const_iterator& end)
{
	std::string::const_iterator eb = itr;
	for (; itr != end; ++itr)
	{
		if (*itr == ':')
		{
			std::string::const_iterator ee = itr;
			++itr;
			if (itr != end && isSpace(*itr))
			{
				++itr;
				return std::string( eb, ee);
			}
		}
	}
	return std::string( eb, end);
}

struct PdfPrinter::PdfPrinterImpl
{
public:
	PdfPrinterImpl()
	{
		m_exprstrings.push_back( '\0');
	}

	void parseDescription( const std::string& src)
	{
		std::size_t linecnt = 0;
		std::string::const_iterator itr = src.begin(), end=src.end();
		try
		{
			std::string line = getLine( linecnt, itr, end);
			std::string::const_iterator li=line.begin(), le=line.end();
			std::string xpathstr = getSelectionExpression( li, le);
			int xerr = m_parser.addExpression( (int)m_statedef.size()+1, xpathstr.c_str(), xpathstr.size());
			if (xerr != 0)
			{
				std::runtime_error( std::string( "in xml selection path at position ") + boost::lexical_cast<std::string>(xerr));
			}
			StateDef stateDef = parseStateDef( li, le, m_exprstrings);
			m_statedef.push_back( stateDef);
		}
		catch (const std::runtime_error& e)
		{
			throw std::runtime_error( std::string( "error on line ") + boost::lexical_cast<std::string>(linecnt) + " of simple PDF printer description source (" + e.what() + ")");
		}
	}

private:
	friend class PdfPrinter;
	typedef textwolf::XMLPathSelectAutomatonParser<textwolf::charset::UTF8,textwolf::charset::UTF8> XMLPathSelectParser;
	XMLPathSelectParser m_parser;
	std::vector<StateDef> m_statedef;
	std::string m_exprstrings;

};

PdfPrinter::PdfPrinter( const std::string& name_, const std::string& description)
	:PrintingInterface(name_)
	,m_impl(new PdfPrinterImpl())
{
	m_impl->parseDescription( description);
}

std::string PdfPrinter::print( const langbind::TypedInputFilterR& ) const
{
	return std::string();
}

PrintingInterfaceR _Wolframe::prnt::createPdfPrinter( const std::string& description)
{
	PdfPrinter* ref = new PdfPrinter( "simplepdf", description);
	PrintingInterfaceR rt( ref);
	return rt;
}



