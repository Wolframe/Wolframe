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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\brief Loading function for transaction definition programs
///\file loadTransactionProgram.cpp
#include "database/loadTransactionProgram.hpp"
#include "transactionFunctionDescription.hpp"
#include "utils/parseUtils.hpp"
#include "utils/fileUtils.hpp"
#include "logger-v1.hpp"
#include "config/programBase.hpp"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;

static bool isLineStart( std::string::const_iterator si, const std::string& src)
{
	if (si == src.begin()) return true;
	for (--si; si >= src.begin() && *si <= ' ' && *si > '\0'; --si)
	{
		if (*si == '\n') return true;
	}
	return si == src.begin();
}

static std::string::const_iterator lineStart( std::string::const_iterator si, const std::string& src)
{
	if (si == src.begin()) return si;
	for (--si; si >= src.begin() && *si <= ' ' && *si > '\0'; --si)
	{
		if (*si == '\n') return si+1;
	}
	throw std::logic_error( "internal: Called lineStart without calling isLineStart before");
}

static std::size_t lineCount( std::string::const_iterator si, std::string::const_iterator se)
{
	std::size_t rt = 0;
	for (; si != se; ++si)
	{
		if (*si == '\n') ++rt;
	}
	return rt;
}

static const utils::CharTable g_optab( ";:-,.=)(<>[]{}/&%*|+-#?!$");

static bool isAlphaNumeric( char ch)
{
	if (ch >= '0' && ch <= '9') return true;
	if (ch >= 'A' && ch <= 'Z') return true;
	if (ch >= 'a' && ch <= 'z') return true;
	if (ch == '_') return true;
	return false;
}

static char gotoNextToken( const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator se)
{
	const char* commentopr = langdescr->eoln_commentopr();
	char ch;
	while ((ch = utils::gotoNextToken( si, se)) != 0)
	{
		if (ch == commentopr[0])
		{
			std::string::const_iterator ti = si+1;
			std::size_t ci = 1;
			while (commentopr[ci] && ti != se && commentopr[ci] == *ti)
			{
				++ci;
				++ti;
			}
			if (!commentopr[ci])
			{
				// skip to end of line
				while (ti != se && *ti != '\n') ++ti;
				si = ti;
				continue;
			}
		}
		break;
	}
	return ch;
}

static char parseNextToken( const LanguageDescription* langdescr, std::string& tok, std::string::const_iterator& si, std::string::const_iterator se)
{
	char ch = gotoNextToken( langdescr, si, se);
	if (!ch) return 0;
	return utils::parseNextToken( tok, si, se, g_optab);
}

static std::vector<std::string> parse_INTO_path( const LanguageDescription* langdescr, std::string::const_iterator& si, std::string::const_iterator se)
{
	std::vector<std::string> rt;
	for (;;)
	{
		std::string output;
		char ch = parseNextToken( langdescr, output, si, se);
		if (!ch) throw std::runtime_error( "unexpected end of description. result tag path expected after INTO");
		if (ch == '.' && output.empty()) output.push_back(ch);

		if (output.empty())
		{
			throw std::runtime_error( "identifier or '.' expected after INTO");
		}
		rt.push_back( output);
		ch = gotoNextToken( langdescr, si, se);
		if (ch != '/') break;
		++si;
	}
	return rt;
}

static TransactionFunctionDescription::MainProcessingStep::Call::Param
	parseReferenceParameter( const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	typedef TransactionFunctionDescription::MainProcessingStep::Call Call;
	char ch = utils::gotoNextToken( si, se);
	if (ch == '(' || ch == '[')
	{
		char sb,eb;
		Call::Param::Type type;
		if (ch == '(')
		{
			sb = '('; eb = ')';
			type = Call::Param::InputSelectorPath;
		}
		else
		{
			sb = '['; eb = ']';
			type = Call::Param::VariableReference;
		}
		++si;
		std::string::const_iterator argstart = si;
		std::string tok;
		ch = parseNextToken( langdescr, tok, si, se);
		for (; ch && ch != sb && ch != eb; ch=parseNextToken( langdescr, tok, si, se));
		if (ch == eb)
		{
			Call::Param param( type, boost::algorithm::trim_copy( std::string( argstart, si-1)));
			return param;
		}
		else
		{
			throw std::runtime_error( std::string( "missing close bracket for expression") + ((eb==')')?"')'":"']'"));
		}
	}
	else if (ch >= '0' && ch <= '9')
	{
		std::string::const_iterator argstart = si;
		for (; si!=se && *si>= '0' && *si<= '9'; ++si);
		Call::Param::Type type = Call::Param::NumericResultReference;
		Call::Param param( type, boost::algorithm::trim_copy( std::string( argstart, si)));
		return param;
	}
	else if (isAlphaNumeric(ch))
	{
		std::string::const_iterator argstart = si;
		for (; si!=se && isAlphaNumeric(*si); ++si);
		Call::Param::Type type = Call::Param::SymbolicResultReference;
		Call::Param param( type, boost::algorithm::trim_copy( std::string( argstart, si)));
		return param;
	}
	else
	{
		throw std::runtime_error( "expected open bracket '(' '[' or digit");
	}
}

