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
///\file libwolframe_database/program.hpp
#include "database/program.hpp"
#include "utils/miscUtils.hpp"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;

Program::LineInfo::LineInfo( const std::string::const_iterator& start, const std::string::const_iterator& pos)
	:line(1),col(1)
{
	std::string::const_iterator ii = start;
	for (; ii!=pos; ++ii)
	{
		if (*ii == '\n')
		{
			col = 0;
			line += 1;
			continue;
		}
		++col;
	}
}

Program::Error::Error( const LineInfo& pos_, const std::string& msg_)
	:std::runtime_error(msg_)
{
	std::ostringstream msg;
	msg << "error on line " << pos_.line << " column " << pos_.col << ":" << msg_;
	m_msg = msg.str();
}

Program::Error::Error( const LineInfo& pos_, const std::string& msg_, const std::string& arg_)
	:std::runtime_error(msg_)
{
	std::ostringstream msg;
	msg << "error on line " << pos_.line << " column " << pos_.col << ":" << msg_ << " (" << arg_ << ")";
	m_msg = msg.str();
}

Program::Error::Error( const LineInfo& pos_, const std::string& msg_, char arg_)
	:std::runtime_error(msg_)
{
	std::ostringstream msg;
	if (arg_ < 32)
	{
		msg << "error on line " << pos_.line << " column " << pos_.col << ":" << msg_ << " ('" << arg_ << "')";
	}
	else
	{
		msg << "error on line " << pos_.line << " column " << pos_.col << ":" << msg_ << " (" << (unsigned int)(unsigned char)arg_ << ")";
	}
	m_msg = msg.str();
}

static bool isLineStart( std::string::const_iterator si, const std::string& src)
{
	for (; si >= src.begin() && *si <= ' ' && *si > '\0'; --si)
	{
		if (*si == '\n') return true;
	}
	return si == src.begin();
}

