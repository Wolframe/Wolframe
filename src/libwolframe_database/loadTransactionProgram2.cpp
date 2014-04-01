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
#include "tdl/elementReference.hpp"
#include "tdl/subroutineCallStatement.hpp"
#include "tdl/embeddedStatement.hpp"
#include "tdl/preprocElementReference.hpp"
#include "tdl/preprocCallStatement.hpp"
#include "transactionFunctionDescription.hpp"
#include "utils/parseUtils.hpp"
#include "utils/fileUtils.hpp"
#include "types/conversions.hpp"
#include "logger-v1.hpp"
#include "config/programBase.hpp"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;

static void programAddCommandDefinition( TdlTranslatorInterface& prg, const CommandDefinition& cmd)
{
	
}

static const char* g_prgblock_ids[] = {"END","FOREACH","INTO","DO","ON","KEEP","PRINT",0};
enum PrgBlockKeyword{ m_NONE, m_END,m_FOREACH,m_INTO,m_DO,m_ON,m_KEEP,m_PRINT};
static const utils::IdentifierTable g_prgblock_idtab( false, g_prgblock_ids);

static void parsePrgBlock( TdlTranslatorInterface& prg, const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	std::string tok;
	char ch = 0;

	while ((ch = gotoNextToken( langdescr, si, se)) != 0)
	{
		std::string::const_iterator start = si;

		switch ((PrgBlockKeyword)utils::parseNextIdentifier( si, se, g_prgblock_idtab))
		{
			case m_NONE:
				ch = parseNextToken( langdescr, tok, si, se);
				if (ch == ';')
				{
					throw std::runtime_error( "unexpected semicolon without statement");
				}
				else
				{
					throw std::runtime_error( std::string( "keyword (") + g_prgblock_idtab.tostring() + ") expected instead of " + errorTokenString( ch, tok));
				}
				break;
			case m_ON:
				throw std::runtime_error( "unexpected keyword ON (for error hint), only valid after database command");
			case m_KEEP:
				if (parseNextKeyword( langdescr, si, se, "AS"))
				{
					std::string resultname;
					if (!isAlpha( parseNextToken( langdescr, resultname, si, se)))
					{
						throw std::runtime_error( "identifier expected after KEEP AS");
					}
					if (gotoNextToken( langdescr, si, se) != ';')
					{
						throw std::runtime_error( "semicolon ';' expected after KEEP AS <identifier>");
					}
					++si;
					prg.result_KEEP( resultname);
				}
				break;
			case m_END:
				return;
			case m_PRINT:
			{
				ElementReference elem = ElementReference::parse( langdescr, si, se);
				std::vector<std::string> pt;
				if (parseKeyword( langdescr, si, se, "INTO"))
				{
					pt = parse_INTO_path( langdescr, si, se);
				}
				std::vector<std::string>::const_iterator pi = pt.begin(), pe = pt.end(); 
				for(; pi != pe; ++pi)
				{
					prg.begin_INTO_block( *pi);
				}
				switch (elem.type)
				{
					case SelectorPath:
						prg.print_ARGUMENT_PATH( elem.selector);
						break;
					case LoopCounter:
						prg.print_ARGUMENT_LOOPCNT();
					case Constant:
						prg.print_ARGUMENT_CONST( elem.selector);
						break;
					case NamedSetElement:
						prg.print_ARGUMENT_TUPLESET( elem.name);
						break;
					case IndexSetElement:
						prg.print_ARGUMENT_TUPLESET( elem.index);
						break;
				};
				std::vector<std::string>::const_iterator pi = pt.begin(), pe = pt.end(); 
				for(; pi != pe; ++pi)
				{
					prg.end_INTO_block();
				}
				ch = parseNextToken( langdescr, tok, si, se);
				if (ch != ';')
				{
					throw ERROR( si, "expected ';' terminating PRINT instruction");
				}
				break;
			}
			case m_DO:
			case m_FOREACH:
			{
				si = start;
				CommandDefinition cmd = CommandDefinition::parse( langdescr, si, se);
				programAddCommandDefinition( prg, cmd);
				break;
			}
			case m_INTO:
			{
				std::vector<std::string> pt = parse_INTO_path( langdescr, si, se);
				if (parseKeyword( langdescr, si, se, "PRINT"))
				{
					std::vector<std::string>::const_iterator pi = pt.begin(), pe = pt.end(); 
					for(; pi != pe; ++pi)
					{
						prg.begin_INTO_block( *pi);
					}
					switch (elem.type)
					{
						case SelectorPath:
							prg.print_ARGUMENT_PATH( elem.selector);
							break;
						case LoopCounter:
							prg.print_ARGUMENT_LOOPCNT();
						case Constant:
							prg.print_ARGUMENT_CONST( elem.selector);
							break;
						case NamedSetElement:
							prg.print_ARGUMENT_TUPLESET( elem.name);
							break;
						case IndexSetElement:
							prg.print_ARGUMENT_TUPLESET( elem.index);
							break;
					};
					std::vector<std::string>::const_iterator pi = pt.begin(), pe = pt.end(); 
					for(; pi != pe; ++pi)
					{
						prg.end_INTO_block();
					}
					ch = parseNextToken( langdescr, tok, si, se);
					if (ch != ';')
					{
						throw ERROR( si, "expected ';' terminating PRINT instruction");
					}
				}
				else if (parseKeyword( langdescr, si, se, "BEGIN"))
				{
					std::vector<std::string>::const_iterator pi = pt.begin(), pe = pt.end(); 
					for(; pi != pe; ++pi)
					{
						prg.begin_INTO_block( *pi);
					}
					parsePrgBlock( prg, langdescr, si, se);
					std::vector<std::string>::const_iterator pi = pt.begin(), pe = pt.end(); 
					for(; pi != pe; ++pi)
					{
						prg.end_INTO_block();
					}
				}
				else
				{
					si = start;
					CommandDefinition cmd = CommandDefinition::parse( langdescr, si, se);
					programAddCommandDefinition( prg, cmd);
				}
				break;
			}
		}
	}
	throw std::runtime_error("unexpected end of file (missing END of block)");
}