static TransactionFunctionDescription::MainProcessingStep::Call::Param
	parsePathParameter( const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	typedef TransactionFunctionDescription::MainProcessingStep::Call Call;
	std::string pp;
	std::string tok;
	char ch;
	for (;;)
	{
		ch = gotoNextToken( langdescr, si, se);
		if (ch == ',' || ch == ')') break;
		if (!ch) throw std::runtime_error( "unexpected end of function call");

		ch = parseNextToken( langdescr, tok, si, se);

		if (ch == '/' || ch == '*' || ch == '.')
		{
			pp.push_back( ch);
		}
		else if (isAlphaNumeric( ch))
		{
			pp.append( tok);
		}
		else
		{
			throw std::runtime_error( "illegal token in path parameter of function call");
		}
	}
	Call::Param param( Call::Param::InputSelectorPath, pp);
	return param;
}

static TransactionFunctionDescription::MainProcessingStep::Call
	parseEmbeddedStatement( const LanguageDescription* langdescr, const std::string& funcname, int index, std::string::const_iterator& osi, std::string::const_iterator ose, types::keymap<std::string>& embeddedStatementMap)
{
	typedef TransactionFunctionDescription::MainProcessingStep::Call Call;
	Call rt;
	std::string stm;
	std::string dbstm = langdescr->parseEmbeddedStatement( osi, ose);
	std::string::const_iterator start = dbstm.begin(), si = dbstm.begin(), se = dbstm.end();
	std::string tok;

	char ch = parseNextToken( langdescr, tok, si, se);
	for (; si != se && ch; ch = parseNextToken( langdescr, tok, si, se))
	{
		if (ch == '$' && si != se)
		{
			if (*si == '(' || *si == '[' || isAlphaNumeric(*si))
			{
				stm.append( start, si - 1);
				Call::Param param = parseReferenceParameter( langdescr, si, se);
				rt.paramlist.push_back( param);
				start = si;
				stm.append( langdescr->stm_argument_reference( rt.paramlist.size()));
			}
		}
	}
	stm.append( start, si);

	rt.funcname.append( "__");
	rt.funcname.append( funcname);
	rt.funcname.append( "_");
	rt.funcname.append( boost::lexical_cast<std::string>( index));
	embeddedStatementMap.insert( rt.funcname, stm);
	return rt;
}

static TransactionFunctionDescription::MainProcessingStep::Call
	parseCallStatement( const LanguageDescription* langdescr, std::string::const_iterator& ci, std::string::const_iterator ce)
{
	typedef TransactionFunctionDescription::MainProcessingStep::Call Call;
	Call rt;
	std::string tok;

	if (!utils::gotoNextToken( ci, ce))
	{
		throw std::runtime_error( "unexpected end of transaction description. Function call expected");
	}
	while (ci < ce && isAlphaNumeric( *ci))
	{
		rt.funcname.push_back( *ci);
		++ci;
	}
	if (rt.funcname.empty())
	{
		throw std::runtime_error( "identifier expected for name of function");
	}
	utils::gotoNextToken( ci, ce);
	if (*ci != '(')
	{
		throw std::runtime_error( "'(' expected after function name");
	}
	++ci; utils::gotoNextToken( ci, ce);

	// Parse parameter list:
	if (*ci == ')')
	{
		// ... empty parameter list
		++ci;
	}
	else
	{
		std::string pp;
		char ch = ',';
		while (ch == ',')
		{
			ch = gotoNextToken( langdescr, ci, ce);
			if (ch == '\'' || ch == '\"')
			{
				ch = parseNextToken( langdescr, tok, ci, ce);
				Call::Param::Type type = Call::Param::Constant;
				Call::Param param( type, tok);
				rt.paramlist.push_back( param);
			}
			else if (ch == '$')
			{
				++ci;
				Call::Param param = parseReferenceParameter( langdescr, ci, ce);
				rt.paramlist.push_back( param);
			}
			else
			{
				Call::Param param = parsePathParameter( langdescr, ci, ce);
				rt.paramlist.push_back( param);
			}
			ch = parseNextToken( langdescr, tok, ci, ce);
		}
		if (ch != ')')
		{
			throw std::runtime_error( "unexpected token in function call parameter. close bracket ')' or comma ',' expected after argument");
		}
	}
	return rt;
}