static std::string::const_iterator lineStart( std::string::const_iterator si, const std::string& src)
{
	for (; si >= src.begin() && *si <= ' ' && *si > '\0'; --si)
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

static const utils::CharTable g_optab( ";:-,.=)(<>[]/&%*|+-#?!$");

void Program::loadfile( const std::string& filename, std::string& dbsource)
{
	try
	{
		load( utils::readSourceFileContent( filename), dbsource);
	}
	catch (const std::runtime_error& e)
	{
		throw std::runtime_error( std::string( "error in file '") + utils::getFileStem( filename) + "' " + e.what());
	}
}

void Program::load( const std::string& source, std::string& dbsource)
{
	char ch;
	std::string tok;
	std::string::const_iterator si = source.begin(), se = source.end();
	std::string::const_iterator dbi = source.begin();
	std::vector<std::vector< TransactionDescription> > tdar;
	std::vector<std::string> tdnamear;
	std::vector<std::vector<std::string::const_iterator> > tdsrcar;
	std::vector<std::string::const_iterator> tstartar;
	dbsource.clear();

	try
	{
		while ((ch = utils::gotoNextToken( si, se)) != 0)
		{
			std::string::const_iterator tokstart = si;
			ch = utils::parseNextToken( tok, si, se, g_optab);
			if (ch == m_commentopr[0])
			{
				std::string::const_iterator ci = m_commentopr.begin()+1, ce = m_commentopr.end();
				while (ci != ce && si != se && *ci == *si)
				{
					ci++;
					si++;
				}
				if (ci == ce)
				{
					// skip to end of line
					while (si != se && *si != '\n') ++si;
				}
			}
			else if ((ch|32) == 't'
				&& boost::algorithm::iequals( tok, "TRANSACTION")
				&& isLineStart( tokstart, source))
			{
				std::string::const_iterator dbe = lineStart( tokstart, source);
				tstartar.push_back( dbe);
				dbsource.append( std::string( dbi, dbe));
				dbi = dbe;

				ch = utils::parseNextToken( tok, si, se, g_optab);
				if (g_optab[ ch])
				{
					throw Error( LineInfo( source.begin(), si),
							"identifier (transaction name) expected", ch);
				}
				tdnamear.push_back( tok);
				tdsrcar.push_back( std::vector<std::string::const_iterator>());
				tdar.push_back( std::vector<TransactionDescription>());

				TransactionDescription desc;
				unsigned int mask = 0;

				ch = utils::parseNextToken( tok, si, se, g_optab);
				if (!boost::algorithm::iequals( tok, "BEGIN"))
				{
					throw Error( LineInfo( source.begin(), si),
							"BEGIN (transaction) expected", ch);
				}
				while ((ch = utils::parseNextToken( tok, si, se, g_optab)) != 0)
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
					else if (g_optab[ch])
					{
						throw Error( LineInfo( source.begin(), si),
								"keyword (END,WITH,INTO,DO) expected instead of operator", ch);
					}
					else if (ch == '\'' || ch == '\"')
					{
						throw Error( LineInfo( source.begin(), si),
								"keyword (END,WITH,INTO,DO) expected instead string");
					}
					else if (boost::algorithm::iequals( tok, "END"))
					{
						break;
					}
					else if (boost::algorithm::iequals( tok, "WITH"))
					{
						if (0 != (mask & (1 << (unsigned)TransactionDescription::Selector)))
						{
							throw Error( LineInfo( source.begin(), si),
									"selector (WITH ..) specified twice in a transaction description");
						}
						mask |= (1 << (unsigned)TransactionDescription::Selector);

						ch = utils::parseNextToken( desc.selector, si, se, utils::emptyCharTable(), utils::anyCharTable());
						if (!ch) throw Error( LineInfo( source.begin(), si),
									"unexpected end of description. sector path expected after WITH");
					}
					else if (boost::algorithm::iequals( tok, "INTO"))
					{
						if (0 != (mask & (1 << (unsigned)TransactionDescription::Output)))
						{
							throw Error( LineInfo( source.begin(), si),
									"function result (INTO ..) specified twice in a transaction description");
						}
						mask |= (1 << (unsigned)TransactionDescription::Output);

						ch = utils::parseNextToken( desc.output, si, se, utils::emptyCharTable(), utils::anyCharTable());
						if (!ch) throw Error( LineInfo( source.begin(), si),
									"unexpected end of description. result tag path expected after INTO");
					}
					else if (boost::algorithm::iequals( tok, "DO"))
					{
						if (0 != (mask & (1 << (unsigned)TransactionDescription::Call)))
						{
							throw Error( LineInfo( source.begin(), si),
									"function call (DO ..) specified twice in a transaction description");
						}
						mask |= (1 << (unsigned)TransactionDescription::Call);

						int st = 0;
						std::string::const_iterator fcallstart = si;
						while (st < 3 && (ch = utils::parseNextToken( tok, si, se, g_optab)) != 0)
						{
							if (!ch)
							{
								throw Error( LineInfo( source.begin(), si),
										"unexpected end of description. unterminated function call (DO ..)");
							}
							switch (st)
							{
								case 0:
									if (g_optab[ch] || ch == '\'' || ch == '\"')
									{
										throw Error( LineInfo( source.begin(), si),
												"function call identifier expected after DO instead of operator or string", ch);
									}
									st = 1;
									continue;
								case 1:
									if (ch != '(')
									{
										throw Error( LineInfo( source.begin(), si),
												"'(' expected in function call after function name (DO ..)", ch);
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
									{	throw Error( LineInfo( source.begin(), si),
													"unexpected '('. unterminated function call (DO ..)");
									}
									continue;
							}
						}
						desc.call = std::string( fcallstart, si);
					}
					else
					{
						throw Error( LineInfo( source.begin(), si),
								"keyword (END,WITH,INTO,DO) expected", tok);
					}
				}
				// append empty lines to keep line info for the dbsource:
				dbsource.append( std::string( lineCount( dbi, si), '\n'));
				dbi = si;
			}
		}
	}
	catch (const Error& e)
	{
		throw e;
	}
	catch (const std::runtime_error& e)
	{
		throw Error( LineInfo( source.begin(), si), e.what());
	}

	std::vector<std::vector< TransactionDescription> >::const_iterator di = tdar.begin(), de = tdar.end();
	std::vector<std::string>::const_iterator ni = tdnamear.begin();
	std::vector<std::vector< std::string::const_iterator> >::const_iterator pi = tdsrcar.begin();

	for (; di != de; ++di,++pi,++ni)
	{
		try
		{
			std::string name = boost::algorithm::to_lower_copy( *ni);
			m_functionmap[ name] = TransactionFunctionR( createTransactionFunction( *ni, *di));
		}
		catch (const TransactionDescription::Error& err)
		{
			std::string errlocation;
			switch (err.elemname)
			{
				case TransactionDescription::Selector:
					errlocation = "in selector expression (WITH ..)";
				break;
				case TransactionDescription::Output:
					errlocation = "in transaction ouput (INTO ..)";
				break;
				case TransactionDescription::Call:
					errlocation = "in transaction call (DO ..)";
				break;
			}
			std::ostringstream msg;
			LineInfo lineinfo( source.begin(), (*pi)[ err.elemidx]);
			msg << "error in definition of transaction '" << *ni << "' " << errlocation << ": "<< err.msg;
			throw Error( lineinfo, msg.str());
		}
		catch (const std::runtime_error& err)
		{
			std::ostringstream msg;
			msg << "error in definition of transaction '" << *ni << "': " << err.what();
			LineInfo lineinfo( source.begin(), tstartar[ di - tdar.begin()]);
			throw Error( lineinfo, msg.str());
		}
	}
}

const TransactionFunction* Program::function( const std::string& name) const
{
	std::string key = boost::algorithm::to_lower_copy( name);
	std::map<std::string, TransactionFunctionR>::const_iterator fi = m_functionmap.find( key);
	if (fi == m_functionmap.end()) return 0;
	return fi->second.get();
}


