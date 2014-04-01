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
///\brief Definition of a database instruction or subroutine call with FOREACH and INTO qualifier as command
///\file tdl/commandDefinition.cpp
#include "tdl/commandDefinition.hpp"
#include "utils/parseUtils.hpp"
#include "tdl/parseUtils.hpp"
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::db;
using namespace _Wolframe::db::tdl;

static const char* g_commanddef_ids[] = {"ON","END","FOREACH","INTO","DO",0};
enum MainBlockKeyword{ m_NONE, m_ON, m_END, m_FOREACH, m_INTO, m_DO };
static const utils::IdentifierTable g_commanddef_idtab(false, g_commanddef_ids);

CommandDefinition CommandDefinition::parse( const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	CommandDefinition rt;
	std::string tok;
	unsigned int mask = 0;
	char ch = 0;

	while ((ch = gotoNextToken( langdescr, si, se)) != 0)
	{
		switch ((MainBlockKeyword)utils::parseNextIdentifier( si, se, g_commanddef_idtab))
		{
			case m_NONE:
				ch = parseNextToken( langdescr, tok, si, se);
				if (ch == ';')
				{
					if (mask)
					{
						while (parseKeyword( langdescr, si, se, "ON"))
						{
							if (!rt.embedded)
							{
								throw std::runtime_error( "hints only allowed after commands with embedded database statements");
							}
							if (parseKeyword( langdescr, si, se, "ERROR"))
							{
								Hint hint;
								ch = parseNextToken( langdescr, hint.errorclass, si, se);
								if (!isStringQuote( ch) && !isAlphaNumeric( ch))
								{
									throw std::runtime_error( "error class expected after ON ERROR");
								}
								if (!parseKeyword( langdescr, si, se, "HINT"))
								{
									throw std::runtime_error( "keyword HINT expected after ON ERROR <errorclass>");
								}
								if (!isStringQuote( parseNextToken( langdescr, hint.message, si, se)))
								{
									throw std::runtime_error( "hint message string expected after ON ERROR <errorclass> HINT");
								}
								rt.hints.push_back( hint);
						
								if (';' != gotoNextToken( langdescr, si, se))
								{
									throw std::runtime_error( "';' expected after ON ERROR <class> HINT <message> declaration");
								}
								++si;
							}
							else
							{
								throw std::runtime_error( "keyword (ERROR) expected after ON (database error hint)");
							}
						}
						return rt;
					}
					else
					{
						throw std::runtime_error( "superfluous semicolon ';' (empty statement)");
					}
				}
				else
				{
					throw std::runtime_error( std::string("keyword (") + g_commanddef_idtab.tostring() + ") or semicolon ';' expected instead of " + errorTokenString( ch, tok));
				}
				break;
			case m_END:
				throw std::runtime_error( "command not terminated with semicolon ';' before closing the block with END");
			case m_ON:
				throw std::runtime_error( "command not terminated with semicolon ';' before specifying error hints with ON");
			case m_FOREACH:
				checkUniqOccurrence( m_FOREACH, mask, g_commanddef_idtab);

				rt.selector = parseSelectorPath( langdescr, si, se);
				break;
			case m_INTO:
				checkUniqOccurrence( m_INTO, mask, g_commanddef_idtab);
	
				rt.resultpath = parse_INTO_path( langdescr, si, se);
				break;
			case m_DO:
				checkUniqOccurrence( m_DO, mask, g_commanddef_idtab);
	
				for (;;)
				{
					if (parseKeyword( langdescr, si, se, "NONEMPTY"))
					{
						rt.nonempty = true;
					}
					else if (parseKeyword( langdescr, si, se, "UNIQUE"))
					{
						rt.unique = true;
					}
					else
					{
						break;
					}
						
				}
				if (!gotoNextToken( langdescr, si, se))
				{
					throw std::runtime_error( "unexpected end of transaction description after DO");
				}
				if (langdescr->isEmbeddedStatement( si, se))
				{
					rt.embedded = true;
					rt.statement = EmbeddedStatement::parse( langdescr, si, se);
				}
				else
				{
					rt.embedded = false;
					rt.call = SubroutineCallStatement::parse( langdescr, si, se);
				}
				break;
		}
	}
	throw std::runtime_error( "unexpected end of source in command definition");
}