static TransactionFunctionDescription::VariableValue
	parseVariableValue( const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se, int scope_functionidx, const TransactionFunctionDescription::VariableTable& varmap)
{
	typedef TransactionFunctionDescription::MainProcessingStep::Call Call;
	typedef TransactionFunctionDescription::VariableTable VariableTable;
	typedef TransactionFunctionDescription::VariableValue VariableValue;
	typedef TransactionFunctionDescription::ConstantValue ConstantValue;
	std::string tok;
	char ch = gotoNextToken( langdescr, si, se);
	if (ch == '\'' || ch == '\"')
	{
		ch = parseNextToken( langdescr, tok, si, se);
		return ConstantValue( tok);
	}
	else if (ch == '$')
	{
		++si;
		Call::Param param = parseReferenceParameter( langdescr, si, se);
		switch (param.type)
		{
			case Call::Param::VariableReference:
			{
				VariableTable::const_iterator vi = varmap.find( param.value);
				if (vi == varmap.end()) throw std::runtime_error( "undefined variable reference in variable value (LET definition)");
				return VariableValue( vi->second);
			}
			case Call::Param::NumericResultReference:
			{
				unsigned short columnidx = boost::lexical_cast<unsigned short>( param.value);
				if (columnidx == 0) std::runtime_error( "illegal result column reference (0) in variable value (LET definition)");
				return VariableValue( columnidx, scope_functionidx);
			}
			case Call::Param::SymbolicResultReference:
			{
				return VariableValue( param.value, scope_functionidx);
			}
			case Call::Param::Constant:
			case Call::Param::InputSelectorPath:
				throw std::runtime_error( "string or result column reference expected as variable value (LET definition)");
		}
	}
	throw std::runtime_error( "string or result column reference expected as variable value (LET definition)");
}

static TransactionFunctionDescription::PreProcessingStep::Argument
	parsePreProcessingStepArgument( const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	typedef TransactionFunctionDescription::PreProcessingStep::Argument Argument;
	Argument rt;
	std::string tok;
	char ch;
	bool nameDefined = false;
	bool nameLegal = true;

	ch = gotoNextToken( langdescr, si, se);
	if (isAlphaNumeric( ch) || ch == '.')
	{
		parseNextToken( langdescr, tok, si, se);
		ch = gotoNextToken( langdescr, si, se);
		if (ch == '=')
		{
			nameDefined = true;
			rt.name = tok;
			++si;
			ch = gotoNextToken( langdescr, si, se);
		}
		else
		{
			rt.value.append( tok);
			rt.name = tok;
		}
	}
	if (ch == '\'' || ch == '\"')
	{
		parseNextToken( langdescr, rt.value, si, se);
		rt.type = Argument::Constant;
	}
	else
	{
		rt.type = Argument::Selector;
		for (;;)
		{
			ch = gotoNextToken( langdescr, si, se);
			if (ch == ',' || ch == ')') break;
			if (!ch) throw std::runtime_error( "unexpected end of function call");

			ch = parseNextToken( langdescr, tok, si, se);

			if (ch == '/' || ch == '*' || ch == '.')
			{
				if (ch == '.' && !nameDefined)
				{
					rt.name = ".";
				}
				else
				{
					nameLegal = false;
				}
				rt.value.push_back( ch);
			}
			else if (isAlphaNumeric( ch))
			{
				nameLegal = true;
				if (!nameDefined) rt.name = tok;
				rt.value.append( tok);
			}
			else
			{
				throw std::runtime_error( "illegal token in path parameter of preprocesing command");
			}
		}
		if (!nameDefined && !nameLegal)
		{
			throw std::runtime_error( "illegal token in path parameter of preprocesing command");
		}
		if (rt.name == ".")
		{
			rt.name.clear();
		}
	}
	return rt;
}

static std::vector<TransactionFunctionDescription::PreProcessingStep::Argument>
	parsePreProcessingStepArguments( const LanguageDescription* langdescr, std::string::const_iterator& si, std::string::const_iterator se)
{
	typedef TransactionFunctionDescription::PreProcessingStep::Argument Argument;
	std::vector<Argument> rt;

	for (;;)
	{
		char ch = gotoNextToken( langdescr, si, se);
		if (ch == ')' || ch == '\0') break;

		rt.push_back( parsePreProcessingStepArgument( langdescr, si, se));
	}
	return rt;
}

static void parseSubroutineArguments( TransactionFunctionDescription::VariableTable& variablemap, const LanguageDescription* langdescr, std::string::const_iterator& si, std::string::const_iterator se)
{
	typedef TransactionFunctionDescription::VariableValue VariableValue;
	typedef TransactionFunctionDescription::VariableTable VariableTable;

	std::string varname;
	char ch;
	int column_idx = 0;
	ch = parseNextToken( langdescr, varname, si, se);
	if (ch == ')') return;

	if (isAlphaNumeric(ch))
	{
		VariableTable::const_iterator vi = variablemap.find( varname);
		if (vi != variablemap.end())
		{
			throw std::runtime_error( std::string("duplicate definition of variable '") + varname + "' (as subroutine argument)");
		}
		variablemap[ varname] = VariableValue( ++column_idx, 0);
	}
	else
	{
		throw std::runtime_error( "variable name expected (itentifier in SUBROUTINE argument list)");
	}
	while (0!=(ch=gotoNextToken( langdescr, si, se)))
	{
		if (ch != ',') break;
		++si;
		ch = parseNextToken( langdescr, varname, si, se);
		if (isAlphaNumeric(ch))
		{
			VariableTable::const_iterator vi = variablemap.find( varname);
			if (vi != variablemap.end())
			{
				throw std::runtime_error( std::string("duplicate definition of variable '") + varname + "' (as subroutine argument)");
			}
			variablemap[ varname] = VariableValue( ++column_idx, 0);
		}
		else
		{
			throw std::runtime_error( "variable name expected (itentifier in SUBROUTINE argument list)");
		}
	}
	if (ch == ')')
	{
		++si;
	}
	else
	{
		throw std::runtime_error( "syntax error in in SUBROUTINE argument list: expected ')'");
	}
}


