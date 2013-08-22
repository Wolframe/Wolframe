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
#include "logger/logger-v1.hpp"
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

static TransactionFunctionDescription::OperationStep::Call::Param
	parseReferenceParameter( const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	typedef TransactionFunctionDescription::OperationStep::Call Call;
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

static TransactionFunctionDescription::OperationStep::Call::Param
	parsePathParameter( const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	typedef TransactionFunctionDescription::OperationStep::Call Call;
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

static TransactionFunctionDescription::OperationStep::Call
	parseEmbeddedStatement( const LanguageDescription* langdescr, const std::string& funcname, int index, std::string::const_iterator& osi, std::string::const_iterator ose, types::keymap<std::string>& embeddedStatementMap)
{
	typedef TransactionFunctionDescription::OperationStep::Call Call;
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

static TransactionFunctionDescription::OperationStep::Call
	parseCallStatement( const LanguageDescription* langdescr, std::string::const_iterator& ci, std::string::const_iterator ce)
{
	typedef TransactionFunctionDescription::OperationStep::Call Call;
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
	typedef TransactionFunctionDescription::OperationStep::Call Call;
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

namespace {
struct Operation
{
	Operation( const std::string& name_, std::string::const_iterator start_, bool isTransaction_)
		:name(name_),start(start_),isTransaction(isTransaction_),embstm_index(0){}
	Operation( const Operation& o)
		:name(o.name),start(o.start),isTransaction(o.isTransaction),description(o.description),callstartar(o.callstartar),embstm_index(o.embstm_index){}

	std::string name;
	std::string::const_iterator start;
	bool isTransaction;
	TransactionFunctionDescription description;
	std::vector<std::string::const_iterator> callstartar;
	int embstm_index;
};
}// anonymous namespace

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
	types::keymap<TransactionFunctionR> operationmap;

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
			else if (boost::algorithm::iequals( tok, "OPERATION") && isLineStart( tokstart, source))
			{
				isTransaction = false;
				enterDefinition = true;
			}
			if (enterDefinition)
			{
				std::vector<TransactionFunctionDescription::Block> blockstk;
				std::vector<std::string> result_INTO;	//< RESULT INTO path

				std::string::const_iterator dbe = lineStart( tokstart, source);
				std::string transactionName;

				ch = parseNextToken( langdescr, transactionName, si, se);
				if (!ch) throw ERROR( si, MSG << "unexpected end of transaction definition (transaction name expected)");
				if (g_optab[ ch]) throw ERROR( si, MSG << "identifier (transaction name) expected instead of '" << ch << "'");

				Operation operation( transactionName, dbe, isTransaction);
				operation.description.casesensitive = langdescr->isCaseSensitive();

				dbsource.append( std::string( dbi, dbe));
				dbi = dbe;
				TransactionFunctionDescription::OperationStep opstep;
				unsigned int mask = 0;

				while (parseNextToken( langdescr, tok, si, se))
				{
					if (boost::algorithm::iequals( tok, "RESULT"))
					{
						if (!parseNextToken( langdescr, tok, si, se)
						||  !boost::algorithm::iequals( tok, "INTO"))
						{
							throw ERROR( si, "INTO expected after RESULT");
						}
						result_INTO = parse_INTO_path( langdescr, si, se);
					}
					else if (boost::algorithm::iequals( tok, "AUTHORIZE"))
					{
						std::string authfunction;
						std::string authresource;

						if (isTransaction)
						{
							throw ERROR( si, "Cannot define AUTHORIZE in operation. Only allowed as TRANSACTION definition attribute");
						}
						ch = gotoNextToken( langdescr, si, se);
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
						operation.description.auth.init( authfunction, authresource);
					}
					else
					{
						break;
					}
				}
				if (!boost::algorithm::iequals( tok, "BEGIN"))
				{
					throw ERROR( si, "BEGIN (transaction) expected");
				}
				while ((ch = parseNextToken( langdescr, tok, si, se)) != 0)
				{
					while (operation.callstartar.size() <= operation.description.steps.size())
					{
						operation.callstartar.push_back( si);
					}
					if (ch == ';')
					{
						if (mask)
						{
							operation.description.steps.push_back( opstep);
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
						if (parseNextToken( langdescr, tok, si, se)
						&&  boost::algorithm::iequals( tok, "ERROR"))
						{
							if (operation.description.steps.empty())
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
							if (operation.description.steps.back().hints.find( errclass) != operation.description.steps.back().hints.end())
							{
								throw ERROR( si, std::string( "Duplicate hint for error class '") + errclass + "' for this database call");
							}
							operation.description.steps.back().hints[  errclass] = errhint;

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
					else if (boost::algorithm::iequals( tok, "LET"))
					{
						std::string varname;
						if (!parseNextToken( langdescr, varname, si, se))
						{
							throw ERROR( si, "variable name expected after LET");
						}
						if ('=' != parseNextToken( langdescr, tok, si, se))
						{
							throw ERROR( si, std::string("'=' expected after LET ") + varname);
						}
						int scope_functionidx = operation.description.steps.size();
						VariableValue varvalue = parseVariableValue( langdescr, si, se, scope_functionidx, operation.description.variablemap);

						VariableTable::const_iterator vi = operation.description.variablemap.find( varname);
						if (vi != operation.description.variablemap.end())
						{
							throw ERROR( si, std::string("duplicate definition of variable '") + varname + "'");
						}
						operation.description.variablemap[ varname] = varvalue;
						if (';' != gotoNextToken( langdescr, si, se))
						{
							throw ERROR( si, std::string("';' expected after LET ") + varname);
						}
						++si;
					}
					else if (boost::algorithm::iequals( tok, "END"))
					{
						try
						{
							if (blockstk.empty())
							{
								if (!result_INTO.empty())
								{
									operation.description.blocks.insert( operation.description.blocks.begin(), TransactionFunctionDescription::Block( result_INTO, 0, operation.description.steps.size()));
								}
								if (operation.isTransaction)
								{
									LOG_TRACE << "Registering transaction definition '" << operation.name << "'";
									TransactionFunctionR ff( createTransactionFunction( operation.name, operation.description, operationmap));
									rt.push_back( std::pair<std::string,TransactionFunctionR>( operation.name, ff));
								}
								else
								{
									operationmap[ operation.name] = TransactionFunctionR( createTransactionFunction( operation.name, operation.description, operationmap));
								}
							}
							else
							{
								blockstk.back().size = operation.description.steps.size() - blockstk.back().startidx;
								operation.description.blocks.push_back( blockstk.back());
								blockstk.pop_back();
							}
						}
						catch (const TransactionFunctionDescription::OperationStep::Error& err)
						{
							throw ERROR( operation.callstartar[ err.elemidx], MSG << "error in definition of transaction: " << err.msg);
						}
						catch (const std::runtime_error& err)
						{
							throw ERROR( operation.start, MSG << "error in definition of transaction: " << err.what());
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
					else if (boost::algorithm::iequals( tok, "RESULT"))
					{
						if (parseNextToken( langdescr, tok, si, se)
						&&  boost::algorithm::iequals( tok, "INTO"))
						{
							std::vector<std::string> path = parse_INTO_path( langdescr, si, se);

							if (parseNextToken( langdescr, tok, si, se)
							&&  boost::algorithm::iequals( tok, "BEGIN"))
							{
								blockstk.push_back( TransactionFunctionDescription::Block( path, operation.description.steps.size(), 0));
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
							throw ERROR( si, "selector (FOREACH ..) specified twice in a transaction description");
						}
						mask |= 0x1;

						ch = utils::parseNextToken( opstep.selector_FOREACH, si, se, utils::emptyCharTable(), utils::anyCharTable());
						if (!ch) throw ERROR( si, "unexpected end of description. sector path expected after FOREACH");
					}
					else if (boost::algorithm::iequals( tok, "INTO"))
					{
						if (0 != (mask & 0x2))
						{
							throw ERROR( si, "function result (INTO ..) specified twice in a transaction description");
						}
						mask |= 0x2;

						opstep.path_INTO = parse_INTO_path( langdescr, si, se);
					}
					else if (boost::algorithm::iequals( tok, "DO"))
					{
						if (0 != (mask & 0x4))
						{
							throw ERROR( si, "function call (DO ..) specified twice in a transaction description");
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
							opstep.call = parseEmbeddedStatement( langdescr, transactionName, operation.embstm_index++, si, se, embeddedStatementMap);
						}
						else
						{
							opstep.call = parseCallStatement( langdescr, si, se);
						}
					}
					else
					{
						throw ERROR( si, MSG << "keyword (END,FOREACH,INTO,DO) expected instead of '" << tok << "'");
					}
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
	_Wolframe::db::loadTransactionProgramFile( const std::string& filename, const LanguageDescription* langdescr, std::string& dbsource, types::keymap<std::string>& embeddedStatementMap)
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