static const char* g_mainblock_ids[] = {"END","FOREACH","INTO","DO","ON","KEEP","PRINT","RESULT",0};
enum MainBlockKeyword{ m_NONE, m_END,m_FOREACH,m_INTO,m_DO,m_ON,m_KEEP,m_PRINT,m_RESULT};
static const utils::IdentifierTable g_mainblock_idtab(false, g_mainblock_ids);

static void parseMainBlock( Subroutine& subroutine, config::PositionalErrorMessageBase& ERROR, const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	config::PositionalErrorMessageBase::Message MSG;
	std::string tok;
	unsigned int mask = 0;
	char ch = 0;
	types::keymap<int> keepResult_map;

	TransactionFunctionDescription::MainProcessingStep opstep;

	while ((ch = gotoNextToken( langdescr, si, se)) != 0)
	{
		while (subroutine.callstartar.size() <= subroutine.description.steps.size())
		{
			subroutine.callstartar.push_back( si);
		}
		switch ((MainBlockKeyword)utils::parseNextIdentifier( si, se, g_mainblock_idtab))
		{
			case m_NONE:
				ch = parseNextToken( langdescr, tok, si, se);
				if (ch == ';')
				{
					if (mask)
					{
						subroutine.description.steps.push_back( opstep);
						opstep.clear();
						mask = 0;
					}
				}
				else
				{
					if (mask != 0x0 && isAlpha(ch))
					{
						throw ERROR( si, MSG << "keyword (" << g_mainblock_idtab.tostring() << ") expected instead of " << errorTokenString( ch, tok));
					}
					else
					{
						throw ERROR( si, MSG << "keyword (FOREACH,INTO,DO) expected instead of " << errorTokenString( ch, tok));
					}
				}
				break;
			case m_ON:
				throw std::runtime_error( "unexpected keyword ON (for error hint), only valid after database command");
			case m_KEEP:
				if (parseNextKeyword( langdescr, si, se, "AS"))
				{
					std::string resultname;
					if (!isAlpha( parseNextToken( langdescr, resultname, si, se)))
					{
						throw std::runtime_error( "identifier expected after KEEP AS");
					}
					if (gotoNextToken( langdescr, si, se) != ';')
					{
						throw std::runtime_error( "semicolon ';' expected after KEEP AS <identifier>");
					}
					++si;
				}
				break;
			case m_END:
				if (mask != 0) throw std::runtime_error( "database command not terminated with ';'");
				try
				{
					if (subroutine.blockstk.empty())
					{
						if (!subroutine.result_INTO.empty())
						{
							subroutine.description.resultblocks.insert( subroutine.description.resultblocks.begin(), TransactionFunctionDescription::ResultBlock( subroutine.result_INTO, 0, subroutine.description.steps.size()));
						}
					}
					else
					{
						subroutine.blockstk.back().size = subroutine.description.steps.size() - subroutine.blockstk.back().startidx;
						subroutine.description.resultblocks.push_back( subroutine.blockstk.back());
						subroutine.blockstk.pop_back();
					}
				}
				catch (const TransactionFunctionDescription::MainProcessingStep::Error& err)
				{
					throw ERROR( subroutine.callstartar[ err.elemidx], MSG << "error in definition of transaction: " << err.msg);
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
				return;
			case m_PRINT:
				{
					std::vector<std::string> print_INTO_path;
					if (mask != 0)
					{
						unsigned int idshft = (1 << ((unsigned int)m_INTO+1));
						if (mask == idshft)
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
					ElementReference = ElementReference::parse( langdescr, si, se);

					if (parseKeyword( langdescr, si, se, "INTO"))
					{
						if (print_INTO_path.empty())
						{
							print_INTO_path = parse_INTO_path( langdescr, si, se);
						}
						else
						{
							throw ERROR( si, "INTO specified twice for 'PRINT'");
						}
					}
					ch = parseNextToken( langdescr, tok, si, se);
					if (ch != ';')
					{
						throw ERROR( si, "expected ';' terminating PRINT instruction");
					}
				}
				break;
			case m_RESULT:
				if (mask != 0) throw std::runtime_error( "database command not terminated with ';'");
				if (parseKeyword( langdescr, si, se, "INTO"))
				{
					std::vector<std::string> path = parse_INTO_path( langdescr, si, se);
	
					if (0!=(ch=parseNextToken( langdescr, tok, si, se))
					&&  isAlpha(ch) && boost::algorithm::iequals( tok, "BEGIN"))
					{
						subroutine.blockstk.push_back( TransactionFunctionDescription::ResultBlock( path, subroutine.description.steps.size(), 0));
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
				break;
			case m_FOREACH:
				checkUniqOccurrence( m_FOREACH, mask, g_mainblock_idtab, ERROR, si);

				ch = utils::parseNextToken( opstep.selector_FOREACH, si, se, utils::emptyCharTable(), utils::anyCharTable());
				if (!ch) throw ERROR( si, "unexpected end of description. sector path expected after FOREACH");
	
				if (isIdentifier(opstep.selector_FOREACH))
				{
					opstep.resultref_FOREACH = getResultNamespaceIdentifier( opstep.selector_FOREACH, keepResult_map, subroutine.description.steps.size());
					if (opstep.resultref_FOREACH >= 0)
					{
						if (opstep.resultref_FOREACH == 0 && boost::algorithm::iequals( opstep.selector_FOREACH, "PARAM"))
						{
							throw ERROR( si, "PARAM not allowed as argument of FOREACH");
						}
						opstep.selector_FOREACH.clear();
					}
				}
				break;
			case m_INTO:
				checkUniqOccurrence( m_INTO, mask, g_mainblock_idtab, ERROR, si);
	
				opstep.path_INTO = parse_INTO_path( langdescr, si, se);
				break;
			case m_DO:
				checkUniqOccurrence( m_DO, mask, g_mainblock_idtab, ERROR, si);
	
				std::string::const_iterator oi = si;
				while (0!=(ch=parseNextToken( langdescr, tok, oi, se)))
				{
					if (isAlpha(ch) && boost::algorithm::iequals( tok, "NONEMPTY"))
					{
						opstep.nonempty = true;
						si = oi;
					}
					else if (isAlpha(ch) && boost::algorithm::iequals( tok, "UNIQUE"))
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
					opstep.call = parseEmbeddedStatement( langdescr, si, se, subroutine.param_map, keepResult_map, subroutine.description.steps.size());
				}
				else
				{
					opstep.call = parseCallStatement( langdescr, si, se, subroutine.param_map, keepResult_map, subroutine.description.steps.size());
				}
				break;
		}
	}
}

static std::vector<std::string> parseDatabaseList( config::PositionalErrorMessageBase& ERROR, const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	std::vector<std::string> rt;
	std::string dbName;
	for (;;)
	{
		char ch = parseNextToken( langdescr, dbName, si, se);
		if (!ch) throw ERROR( si, "unexpected end of file in DATABASE declaration");
		if (isAlpha(ch))
		{
			rt.push_back( dbName);
			ch = gotoNextToken( langdescr, si, se);
			if (ch == ',')
			{
				++si;
				continue;
			}
			else
			{
				return rt;
			}
		}
		throw ERROR( si, "unexpected token in DATABASE declaration");
	}
}

static bool checkDatabaseList( const std::string& databaseID, const std::string& databaseClassName, config::PositionalErrorMessageBase& ERROR, const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	std::vector<std::string> dblist = parseDatabaseList( ERROR, langdescr, si, se);
	std::vector<std::string>::const_iterator di = dblist.begin(), de = dblist.end();
	for (; di != de; ++di)
	{
		if (boost::algorithm::iequals( *di, databaseID)) return true;
		if (boost::algorithm::iequals( *di, databaseClassName)) return true;
	}
	return false;
}

///\brief Forward declaration
static std::vector<std::pair<std::string,TransactionFunctionR> >
	includeFile( const std::string& mainfilename, const std::string& incfilename, const std::string& databaseId, const std::string& databaseClassName, const LanguageDescription* langdescr, SubroutineDeclarationMap& subroutineMap);

static std::string getInclude( const std::string& mainfilename, const std::string& incfilename)
{
	std::string::const_iterator fi = incfilename.begin(), fe = incfilename.end();
	for (; fi != fe; ++fi)
	{
		if (isAlphaNumeric(ch) || ch == '_' || ch == '/' || ch == '\\' || ch == '.') continue;
		throw std::runtime_error( "illegal include file: only relative paths allowed for include");
	}
	std::string ext = utils::getFileExtension( incfilename);
	if (ext.empty())
	{
		return utils::readSourceFileContent( utils::joinPath( utils::getParentPath( mainfilename), incfilename) + utils::getFileExtension( mainfilename));
	}
	else
	{
		return utils::readSourceFileContent( utils::joinPath( utils::getParentPath( mainfilename), incfilename));
	}
}

static const char* g_body_ids[] = {"DATABASE","TRANSACTION","SUBROUTINE","RESULT","AUTHORIZE","PREPROC","BEGIN",0};
enum BodyKeyword{ b_NONE, b_DATABASE,b_TRANSACTION,b_SUBROUTINE,b_RESULT,b_AUTHORIZE,b_PREPROC,b_BEGIN};
static const utils::IdentifierTable g_body_idtab( false, g_body_ids);

static bool parseProcedureBody( Subroutine& subroutine, config::PositionalErrorMessageBase& ERROR, const std::string& databaseId, const std::string& databaseClassName, const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	unsigned int mask = 0;
	config::PositionalErrorMessageBase::Message MSG;
	char ch;
	std::string tok;

	while (0!=(ch=gotoNextToken( langdescr, si, se)))
	{
		switch ((BodyKeyword)utils::parseNextIdentifier( si, se, g_body_idtab))
		{
			case b_NONE:
				ch = utils::parseNextToken( tok, si, se);
				throw ERROR( si, MSG << "keyword (" << g_body_idtab.tostring() << ") expected instead of identifier '" << tok << "'");
			case b_DATABASE:
				checkUniqOccurrence( b_DATABASE, mask, g_body_idtab, ERROR, si);
				subroutine.isValidDatabase &= checkDatabaseList( databaseId, databaseClassName, ERROR, langdescr, si, se);
				break;
			case b_RESULT:
				checkUniqOccurrence( b_RESULT, mask, g_body_idtab, ERROR, si);
				parseResultDirective( subroutine, ERROR, langdescr, si, se);
				break;
			case b_AUTHORIZE:
				checkUniqOccurrence( b_AUTHORIZE, mask, g_body_idtab, ERROR, si);
				if (!subroutine.isTransaction)
				{
					throw ERROR( si, "Cannot define AUTHORIZE in SUBROUTINE. Only allowed as TRANSACTION definition attribute");
				}
				parseAuthorizeDirective( subroutine, ERROR, langdescr, si, se);
				break;
			case b_PREPROC:
				checkUniqOccurrence( b_PREPROC, mask, g_body_idtab, ERROR, si);
				if (!subroutine.isTransaction)
				{
					throw ERROR( si, "Cannot define PREPROC in SUBROUTINE. Only allowed in TRANSACTION definition");
				}
				parsePreProcessingBlock( subroutine, ERROR, langdescr, si, se);
				break;
			case b_BEGIN:
				checkUniqOccurrence( b_BEGIN, mask, g_body_idtab, ERROR, si);
				parseMainBlock( subroutine, ERROR, langdescr, si, se);
				if (subroutine.isValidDatabase)
				{
					return true;
				}
				else
				{
					LOG_DEBUG << "parsed but ignored " << (subroutine.isTransaction?"TRANSACTION":"OPERATION") << " '" << subroutine.description.name << "' for active transaction database '" << databaseId << "'";
				}
				return false;
			case b_TRANSACTION:
			case b_SUBROUTINE:
				break;
		}
	}
	if (subroutine.isTransaction)
	{
		throw ERROR( si, "missing main block BEGIN..END in TRANSACTION");
	}
	else
	{
		throw ERROR( si, "missing main block BEGIN..END in SUBROUTINE");
	}
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

static const char* g_toplevel_ids[] = {"DATABASE","TRANSACTION","SUBROUTINE","TEMPLATE","INCLUDE",0};
enum TopLevelKeyword{ t_NONE,t_DATABASE,t_TRANSACTION,t_SUBROUTINE,t_TEMPLATE,t_INCLUDE};
static const utils::IdentifierTable g_toplevel_idtab( false, g_toplevel_ids);

static std::vector<std::pair<std::string,TransactionFunctionR> >
	load( const std::string& filename, const std::string& source, const std::string& databaseId, const std::string& databaseClassName, const LanguageDescription* langdescr, SubroutineDeclarationMap& subroutineMap)
{
	std::vector<std::pair<std::string,TransactionFunctionR> > rt;
	char ch;
	bool firstDef = true;
	bool isValidDatabase = true;
	std::vector<std::string> templateArguments;
	std::string tok;
	std::string::const_iterator si = source.begin(), se = source.end();
	std::string::const_iterator tokstart;

	config::PositionalErrorMessageBase ERROR(source);
	config::PositionalErrorMessageBase::Message MSG;

	if (!langdescr) throw std::logic_error( "no database language description defined");
	try
	{
		while ((ch = gotoNextToken( langdescr, si, se)) != 0)
		{
			tokstart = si;
			bool enterDefinition = false;
			bool isTransaction = false;
			templateArguments.clear();

			switch ((TopLevelKeyword)utils::parseNextIdentifier( si, se, g_toplevel_idtab))
			{
				case t_NONE:
					ch = utils::parseNextToken( tok, si, se);
					throw ERROR( si, MSG << "keyword (" << g_toplevel_idtab.tostring() << ") expected instead of identifier '" << tok << "'");
				case t_DATABASE:
					if (!firstDef)
					{
						throw ERROR( si, MSG << "DATABASE definition only allowed at the beginning of the TDL source (global) or as part of the transaction declaration (per function)");
					}
					isValidDatabase = checkDatabaseList( databaseId, databaseClassName, ERROR, langdescr, si, se);
					if (!isValidDatabase)
					{
						LOG_DEBUG << "TDL file parsed but ignored for database '" << databaseId << "'";
					}
					break;
				case t_TRANSACTION:
					isTransaction = true;
					enterDefinition = true;
					break;
				case t_SUBROUTINE:
					isTransaction = false;
					enterDefinition = true;
					break;
				case t_TEMPLATE:
					if (gotoNextToken( langdescr, si, se) != '<')
					{
						throw ERROR( si, MSG << "template argument list in '<' '>' brackets expected instead of '" << ch << "'");
					}
					templateArguments = parseTemplateArguments( langdescr, si, se);
	
					ch = parseNextToken( langdescr, tok, si, se);
					if (isAlpha(ch) && boost::algorithm::iequals( tok, "SUBROUTINE"))
					{
						isTransaction = false;
						enterDefinition = true;
					}
					else
					{
						throw ERROR( si, MSG << "SUBROUTINE declaration expected after TEMPLATE declaration instead of '" << errorTokenString( ch, tok) << "'");
					}
					break;
				case t_INCLUDE:
					enterDefinition = false;

					ch = utils::parseNextToken( tok, si, se, utils::emptyCharTable(), utils::anyCharTable());
					if (!ch) throw ERROR( si, MSG << "unexpected end of include definition (include file name expected)");
					std::vector<std::pair<std::string,TransactionFunctionR> > lst
						= includeFile( filename, tok, databaseId, databaseClassName, langdescr, subroutineMap);
					rt.insert( rt.begin(), lst.begin(), lst.end());
					break;
			}
			if (enterDefinition)
			{
				std::string::const_iterator dbe = lineStart( tokstart, source);
				std::string transactionName;

				ch = parseNextToken( langdescr, transactionName, si, se);
				if (!ch) throw ERROR( si, MSG << "unexpected end of transaction definition (transaction name expected)");
				if (!isAlpha(ch)) throw ERROR( si, MSG << "identifier (transaction name) expected instead of " << errorTokenString( ch, transactionName));

				Subroutine subroutine( templateArguments, dbe, isTransaction);
				subroutine.description.name = transactionName;
				subroutine.description.casesensitive = langdescr->isCaseSensitive();
				subroutine.isValidDatabase = isValidDatabase;

				if (!subroutine.isTransaction && gotoNextToken( langdescr, si, se) == '(')
				{
					// ... subroutine call argument list
					parseNextToken( langdescr, tok, si, se);
					parseSubroutineArguments( subroutine.param_map, langdescr, si, se);
				}
				try
				{
					if (parseProcedureBody( subroutine, ERROR, databaseId, databaseClassName, langdescr, si, se))
					{
						if (subroutine.isTransaction)
						{
							LOG_TRACE << "Registering transaction definition '" << subroutine.description.name << "'";
							TransactionFunctionR func( createTransactionFunction( subroutine.description, langdescr, subroutineMap));
							rt.push_back( std::pair<std::string,TransactionFunctionR>( subroutine.description.name, func));
						}
						else
						{
							TransactionFunctionR func( createTransactionFunction( subroutine.description, langdescr, subroutineMap));
							subroutineMap[ subroutine.description.name] = SubroutineDeclaration( subroutine.templateArguments, func);
						}
					}
				}
				catch (const TransactionFunctionDescription::MainProcessingStep::Error& err)
				{
					throw ERROR( subroutine.callstartar[ err.elemidx], MSG << "error in definition of transaction: " << err.msg);
				}
				catch (const TransactionFunctionDescription::PreProcessingStep::Error& err)
				{
					throw ERROR( subroutine.pprcstartar[ err.elemidx], MSG << "error in definition of transaction: " << err.msg);
				}

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
	return rt;
}

static std::vector<std::pair<std::string,TransactionFunctionR> >
	includeFile( const std::string& mainfilename, const std::string& incfilename, const std::string& databaseId, const std::string& databaseClassName, const LanguageDescription* langdescr, SubroutineDeclarationMap& subroutineMap)
{
	try
	{
		std::string incsrc = getInclude( mainfilename, incfilename);
		return load( mainfilename, incsrc, databaseId, databaseClassName, langdescr, subroutineMap);
	}
	catch (const config::PositionalErrorException& e)
	{
		config::PositionalFileError err( e, incfilename);
		throw config::PositionalFileErrorException( err);
	}
	catch (const std::runtime_error& e)
	{
		LOG_ERROR << "error loading program from file '" << mainfilename << " when including file '" << incfilename << "'";
		throw e;
	}
	catch (...)
	{
		LOG_ERROR << "uncaught exception in TDL include file '" << incfilename << "' included from file '" << mainfilename << "'";
		throw std::runtime_error( "uncaught exception. see logs");
	}
}

std::vector<std::pair<std::string,TransactionFunctionR> >
	db::loadTransactionProgramFile2(
		const std::string& filename,
		const std::string& databaseId,
		const std::string& databaseClassName,
		const LanguageDescription* langdescr)
{
	try
	{
		SubroutineDeclarationMap subroutineMap;
		return load( filename, utils::readSourceFileContent( filename), databaseId, databaseClassName, langdescr, subroutineMap);
	}
	catch (const config::PositionalFileErrorException& e)
	{
		throw e;
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