namespace {
struct Subroutine
{
	Subroutine( const std::string& name_, std::string::const_iterator start_, bool isTransaction_)
		:name(name_),start(start_),isTransaction(isTransaction_),embstm_index(0){}
	Subroutine( const Subroutine& o)
		:name(o.name),start(o.start),isTransaction(o.isTransaction),description(o.description),callstartar(o.callstartar),pprcstartar(o.pprcstartar),result_INTO(o.result_INTO),blockstk(o.blockstk),embstm_index(o.embstm_index){}

	std::string name;
	std::string::const_iterator start;
	bool isTransaction;
	TransactionFunctionDescription description;
	std::vector<std::string::const_iterator> callstartar;
	std::vector<std::string::const_iterator> pprcstartar;
	std::vector<std::string> result_INTO;				//< RESULT INTO path
	std::vector<TransactionFunctionDescription::Block> blockstk;	//< stack of current RESULT INTO block scope
	int embstm_index;
};
}// anonymous namespace

static void parsePreProcessingBlock( Subroutine& subroutine, config::PositionalErrorMessageBase& ERROR, const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	config::PositionalErrorMessageBase::Message MSG;
	std::string tok;
	if (!subroutine.isTransaction)
	{
		throw ERROR( si, "Cannot define PREPROCESS in SUBROUTINE. Only allowed in TRANSACTION definition");
	}
	char ch = 0;
	unsigned int mask = 0;
	TransactionFunctionDescription::PreProcessingStep prcstep;

	if (!parseNextToken( langdescr, tok, si, se)
	||  !boost::algorithm::iequals( tok, "BEGIN"))
	{
		throw ERROR( si, "BEGIN expected after PREPROCESS");
	}
	while ((ch = parseNextToken( langdescr, tok, si, se)) != 0)
	{
		while (subroutine.pprcstartar.size() <= subroutine.description.preprocs.size())
		{
			subroutine.pprcstartar.push_back( si);
		}
		if (ch == ';')
		{
			if (mask)
			{
				subroutine.description.preprocs.push_back( prcstep);
				prcstep.clear();
				mask = 0;
			}
		}
		else if (g_optab[ch])
		{
			throw ERROR( si, MSG << "keyword (END,FOREACH,INTO,DO) expected instead of operator '" << ch << "'");
		}
		else if (ch == '\'' || ch == '\"')
		{
			throw ERROR( si, "keyword (END,FOREACH,INTO,DO) expected instead string");
		}
		else if (boost::algorithm::iequals( tok, "END"))
		{
			if (mask != 0)
			{
				throw std::runtime_error( "preprocessing command not terminated with ';'");
			}
			break;
		}
		else if (boost::algorithm::iequals( tok, "FOREACH"))
		{
			if (0 != (mask & 0x1))
			{
				throw ERROR( si, "selector (FOREACH ..) specified twice in a command");
			}
			mask |= 0x1;

			ch = utils::parseNextToken( prcstep.selector_FOREACH, si, se, utils::emptyCharTable(), utils::anyCharTable());
			if (!ch) throw ERROR( si, "unexpected end of description. sector path expected after FOREACH");
		}
		else if (boost::algorithm::iequals( tok, "INTO"))
		{
			if (0 != (mask & 0x2))
			{
				throw ERROR( si, "function result (INTO ..) specified twice in a command");
			}
			mask |= 0x2;

			prcstep.path_INTO = parse_INTO_path( langdescr, si, se);
		}
		else if (boost::algorithm::iequals( tok, "DO"))
		{
			if (0 != (mask & 0x4))
			{
				throw ERROR( si, "function call (DO ..) specified twice in a command");
			}
			mask |= 0x4;

			ch = parseNextToken( langdescr, prcstep.functionname, si, se);
			if (!isAlphaNumeric( ch))
			{
				throw std::runtime_error( "identifier expected for preprocessing function name after DO");
			}
			ch = gotoNextToken( langdescr, si, se);
			if (ch != '(')
			{
				throw std::runtime_error( "'(' expected after preprocessing function name");
			}
			++si;
			prcstep.args = parsePreProcessingStepArguments( langdescr, si, se);
			ch = gotoNextToken( langdescr, si, se);
			if (ch != ')')
			{
				throw std::runtime_error( "')' expected after preprocessing function arguments");
			}
			++si;
		}
		else
		{
			throw ERROR( si, MSG << "keyword (END,FOREACH,INTO,DO) expected instead of '" << tok << "'");
		}
	}
}

