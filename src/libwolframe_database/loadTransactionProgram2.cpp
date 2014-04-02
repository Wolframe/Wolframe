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
#include "tdl/commandDefinition.hpp"
#include "tdl/elementReference.hpp"
#include "tdl/subroutineCallStatement.hpp"
#include "tdl/embeddedStatement.hpp"
#include "tdl/preprocElementReference.hpp"
#include "tdl/preprocCallStatement.hpp"
#include "tdl/preprocBlock.hpp"
#include "tdl/parseUtils.hpp"
#include "transactionFunctionDescription.hpp"
#include "vm/subroutine.hpp"
#include "tdl2vmTranslator.hpp"
#include "utils/parseUtils.hpp"
#include "utils/fileUtils.hpp"
#include "types/conversions.hpp"
#include "logger-v1.hpp"
#include "config/programBase.hpp"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;

static void programAddCommandDefinition( Tdl2vmTranslator& prg, const tdl::CommandDefinition& cmd)
{
	prg.init_resultset();
	if (cmd.nonempty) prg.define_resultset_nonempty();
	if (cmd.unique) prg.define_resultset_unique();

	if (!cmd.selector.empty())
	{
		prg.begin_FOREACH( cmd.selector);
	}
	std::vector<std::string>::const_iterator oi = cmd.resultpath.begin(), oe = cmd.resultpath.end();
	for (; oi != oe; ++oi)
	{
		prg.begin_INTO_block( *oi);
	}
	if (cmd.embedded)
	{
		prg.begin_DO_statement( cmd.statement.stmstring);
		std::vector<tdl::CommandDefinition::Hint>::const_iterator hi = cmd.hints.begin(), he = cmd.hints.end();
		for (; hi != he; ++hi)
		{
			prg.statement_HINT( hi->errorclass, hi->message);
		}
		std::vector<tdl::ElementReference>::const_iterator pi = cmd.statement.params.begin(), pe = cmd.statement.params.end();
		for (; pi != pe; ++pi)
		{
			switch (pi->type)
			{
				case tdl::ElementReference::SelectorPath:
					prg.push_ARGUMENT_PATH( pi->selector);
					break;
				case tdl::ElementReference::LoopCounter:
					prg.push_ARGUMENT_LOOPCNT();
				case tdl::ElementReference::Constant:
					prg.push_ARGUMENT_CONST( pi->selector);
					break;
				case tdl::ElementReference::NamedSetElement:
					prg.push_ARGUMENT_TUPLESET( pi->selector, pi->name);
					break;
				case tdl::ElementReference::IndexSetElement:
					prg.push_ARGUMENT_TUPLESET( pi->selector, pi->index);
					break;
			}
		}
		prg.end_DO_statement();
	}
	else
	{
		prg.begin_DO_subroutine( cmd.call.name, cmd.call.templateparams);
		std::vector<tdl::ElementReference>::const_iterator pi = cmd.call.params.begin(), pe = cmd.call.params.end();
		for (; pi != pe; ++pi)
		{
			switch (pi->type)
			{
				case tdl::ElementReference::SelectorPath:
					prg.push_ARGUMENT_PATH( pi->selector);
					break;
				case tdl::ElementReference::LoopCounter:
					prg.push_ARGUMENT_LOOPCNT();
				case tdl::ElementReference::Constant:
					prg.push_ARGUMENT_CONST( pi->selector);
					break;
				case tdl::ElementReference::NamedSetElement:
					prg.push_ARGUMENT_TUPLESET( pi->selector, pi->name);
					break;
				case tdl::ElementReference::IndexSetElement:
					prg.push_ARGUMENT_TUPLESET( pi->selector, pi->index);
					break;
			}
		}
		prg.end_DO_subroutine();
	}	
	oi = cmd.resultpath.begin();
	for (; oi != oe; ++oi)
	{
		prg.end_INTO_block();
	}
	if (!cmd.selector.empty())
	{
		prg.end_FOREACH();
	}
}


