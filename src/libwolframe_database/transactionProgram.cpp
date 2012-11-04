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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\brief Implemention of programs for the database
///\file transactionProgram.cpp
#include "database/transactionProgram.hpp"
#include "utils/miscUtils.hpp"
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
		if (*si == '\n') return --si;
	}
	return si;
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

const utils::CharTable TransactionProgram::m_optab( ";:-,.=)(<>[]{}/&%*|+-#?!$");

void TransactionProgram::loadfile( const std::string& filename, std::string& dbsource)
{
	try
	{
		load( utils::readSourceFileContent( filename), dbsource);
	}
	catch (const config::PositionalErrorException& e)
	{
		throw config::PositionalErrorException( filename, e);
	}
}

bool TransactionProgram::is_mine( const std::string& filename) const
{
	return boost::algorithm::to_lower_copy( utils::getFileExtension( filename)) == ".tdl";
}

char TransactionProgram::gotoNextToken( std::string::const_iterator& si, const std::string::const_iterator se) const
{
	const char* commentopr = m_langdescr->eoln_commentopr();
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

char TransactionProgram::parseNextToken( std::string& tok, std::string::const_iterator& si, std::string::const_iterator se) const
{
	char ch = gotoNextToken( si, se);
	if (!ch) return 0;
	return utils::parseNextToken( tok, si, se, m_optab);
}

std::string TransactionProgram::parseEmbeddedStatement( const std::string& funcname, int index, std::string::const_iterator& osi, std::string::const_iterator ose)
{
	std::string stm;
	std::string dbstm = m_langdescr->parseEmbeddedStatement( osi, ose);
	std::string::const_iterator start = dbstm.begin(), si = dbstm.begin(), se = dbstm.end();
	std::vector< std::string > arg;
	std::string tok;

	char ch = utils::parseNextToken( tok, si, se, m_optab);
	for (; si != se && ch; ch = utils::parseNextToken( tok, si, se, m_optab))
	{
		if (ch == '$')
		{
			stm.append( start, si - 1);
			start = si - 1;
			ch = utils::gotoNextToken( si, se);
			if (ch == '(')
			{
				++si;
				std::string::const_iterator argstart = si;
				ch = utils::parseNextToken( tok, si, se, m_optab);
				for (; ch && ch != ')'; ++si, ch=utils::parseNextToken( tok, si, se, m_optab));
				if (ch == ')')
				{
					arg.push_back( std::string( argstart, si-1));
					start = si;
					stm.append( m_langdescr->stm_argument_reference( arg.size()));
				}
			}
			else if (ch >= '0' && ch <= '9')
			{
				std::string::const_iterator argstart = si;
				for (; si!=se && *si>= '0' && *si<= '9'; ++si);
				arg.push_back( std::string("$") + std::string( argstart, si));
				start = si;
				stm.append( m_langdescr->stm_argument_reference( arg.size()));
			}
		}
	}
	stm.append( start, si);

	std::string stmname;
	stmname.append( "__");
	stmname.append( funcname);
	stmname.append( "_");
	stmname.append( boost::lexical_cast<std::string>( index));
	m_embeddedStatementMap.insert( stmname, stm);

	std::string functioncall;
	functioncall.append( stmname);
	functioncall.append( "(");
	std::vector<std::string>::const_iterator ai = arg.begin(), ae = arg.end();
	for (; ai != ae; ++ai)
	{
		if (ai != arg.begin()) functioncall.append( ",");
		functioncall.append( *ai);
	}
	functioncall.append( ")");
	return functioncall;
}

std::string TransactionProgram::parseCallStatement( std::string::const_iterator& si, std::string::const_iterator se) const
{
	std::string tok;
	int st = 0; // parse state
	std::string::const_iterator fcallstart = si;
	char ch = gotoNextToken( si, se);
	while (st < 3 && (ch = parseNextToken( tok, si, se)) != 0)
	{
		if (!ch)
		{
			throw std::runtime_error( "unexpected end of description. unterminated function call (DO ..)");
		}
		switch (st)
		{
			case 0:
				if (m_optab[ch] || ch == '\'' || ch == '\"')
				{
					throw std::runtime_error( std::string( "function call identifier expected after DO instead of operator or string '") + ch + "'");
				}
				st = 1;
				continue;
			case 1:
				if (ch != '(')
				{
					throw std::runtime_error( std::string( "'(' expected in function call after function name (DO ..) instead of '") + ch + "'");
				}
				st = 2;
				continue;
			case 2:
				if (ch == ')')
				{
					st = 3;
					break;
				}
				else if (ch == '(')
				{
					throw std::runtime_error( "unexpected '('. unterminated function call (DO ..)");
				}
				continue;
		}
	}
	return std::string( fcallstart, si);
}

void TransactionProgram::load( const std::string& source, std::string& dbsource)
{
	char ch;
	std::string tok;
	std::string::const_iterator si = source.begin(), se = source.end();
	std::string::const_iterator tokstart;
	std::string::const_iterator dbi = source.begin();
	std::vector<std::vector< TransactionDescription> > tdar;
	std::vector<std::string> tdnamear;
	std::vector<std::vector<std::string::const_iterator> > tdsrcar;
	std::vector<std::string::const_iterator> tstartar;
	dbsource.clear();
	config::PositionalErrorMessageBase ERROR(source);
	config::PositionalErrorMessageBase::Message MSG;

	if (!m_langdescr) throw std::logic_error( "no database language description defined");

	try
	{
		while ((ch = gotoNextToken( si, se)) != 0)
		{
			tokstart = si;
			ch = parseNextToken( tok, si, se);
			if (boost::algorithm::iequals( tok, "TRANSACTION") && isLineStart( tokstart, source))
			{
				std::string::const_iterator dbe = lineStart( tokstart, source);
				tstartar.push_back( dbe);
				dbsource.append( std::string( dbi, dbe));
				dbi = dbe;
				std::string transactionName;
				int embstm_index = 0;

				ch = parseNextToken( transactionName, si, se);
				if (!ch) throw ERROR( si, MSG << "unexpected end of transaction definition (transaction name expected)");
				if (m_optab[ ch]) throw ERROR( si, MSG << "identifier (transaction name) expected instead of '" << ch << "'");

				tdnamear.push_back( transactionName);
				tdsrcar.push_back( std::vector<std::string::const_iterator>());
				tdar.push_back( std::vector<TransactionDescription>());

				TransactionDescription desc;
				unsigned int mask = 0;

				ch = parseNextToken( tok, si, se);
				if (!boost::algorithm::iequals( tok, "BEGIN"))
				{
					throw ERROR( si, "BEGIN (transaction) expected");
				}
				while ((ch = parseNextToken( tok, si, se)) != 0)
				{
					if (tdsrcar.back().size() <= tdar.back().size())
					{
						tdsrcar.back().push_back( si);
					}
					if (ch == ';')
					{
						if (mask)
						{
							tdar.back().push_back( desc);
							desc.clear();
							mask = 0;
						}
					}
					else if (m_optab[ch])
					{
						throw ERROR( si, MSG << "keyword (END,FOREACH,INTO,DO) expected instead of operator '" << ch << "'");
					}
					else if (ch == '\'' || ch == '\"')
					{
						throw ERROR( si, "keyword (END,FOREACH,INTO,DO) expected instead string");
					}
					else if (boost::algorithm::iequals( tok, "END"))
					{
						break;
					}
					else if (boost::algorithm::iequals( tok, "FOREACH"))
					{
						if (0 != (mask & (1 << (unsigned)TransactionDescription::Selector)))
						{
							throw ERROR( si, "selector (FOREACH ..) specified twice in a transaction description");
						}
						mask |= (1 << (unsigned)TransactionDescription::Selector);

						ch = utils::parseNextToken( desc.selector, si, se, utils::emptyCharTable(), utils::anyCharTable());
						if (!ch) throw ERROR( si, "unexpected end of description. sector path expected after FOREACH");
					}
					else if (boost::algorithm::iequals( tok, "INTO"))
					{
						if (0 != (mask & (1 << (unsigned)TransactionDescription::Output)))
						{
							throw ERROR( si, "function result (INTO ..) specified twice in a transaction description");
						}
						mask |= (1 << (unsigned)TransactionDescription::Output);

						ch = utils::parseNextToken( desc.output, si, se, utils::emptyCharTable(), utils::anyCharTable());
						if (!ch) throw ERROR( si, "unexpected end of description. result tag path expected after INTO");
					}
					else if (boost::algorithm::iequals( tok, "DO"))
					{
						if (0 != (mask & (1 << (unsigned)TransactionDescription::Call)))
						{
							throw ERROR( si, "function call (DO ..) specified twice in a transaction description");
						}
						mask |= (1 << (unsigned)TransactionDescription::Call);
						if (!gotoNextToken( si, se))
						{
							throw ERROR( si, "unexpected end of transaction description after DO");
						}
						if (m_langdescr->isEmbeddedStatement( si, se))
						{
							desc.call.append( parseEmbeddedStatement( transactionName, embstm_index, si, se));
							++embstm_index;
						}
						else
						{
							desc.call.append( parseCallStatement( si, se));
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

	std::vector<std::vector< TransactionDescription> >::const_iterator di = tdar.begin(), de = tdar.end();
	std::vector<std::string>::const_iterator ni = tdnamear.begin();
	std::vector<std::vector< std::string::const_iterator> >::const_iterator pi = tdsrcar.begin();

	for (; di != de; ++di,++pi,++ni)
	{
		try
		{
			m_functionmap[ *ni] = TransactionFunctionR( createTransactionFunction( *ni, *di));
		}
		catch (const TransactionDescription::Error& err)
		{
			std::string errlocation;
			switch (err.elemname)
			{
				case TransactionDescription::Selector:
					errlocation = "in selector expression (FOREACH ..)";
				break;
				case TransactionDescription::Output:
					errlocation = "in transaction ouput (INTO ..)";
				break;
				case TransactionDescription::Call:
					errlocation = "in transaction call (DO ..)";
				break;
			}
			throw ERROR( (*pi)[ err.elemidx], MSG << "error in definition of transaction '" << *ni << "' " << errlocation << ": "<< err.msg);
		}
		catch (const std::runtime_error& err)
		{
			throw ERROR( tstartar[ di - tdar.begin()], MSG << "error in definition of transaction '" << *ni << "': " << err.what());
		}
	}
}

const TransactionFunction* TransactionProgram::function( const std::string& name) const
{
	types::keymap<TransactionFunctionR>::const_iterator fi = m_functionmap.find( name);
	if (fi == m_functionmap.end()) return 0;
	return fi->second.get();
}