static void parseResultDirective( Subroutine& subroutine, config::PositionalErrorMessageBase& ERROR, const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	config::PositionalErrorMessageBase::Message MSG;
	std::string tok;
	unsigned int mask = 0;

	std::string::const_iterator start = si;
	subroutine.result_INTO.clear();
	while (parseNextToken( langdescr, tok, si, se))
	{
		if (boost::algorithm::iequals( tok, "INTO"))
		{
			if ((mask & 0x2) != 0) throw ERROR( si, "wrong order of definition in RESULT: INTO defined after FILTER");
			if ((mask & 0x1) != 0) throw ERROR( si, "duplicate INTO definition after RESULT");
			mask |= 0x1;
			subroutine.result_INTO = parse_INTO_path( langdescr, si, se);
		}
		else if (boost::algorithm::iequals( tok, "FILTER"))
		{
			if (!subroutine.isTransaction)
			{
				throw ERROR( si, "Cannot define RESULT FILTER in SUBROUTINE. Only allowed as in TRANSACTION definition");
			}
			if ((mask & 0x2) != 0) throw ERROR( si, "duplicate FILTER definition after RESULT");
			mask |= 0x2;
			if (!isAlphaNumeric( parseNextToken( langdescr, subroutine.description.resultfilter, si, se)))
			{
				throw ERROR( si, "identifier expected after RESULT FILTER");
			}
		}
		else if (mask)
		{
			si = start;
			break;
		}
		else
		{
			throw ERROR( si, "INTO or FILTER expected after RESULT");
		}
		start = si;
	}
}

static void parseAuthorizeDirective( Subroutine& subroutine, config::PositionalErrorMessageBase& ERROR, const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	config::PositionalErrorMessageBase::Message MSG;
	std::string tok;

	std::string authfunction;
	std::string authresource;

	if (!subroutine.isTransaction)
	{
		throw ERROR( si, "Cannot define AUTHORIZE in SUBROUTINE. Only allowed as TRANSACTION definition attribute");
	}
	char ch = gotoNextToken( langdescr, si, se);
	if (ch != '(') throw ERROR( si, "Open bracket '(' expected after AUTHORIZE function call");
	si++;
	if (!parseNextToken( langdescr, authfunction, si, se))
	{
		throw ERROR( si, "unexpected end of description. function name expected after AUTHORIZE");
	}
	if (authfunction.empty())
	{
		throw ERROR( si, "AUTHORIZE function name must not be empty");
	}
	ch = gotoNextToken( langdescr, si, se);
	if (ch == ',')
	{
		++si;
		if (!parseNextToken( langdescr, authresource, si, se))
		{
			throw ERROR( si, "unexpected end of description. resource name expected as argument of AUTHORIZE function call");
		}
	}
	ch = gotoNextToken( langdescr, si, se);
	if (ch != ')')
	{
		throw ERROR( si, "Close bracket ')' expected after AUTHORIZE function defintion");
	}
	++si;
	subroutine.description.auth.init( authfunction, authresource);
}

static void parsePrintStep( Subroutine& subroutine, config::PositionalErrorMessageBase& ERROR, const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se, const std::vector<std::string>& print_INTO_path)
{
	config::PositionalErrorMessageBase::Message MSG;
	std::string tok;

	std::vector<std::string> pt = print_INTO_path;

	TransactionFunctionDescription::VariableValue
		varval = parseVariableValue( langdescr, si, se, subroutine.description.steps.size(), subroutine.description.variablemap);

	char ch = gotoNextToken( langdescr, si, se);
	if (isAlphaNumeric(ch))
	{
		if (parseNextToken( langdescr, tok, si, se)
		&&  boost::algorithm::iequals( tok, "INTO"))
		{
			if (pt.empty())
			{
				pt = parse_INTO_path( langdescr, si, se);
				ch = gotoNextToken( langdescr, si, se);
			}
			else
			{
				throw ERROR( si, "INTO specified twice for 'PRINT'");
			}
		}
		else
		{
			throw ERROR( si, "unexpected token after 'PRINT' and argument");
		}
	}
	if (ch != ';')
	{
		throw ERROR( si, "unexpected token as end of print expression (';' expected)");
	}
	subroutine.description.printsteps[ subroutine.description.steps.size()] = TransactionFunctionDescription::PrintStep( pt, varval);
}