static void parsePrgBlock( Tdl2vmTranslator& prg, const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	static const char* g_prgblock_ids[] = {"END","FOREACH","INTO","DO","ON","KEEP","PRINT",0};
	enum PrgBlockKeyword{ m_NONE, m_END,m_FOREACH,m_INTO,m_DO,m_ON,m_KEEP,m_PRINT};
	static const utils::IdentifierTable g_prgblock_idtab( false, g_prgblock_ids);

	std::string tok;
	char ch = 0;

	while ((ch = tdl::gotoNextToken( langdescr, si, se)) != 0)
	{
		std::string::const_iterator start = si;

		switch ((PrgBlockKeyword)utils::parseNextIdentifier( si, se, g_prgblock_idtab))
		{
			case m_NONE:
				ch = tdl::parseNextToken( langdescr, tok, si, se);
				if (ch == ';')
				{
					throw std::runtime_error( "unexpected semicolon without statement");
				}
				else
				{
					throw std::runtime_error( std::string( "unexpected token in TDL program block. keyword (") + g_prgblock_idtab.tostring() + ") expected instead of " + tdl::errorTokenString( ch, tok));
				}
				break;
			case m_ON:
				throw std::runtime_error( "unexpected keyword ON (for error hint), only valid after database command");
			case m_KEEP:
				if (tdl::parseKeyword( langdescr, si, se, "AS"))
				{
					std::string resultname = tdl::parseResultName( langdescr, si, se);
					if (tdl::gotoNextToken( langdescr, si, se) != ';')
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
				tdl::ElementReference elem = tdl::ElementReference::parsePlainReference( langdescr, si, se);
				std::vector<std::string> pt;
				if (tdl::parseKeyword( langdescr, si, se, "INTO"))
				{
					pt = tdl::parse_INTO_path( langdescr, si, se);
				}
				std::vector<std::string>::const_iterator pi = pt.begin(), pe = pt.end(); 
				for(; pi != pe; ++pi)
				{
					prg.begin_INTO_block( *pi);
				}
				switch (elem.type)
				{
					case tdl::ElementReference::SelectorPath:
						prg.print_ARGUMENT_PATH( elem.selector);
						break;
					case tdl::ElementReference::LoopCounter:
						prg.print_ARGUMENT_LOOPCNT();
					case tdl::ElementReference::Constant:
						prg.print_ARGUMENT_CONST( elem.selector);
						break;
					case tdl::ElementReference::NamedSetElement:
						prg.print_ARGUMENT_TUPLESET( elem.selector, elem.name);
						break;
					case tdl::ElementReference::IndexSetElement:
						prg.print_ARGUMENT_TUPLESET( elem.selector, elem.index);
						break;
				}
				pi = pt.begin(), pe = pt.end(); 
				for(; pi != pe; ++pi)
				{
					prg.end_INTO_block();
				}
				ch = tdl::parseNextToken( langdescr, tok, si, se);
				if (ch != ';')
				{
					throw std::runtime_error( "expected ';' terminating PRINT instruction");
				}
				break;
			}
			case m_DO:
			case m_FOREACH:
			{
				si = start;
				tdl::CommandDefinition cmd = tdl::CommandDefinition::parse( langdescr, si, se);
				programAddCommandDefinition( prg, cmd);
				break;
			}
			case m_INTO:
			{
				std::vector<std::string> resultpath = tdl::parse_INTO_path( langdescr, si, se);
				if (tdl::parseKeyword( langdescr, si, se, "PRINT"))
				{
					tdl::ElementReference elem = tdl::ElementReference::parsePlainReference( langdescr, si, se);

					std::vector<std::string>::const_iterator pi = resultpath.begin(), pe = resultpath.end(); 
					for(; pi != pe; ++pi)
					{
						prg.begin_INTO_block( *pi);
					}
					switch (elem.type)
					{
						case tdl::ElementReference::SelectorPath:
							prg.print_ARGUMENT_PATH( elem.selector);
							break;
						case tdl::ElementReference::LoopCounter:
							prg.print_ARGUMENT_LOOPCNT();
						case tdl::ElementReference::Constant:
							prg.print_ARGUMENT_CONST( elem.selector);
							break;
						case tdl::ElementReference::NamedSetElement:
							prg.print_ARGUMENT_TUPLESET( elem.selector, elem.name);
							break;
						case tdl::ElementReference::IndexSetElement:
							prg.print_ARGUMENT_TUPLESET( elem.selector, elem.index);
							break;
					};
					pi = resultpath.begin(), pe = resultpath.end(); 
					for(; pi != pe; ++pi)
					{
						prg.end_INTO_block();
					}
					ch = tdl::parseNextToken( langdescr, tok, si, se);
					if (ch != ';')
					{
						throw std::runtime_error( "expected ';' terminating PRINT instruction");
					}
				}
				else if (tdl::parseKeyword( langdescr, si, se, "BEGIN"))
				{
					parsePrgBlock( prg, langdescr, si, se);
				}
				else
				{
					si = start;
					tdl::CommandDefinition cmd = tdl::CommandDefinition::parse( langdescr, si, se);
					programAddCommandDefinition( prg, cmd);
				}
				break;
			}
		}
	}
	throw std::runtime_error("unexpected end of file (missing END of block)");
}