static void parseErrorHint( Subroutine& subroutine, config::PositionalErrorMessageBase& ERROR, const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	config::PositionalErrorMessageBase::Message MSG;
	std::string tok;

	if (parseNextToken( langdescr, tok, si, se)
	&&  boost::algorithm::iequals( tok, "ERROR"))
	{
		if (subroutine.description.steps.empty())
		{
			throw ERROR( si, "ON ERROR declaration allowed only after a database call");
		}
		std::string errclass;
		if (!parseNextToken( langdescr, errclass, si, se))
		{
			throw ERROR( si, "error class expected after ON ERROR");
		}
		if (!parseNextToken( langdescr, tok, si, se)
		||  !boost::algorithm::iequals( tok, "HINT"))
		{
			throw ERROR( si, "keyword HINT expected after ON ERROR <errorclass>");
		}
		std::string errhint;
		if (!parseNextToken( langdescr, errhint, si, se))
		{
			throw ERROR( si, "hint message string expected after ON ERROR <errorclass> HINT");
		}
		if (subroutine.description.steps.back().hints.find( errclass) != subroutine.description.steps.back().hints.end())
		{
			throw ERROR( si, std::string( "Duplicate hint for error class '") + errclass + "' for this database call");
		}
		subroutine.description.steps.back().hints[ errclass] = errhint;

		if (';' != gotoNextToken( langdescr, si, se))
		{
			throw ERROR( si, "';' expected after ON ERROR declaration");
		}
		++si;
	}
	else
	{
		throw ERROR( si, "keyword (ERROR) expected after ON");
	}
}


static void parseMainBlock( Subroutine& subroutine, config::PositionalErrorMessageBase& ERROR, const LanguageDescription* langdescr, types::keymap<std::string>& embeddedStatementMap, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	typedef TransactionFunctionDescription::VariableValue VariableValue;
	typedef TransactionFunctionDescription::VariableTable VariableTable;

	config::PositionalErrorMessageBase::Message MSG;
	std::string tok;
	unsigned int mask = 0;
	char ch = 0;

	TransactionFunctionDescription::MainProcessingStep opstep;

	while ((ch = parseNextToken( langdescr, tok, si, se)) != 0)
	{
		while (subroutine.callstartar.size() <= subroutine.description.steps.size())
		{
			subroutine.callstartar.push_back( si);
		}
		if (ch == ';')
		{
			if (mask)
			{
				opstep.finalize();
				subroutine.description.steps.push_back( opstep);
				opstep.clear();
				mask = 0;
			}
		}
		else if (g_optab[ch])
		{
			throw ERROR( si, MSG << "keyword (END,FOREACH,INTO,DO,ON,LET) expected instead of operator '" << ch << "'");
		}
		else if (ch == '\'' || ch == '\"')
		{
			throw ERROR( si, "keyword (END,FOREACH,INTO,DO,ON,LET) expected instead string");
		}
		else if (boost::algorithm::iequals( tok, "ON"))
		{
			parseErrorHint( subroutine, ERROR, langdescr, si, se);
		}
		else if (boost::algorithm::iequals( tok, "LET"))
		{
			std::string varname;
			if (!isAlphaNumeric( parseNextToken( langdescr, varname, si, se)))
			{
				throw ERROR( si, "variable name expected after LET");
			}
			if ('=' != parseNextToken( langdescr, tok, si, se))
			{
				throw ERROR( si, std::string("'=' expected after LET ") + varname);
			}
			int scope_functionidx = subroutine.description.steps.size();
			VariableValue varvalue = parseVariableValue( langdescr, si, se, scope_functionidx, subroutine.description.variablemap);

			VariableTable::const_iterator vi = subroutine.description.variablemap.find( varname);
			if (vi != subroutine.description.variablemap.end())
			{
				throw ERROR( si, std::string("duplicate definition of variable '") + varname + "'");
			}
			subroutine.description.variablemap[ varname] = varvalue;
			if (';' != gotoNextToken( langdescr, si, se))
			{
				throw ERROR( si, std::string("';' expected after LET ") + varname);
			}
			++si;
		}
		else if (boost::algorithm::iequals( tok, "END"))
		{
			if (mask != 0)
			{
				throw std::runtime_error( "database command not terminated with ';'");
			}
			try
			{
				if (subroutine.blockstk.empty())
				{
					if (!subroutine.result_INTO.empty())
					{
						subroutine.description.blocks.insert( subroutine.description.blocks.begin(), TransactionFunctionDescription::Block( subroutine.result_INTO, 0, subroutine.description.steps.size()));
					}
					break; //... end loop, return
				}
				else
				{
					subroutine.blockstk.back().size = subroutine.description.steps.size() - subroutine.blockstk.back().startidx;
					subroutine.description.blocks.push_back( subroutine.blockstk.back());
					subroutine.blockstk.pop_back();
				}
			}
			catch (const TransactionFunctionDescription::MainProcessingStep::Error& err)
			{
				throw ERROR( subroutine.callstartar[ err.elemidx], MSG << "error in definition of transaction: " << err.msg);
			}
			catch (const TransactionFunctionDescription::PreProcessingStep::Error& err)
			{
				throw ERROR( subroutine.pprcstartar[ err.elemidx], MSG << "error in preprocessing step of transaction: " << err.msg);
			}
			catch (const std::runtime_error& err)
			{
				throw ERROR( subroutine.start, MSG << "error in definition of transaction: " << err.what());
			}
			catch (const std::exception& e)
			{
				LOG_ERROR << "uncaught exception loading transaction program: " << e.what();
			}
			catch (...)
			{
				LOG_ERROR << "uncaught exception loading transaction program";
			}
			break;
		}
		else if (boost::algorithm::iequals( tok, "PRINT"))
		{
			std::vector<std::string> print_INTO_path;
			if (mask != 0)
			{
				if (mask == 0x2)
				{
					print_INTO_path = opstep.path_INTO;
					opstep.path_INTO.clear();
					mask = 0;
				}
				else
				{
					throw std::runtime_error( "unexpected token PRINT in middle of database instruction definition");
				}
			}
			parsePrintStep( subroutine, ERROR, langdescr, si, se, print_INTO_path);
		}
		else if (boost::algorithm::iequals( tok, "RESULT"))
		{
			if (mask != 0)
			{
				throw std::runtime_error( "unexpected token RESULT in middle of database command");
			}
			if (parseNextToken( langdescr, tok, si, se)
			&&  boost::algorithm::iequals( tok, "INTO"))
			{
				std::vector<std::string> path = parse_INTO_path( langdescr, si, se);

				if (parseNextToken( langdescr, tok, si, se)
				&&  boost::algorithm::iequals( tok, "BEGIN"))
				{
					subroutine.blockstk.push_back( TransactionFunctionDescription::Block( path, subroutine.description.steps.size(), 0));
				}
				else
				{
					throw ERROR( si, "keyword (BEGIN) expected after RESULT INTO <path>");
				}
			}
			else
			{
				throw ERROR( si, "keyword (INTO) expected after RESULT");
			}
		}
		else if (boost::algorithm::iequals( tok, "FOREACH"))
		{
			if (0 != (mask & 0x1))
			{
				throw ERROR( si, "selector (FOREACH ..) specified twice in a command");
			}
			mask |= 0x1;

			ch = utils::parseNextToken( opstep.selector_FOREACH, si, se, utils::emptyCharTable(), utils::anyCharTable());
			if (!ch) throw ERROR( si, "unexpected end of description. sector path expected after FOREACH");
		}
		else if (boost::algorithm::iequals( tok, "INTO"))
		{
			if (0 != (mask & 0x2))
			{
				throw ERROR( si, "function result (INTO ..) specified twice in a command");
			}
			mask |= 0x2;

			opstep.path_INTO = parse_INTO_path( langdescr, si, se);
		}
		else if (boost::algorithm::iequals( tok, "DO"))
		{
			if (0 != (mask & 0x4))
			{
				throw ERROR( si, "function call (DO ..) specified twice in a command");
			}
			mask |= 0x4;

			std::string::const_iterator oi = si;
			while (parseNextToken( langdescr, tok, oi, se))
			{
				if (boost::algorithm::iequals( tok, "NONEMPTY"))
				{
					opstep.nonempty = true;
					si = oi;
				}
				else if (boost::algorithm::iequals( tok, "UNIQUE"))
				{
					opstep.unique = true;
					si = oi;
				}
				else
				{
					break;
				}
			}
			if (!gotoNextToken( langdescr, si, se))
			{
				throw ERROR( si, "unexpected end of transaction description after DO");
			}
			if (langdescr->isEmbeddedStatement( si, se))
			{
				opstep.call = parseEmbeddedStatement( langdescr, subroutine.name, subroutine.embstm_index++, si, se, embeddedStatementMap);
			}
			else
			{
				opstep.call = parseCallStatement( langdescr, si, se);
			}
		}
		else if (mask == 0x0)
		{
			throw ERROR( si, MSG << "keyword (RESULT,PRINT,LET,END,FOREACH,INTO,DO) expected instead of '" << tok << "'");
		}
		else
		{
			throw ERROR( si, MSG << "keyword (FOREACH,INTO,DO) expected instead of '" << tok << "'");
		}
	}
}