static std::vector<std::string> parseDatabaseList( const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	std::vector<std::string> rt;
	std::string dbName;
	for (;;)
	{
		char ch = tdl::parseNextToken( langdescr, dbName, si, se);
		if (!ch) throw std::runtime_error( "unexpected end of file in DATABASE declaration");
		if (tdl::isAlpha(ch))
		{
			rt.push_back( dbName);
			ch = tdl::gotoNextToken( langdescr, si, se);
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
		throw std::runtime_error( "unexpected token in DATABASE declaration");
	}
}

static bool checkDatabaseList( const std::string& databaseID, const std::string& databaseClassName, const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	std::vector<std::string> dblist = parseDatabaseList( langdescr, si, se);
	std::vector<std::string>::const_iterator di = dblist.begin(), de = dblist.end();
	for (; di != de; ++di)
	{
		if (boost::algorithm::iequals( *di, databaseID)) return true;
		if (boost::algorithm::iequals( *di, databaseClassName)) return true;
	}
	return false;
}


static bool parseSubroutineBody( Tdl2vmTranslator& prg, const std::string& databaseId, const std::string& databaseClassName, const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	static const char* g_subroutine_ids[] = {"DATABASE","RESULT","BEGIN",0};
	enum SubroutineKeyword{ s_NONE, s_DATABASE,s_RESULT,s_BEGIN};
	static const utils::IdentifierTable g_subroutine_idtab( false, g_subroutine_ids);

	bool isValidDatabase = true;
	unsigned int mask = 0;
	std::vector<std::string> resultpath;

	while (tdl::gotoNextToken( langdescr, si, se))
	{
		switch ((SubroutineKeyword)utils::parseNextIdentifier( si, se, g_subroutine_idtab))
		{
			case s_NONE:
			{
				std::string tok;
				char ch = utils::parseNextToken( tok, si, se);
				throw std::runtime_error( std::string("unexpected token in TDL subroutine header. keyword (") + g_subroutine_idtab.tostring() + ") expected instead of " + tdl::errorTokenString( ch, tok));
			}
			case s_DATABASE:
				tdl::checkUniqOccurrence( s_DATABASE, mask, g_subroutine_idtab);
				isValidDatabase = checkDatabaseList( databaseId, databaseClassName, langdescr, si, se);
				break;
			case s_RESULT:
				tdl::checkUniqOccurrence( s_RESULT, mask, g_subroutine_idtab);
				if (tdl::parseKeyword( langdescr, si, se, "INTO"))
				{
					resultpath = tdl::parse_INTO_path( langdescr, si, se);
				}
				else
				{
					throw std::runtime_error( "expected INTO after RESULT directive in SUBROUTINE declaration");
				}
				break;
			case s_BEGIN:
			{
				tdl::checkUniqOccurrence( s_BEGIN, mask, g_subroutine_idtab);
				std::vector<std::string>::const_iterator pi = resultpath.begin(), pe = resultpath.end();
				for (; pi != pe; ++pi)
				{
					prg.begin_INTO_block( *pi);
				}
				parsePrgBlock( prg, langdescr, si, se);

				pi = resultpath.begin(), pe = resultpath.end();
				for (; pi != pe; ++pi)
				{
					prg.end_INTO_block();
				}
				return isValidDatabase;
			}
		}
	}
	throw std::runtime_error( "unexpected end of file in subroutine declaration");
}


typedef std::vector<std::pair<std::string,TdlTransactionFunctionR> > TransactionFunctionList;
typedef types::keymap<vm::Subroutine> SubroutineMap;

static bool parseTransactionBody( TdlTransactionFunctionR& tfunc, const std::string& transactionFunctionName, const std::string& databaseId, const std::string& databaseClassName, const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se, const SubroutineMap& subroutineMap)
{
	static const char* g_transaction_ids[] = {"DATABASE","AUTHORIZE","RESULT","PREPROC","BEGIN",0};
	enum TransactionKeyword{ b_NONE, b_DATABASE,b_AUTHORIZE,b_RESULT,b_PREPROC,b_BEGIN};
	static const utils::IdentifierTable g_transaction_idtab( false, g_transaction_ids);

	bool isValidDatabase = true;
	unsigned int mask = 0;
	std::string resultfilter;
	std::vector<std::string> resultpath;
	std::string authfunction;
	std::string authresource;
	tdl::PreProcBlock preproc;

	while (tdl::gotoNextToken( langdescr, si, se))
	{
		switch ((TransactionKeyword)utils::parseNextIdentifier( si, se, g_transaction_idtab))
		{
			case b_NONE:
			{
				std::string tok;
				char ch = utils::parseNextToken( tok, si, se);
				throw std::runtime_error( std::string("unexpected token in TDL subroutine header. keyword (") + g_transaction_idtab.tostring() + ") expected instead of " + tdl::errorTokenString( ch, tok));
			}
			case b_DATABASE:
				tdl::checkUniqOccurrence( b_DATABASE, mask, g_transaction_idtab);
				isValidDatabase = checkDatabaseList( databaseId, databaseClassName, langdescr, si, se);
				break;
			case b_RESULT:
			{
				tdl::checkUniqOccurrence( b_RESULT, mask, g_transaction_idtab);
				bool complete = false;
				if (tdl::parseKeyword( langdescr, si, se, "INTO"))
				{
					resultpath = tdl::parse_INTO_path( langdescr, si, se);
					complete = true;
				}
				if (tdl::parseKeyword( langdescr, si, se, "FILTER"))
				{
					resultfilter = tdl::parseFunctionName( langdescr, si, se);
					complete = true;
				}
				if (!complete)
				{
					throw std::runtime_error( "incomplete RESULT definition, FILTER or INTO expected");
				}
			}
			case b_AUTHORIZE:
			{
				if ('(' != tdl::gotoNextToken( langdescr, si, se))
				{
					throw std::runtime_error( "Open bracket '(' expected after AUTHORIZE function call");
				}
				si++;
				authfunction = tdl::parseFunctionName( langdescr, si, se);
				char ch = tdl::gotoNextToken( langdescr, si, se);
				if (ch == ',')
				{
					++si;
					authresource = tdl::parseResourceName( langdescr, si, se);
				}
				if (')' != tdl::gotoNextToken( langdescr, si, se))
				{
					throw std::runtime_error( "Close bracket ')' expected after AUTHORIZE function defintion");
				}
				++si;
			}
			case b_PREPROC:
			{
				tdl::checkUniqOccurrence( b_PREPROC, mask, g_transaction_idtab);
				preproc = tdl::PreProcBlock::parse( langdescr, si, se);
				break;
			}
			case b_BEGIN:
			{
				tdl::checkUniqOccurrence( b_BEGIN, mask, g_transaction_idtab);
				Tdl2vmTranslator prg( &subroutineMap);

				std::vector<std::string>::const_iterator pi = resultpath.begin(), pe = resultpath.end();
				for (; pi != pe; ++pi)
				{
					prg.begin_INTO_block( *pi);
				}
				parsePrgBlock( prg, langdescr, si, se);

				pi = resultpath.begin(), pe = resultpath.end();
				for (; pi != pe; ++pi)
				{
					prg.end_INTO_block();
				}
				if (isValidDatabase)
				{
					vm::ProgramR program = prg.createProgram();
					tfunc.reset( new TdlTransactionFunction( transactionFunctionName, resultfilter, authfunction, authresource, preproc.build(program.get()), program));
				}
				return isValidDatabase;
			}
		}
	}
	throw std::runtime_error( "unexpected end of file in transaction function declaration");
}


///\brief Forward declaration
static void includeFile( const std::string& mainfilename, const std::string& incfilename, const std::string& databaseId, const std::string& databaseClassName, const LanguageDescription* langdescr, SubroutineMap& subroutineMap, TransactionFunctionList& transactionFunctionList);

static const char* g_toplevel_ids[] = {"DATABASE","TRANSACTION","SUBROUTINE","TEMPLATE","INCLUDE",0};
enum TopLevelKeyword{ t_NONE,t_DATABASE,t_TRANSACTION,t_SUBROUTINE,t_TEMPLATE,t_INCLUDE};
static const utils::IdentifierTable g_toplevel_idtab( false, g_toplevel_ids);

static void load( const std::string& filename, const std::string& source, const std::string& databaseId, const std::string& databaseClassName, const LanguageDescription* langdescr, SubroutineMap& subroutineMap, TransactionFunctionList& transactionFunctionList)
{
	char ch;
	std::string::const_iterator si = source.begin(), se = source.end();

	if (!langdescr) throw std::logic_error( "no database language description defined");
	try
	{
		if (tdl::parseKeyword( langdescr, si, se, "DATABASE"))
		{
			if (!checkDatabaseList( databaseId, databaseClassName, langdescr, si, se))
			{
				LOG_INFO << "TDL file parsed but ignored for database '" << databaseId << "'";
			}
		}
		while ((ch = tdl::gotoNextToken( langdescr, si, se)) != 0)
		{
			std::string::const_iterator start = si;
			switch ((TopLevelKeyword)utils::parseNextIdentifier( si, se, g_toplevel_idtab))
			{
				case t_NONE:
				{
					std::string tok;
					ch = utils::parseNextToken( tok, si, se);
					throw std::runtime_error( std::string("unexpected token in TDL file. keyword (") + g_toplevel_idtab.tostring() + ") expected instead of " + tdl::errorTokenString( ch, tok));
				}
				case t_DATABASE:
					throw std::runtime_error( "unexpected top level DATABASE definition (the top level database definition must be unique and is only allowed as the first declaration in the TDL source");

				case t_TRANSACTION:
				{
					std::string transactionName
						= tdl::parseSubroutineName( langdescr, si, se);

					TdlTransactionFunctionR tfunc;
					if (parseTransactionBody( tfunc, transactionName, databaseId, databaseClassName, langdescr, si, se, subroutineMap))
					{
						std::pair<std::string,TdlTransactionFunctionR> tfuncdef( transactionName, tfunc);
						transactionFunctionList.push_back( tfuncdef);
					}
					else
					{
						utils::LineInfo pos = utils::getLineInfo( source.begin(), start);
						LOG_DEBUG << "TDL transaction function '" << transactionName << "' defined at line " << pos.line << " column " << pos.column << " is ignored because of database exclusion in file " << filename;
					}
					break;
				}
				case t_SUBROUTINE:
				{
					std::string subroutineName
						= tdl::parseSubroutineName( langdescr, si, se);
					std::vector<std::string> templateArguments;
					std::vector<std::string> callArguments 
						= tdl::parseCallArguments( langdescr, si, se);

					Tdl2vmTranslator prg( &subroutineMap);
					if (parseSubroutineBody( prg, databaseId, databaseClassName, langdescr, si, se))
					{
						subroutineMap.insert( 
							subroutineName, 
							vm::Subroutine( subroutineName, templateArguments, callArguments, prg.createProgram()));
					}
					else
					{
						utils::LineInfo pos = utils::getLineInfo( source.begin(), start);
						LOG_DEBUG << "TDL subroutine '" << subroutineName << "' defined at line " << pos.line << " column " << pos.column << " is ignored because of database exclusion in file " << filename;
					}
				}
				case t_TEMPLATE:
				{
					(void)tdl::gotoNextToken( langdescr, si, se);

					std::vector<std::string> templateArguments
						= tdl::parseTemplateArguments( langdescr, si, se);
	
					if (!tdl::parseKeyword( langdescr, si, se, "SUBROUTINE"))
					{
						throw std::runtime_error( "SUBROUTINE declaration expected after TEMPLATE declaration");
					}

					std::string subroutineName
						= tdl::parseSubroutineName( langdescr, si, se);
					std::vector<std::string> callArguments 
						= tdl::parseCallArguments( langdescr, si, se);

					Tdl2vmTranslator prg( &subroutineMap);
					if (parseSubroutineBody( prg, databaseId, databaseClassName, langdescr, si, se))
					{
						subroutineMap.insert( 
							subroutineName, 
							vm::Subroutine( subroutineName, templateArguments, callArguments, prg.createProgram()));
					}
					else
					{
						utils::LineInfo pos = utils::getLineInfo( source.begin(), start);
						LOG_DEBUG << "TDL subroutine template '" << subroutineName << "' at line " << pos.line << " column " << pos.column << " is ignored because of database exclusion in file " << filename;
					}
					break;
				}
				case t_INCLUDE:
				{
					std::string incfile = tdl::parseFilename( langdescr, si, se);
					includeFile( filename, incfile, databaseId, databaseClassName, langdescr, subroutineMap, transactionFunctionList);
					break;
				}
			}
		}
	}
	catch (const std::runtime_error& e)
	{
		config::PositionalErrorMessageBase base(source);
		base.getError( si, e.what());
		
		config::PositionalFileError err( base.getError( si, e.what()), filename);
		throw config::PositionalFileErrorException( err);
	}
}

static void includeFile( const std::string& mainfilename, const std::string& incfilename, const std::string& databaseId, const std::string& databaseClassName, const LanguageDescription* langdescr, SubroutineMap& subroutineMap, TransactionFunctionList& transactionFunctionList)
{
	try
	{
		std::string ext = utils::getFileExtension( incfilename);
		std::string src;
		if (ext.empty())
		{
			src = utils::readSourceFileContent( utils::joinPath( utils::getParentPath( mainfilename), incfilename) + utils::getFileExtension( mainfilename));
		}
		else
		{
			src = utils::readSourceFileContent( utils::joinPath( utils::getParentPath( mainfilename), incfilename));
		}
		load( incfilename, src, databaseId, databaseClassName, langdescr, subroutineMap, transactionFunctionList);
	}
	catch (const config::PositionalFileErrorException& e)
	{
		LOG_ERROR << "error loading program from file '" << mainfilename << " when including file '" << incfilename << "'";
		throw e;
	}
	catch (const std::runtime_error& e)
	{
		LOG_ERROR << "error loading program from file '" << mainfilename << " when including file '" << incfilename << "'";
		throw e;
	}
}

TransactionFunctionList
	db::loadTransactionProgramFile2(
		const std::string& filename,
		const std::string& databaseId,
		const std::string& databaseClassName,
		const LanguageDescription* langdescr)
{
	TransactionFunctionList rt;
	try
	{
		SubroutineMap subroutineMap;
		load( filename, utils::readSourceFileContent( filename), databaseId, databaseClassName, langdescr, subroutineMap, rt);
	}
	catch (const config::PositionalFileErrorException& e)
	{
		throw e;
	}
	catch (const std::runtime_error& e)
	{
		LOG_ERROR << "error loading program from file '" << filename << "'";
		throw e;
	}
	catch (const std::exception& e)
	{
		LOG_FATAL << "uncaught exception loading program from file '" << filename << "':" << e.what();
		throw e;
	}
	return rt;
}