static std::vector<std::pair<std::string,TransactionFunctionR> >
	load( const std::string& source, const LanguageDescription* langdescr, std::string& dbsource, types::keymap<std::string>& embeddedStatementMap)
{
	typedef TransactionFunctionDescription::VariableValue VariableValue;
	typedef TransactionFunctionDescription::VariableTable VariableTable;
	std::vector<std::pair<std::string,TransactionFunctionR> > rt;
	char ch;
	std::string tok;
	std::string::const_iterator si = source.begin(), se = source.end();
	std::string::const_iterator tokstart;
	std::string::const_iterator dbi = source.begin();
	types::keymap<TransactionFunctionR> subroutinemap;
	enum SectionMask {Preprocess=0x1,Authorize=0x2,Result=0x4};

	config::PositionalErrorMessageBase ERROR(source);
	config::PositionalErrorMessageBase::Message MSG;

	dbsource.clear();

	if (!langdescr) throw std::logic_error( "no database language description defined");
	try
	{
		while ((ch = gotoNextToken( langdescr, si, se)) != 0)
		{
			tokstart = si;
			ch = parseNextToken( langdescr, tok, si, se);
			bool enterDefinition = false;
			bool isTransaction = false;
			if (boost::algorithm::iequals( tok, "TRANSACTION") && isLineStart( tokstart, source))
			{
				isTransaction = true;
				enterDefinition = true;
			}
			else if (boost::algorithm::iequals( tok, "SUBROUTINE") && isLineStart( tokstart, source))
			{
				isTransaction = false;
				enterDefinition = true;
			}
			if (enterDefinition)
			{
				std::string::const_iterator dbe = lineStart( tokstart, source);
				std::string transactionName;

				ch = parseNextToken( langdescr, transactionName, si, se);
				if (!ch) throw ERROR( si, MSG << "unexpected end of transaction definition (transaction name expected)");
				if (g_optab[ ch]) throw ERROR( si, MSG << "identifier (transaction name) expected instead of '" << ch << "'");

				Subroutine subroutine( transactionName, dbe, isTransaction);
				subroutine.description.casesensitive = langdescr->isCaseSensitive();

				dbsource.append( std::string( dbi, dbe));
				dbi = dbe;
				unsigned int mask = 0;

				if (gotoNextToken( langdescr, si, se) == '(')
				{
					// ... subroutine call argument list
					parseNextToken( langdescr, tok, si, se);
					if (subroutine.isTransaction) throw ERROR( si, "unexpected token '(': no positional arguments allowed positional transaction function");
					parseSubroutineArguments( subroutine.description.variablemap, langdescr, si, se);
				}
				while (0!=(ch=parseNextToken( langdescr, tok, si, se)))
				{
					if (boost::algorithm::iequals( tok, "RESULT"))
					{
						if ((mask & (unsigned int)Result) != 0) throw std::runtime_error( "duplicate RESULT definition in transaction");
						mask |= (unsigned int)Result;

						parseResultDirective( subroutine, ERROR, langdescr, si, se);
					}
					else if (boost::algorithm::iequals( tok, "AUTHORIZE"))
					{
						if ((mask & (unsigned int)Authorize) != 0) throw std::runtime_error( "duplicate AUTHORIZE definition in transaction");
						mask |= (unsigned int)Authorize;

						parseAuthorizeDirective( subroutine, ERROR, langdescr, si, se);
					}
					else if (boost::algorithm::iequals( tok, "PREPROCESS"))
					{
						if ((mask & (unsigned int)Preprocess) != 0) throw std::runtime_error( "duplicate PREPROCESS definition in transaction");
						mask |= (unsigned int)Preprocess;

						parsePreProcessingBlock( subroutine, ERROR, langdescr, si, se);
					}
					else if (!boost::algorithm::iequals( tok, "BEGIN"))
					{
						throw ERROR( si, std::string("RESULT, AUTHORIZE, PREPROCESS or BEGIN expected instead of '") + tok + "'");
					}
					else
					{
						break;
					}
				}
				parseMainBlock( subroutine, ERROR, langdescr, embeddedStatementMap, si, se);
				if (subroutine.isTransaction)
				{
					LOG_TRACE << "Registering transaction definition '" << subroutine.name << "'";
					TransactionFunctionR ff( createTransactionFunction( subroutine.name, subroutine.description, subroutinemap));
					rt.push_back( std::pair<std::string,TransactionFunctionR>( subroutine.name, ff));
				}
				else
				{
					subroutinemap[ subroutine.name] = TransactionFunctionR( createTransactionFunction( subroutine.name, subroutine.description, subroutinemap));
				}
				// append empty lines to keep line info for the dbsource:
				dbsource.append( std::string( lineCount( dbi, si), '\n'));
				dbi = si;
			}
		}
	}
	catch (const config::PositionalErrorException& e)
	{
		throw e;
	}
	catch (const std::runtime_error& e)
	{
		throw ERROR( si, e.what());
	}
	dbsource.append( std::string( dbi, source.end()));
	return rt;
}


std::vector<std::pair<std::string,TransactionFunctionR> >
	_Wolframe::db::loadTransactionProgramFile(
		const std::string& filename,
		const LanguageDescription* langdescr,
		std::string& dbsource,
		types::keymap<std::string>& embeddedStatementMap)
{
	try
	{
		return load( utils::readSourceFileContent( filename), langdescr, dbsource, embeddedStatementMap);
	}
	catch (const config::PositionalErrorException& e)
	{
		config::PositionalFileError err( e, filename);
		throw config::PositionalFileErrorException( err);
	}
	catch (const std::runtime_error& e)
	{
		LOG_ERROR << "error loading program from file '" << filename << "'";
		throw e;
	}
	catch (...)
	{
		LOG_ERROR << "uncaught exception loading program from file '" << filename << "'";
		throw std::runtime_error( "uncaught exception. see logs");
	}
}


