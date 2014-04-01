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
///\brief Implementation of preprocessing calls
///\file tdl/preprocBlock.cpp
#include "tdl/preprocBlock.hpp"
#include "tdl/preprocCallStatement.hpp"
#include "tdl/elementReference.hpp"
#include "tdl/parseUtils.hpp"
#include "database/databaseLanguage.hpp"
#include "utils/parseUtils.hpp"
#include <string>
#include <vector>
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::db;
using namespace _Wolframe::db::tdl;

static const char* g_preproc_ids[] = {"ENDPROC","FOREACH","INTO","DO",0};
enum PreProcKeyword{ p_NONE,p_ENDPROC,p_FOREACH,p_INTO,p_DO };
static const utils::IdentifierTable g_preproc_idtab( false, g_preproc_ids);

void PreProcStep::clear()
{
	selector.clear();
	resultpath.clear();
	statement.clear();
}

PreProcBlock PreProcBlock::parse( const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	PreProcBlock rt;
	PreProcStep step;
	std::string tok;
	char ch = 0;
	unsigned int mask = 0;

	while ((ch = gotoNextToken( langdescr, si, se)) != 0)
	{
		switch ((PreProcKeyword)utils::parseNextIdentifier( si, se, g_preproc_idtab))
		{
			case p_NONE:
				ch = parseNextToken( langdescr, tok, si, se);
				if (ch == ';')
				{
					if (mask)
					{
						rt.steps.push_back( step);
						step.clear();
						mask = 0;
					}
				}
				else
				{
					throw std::runtime_error( std::string( "keyword (") + g_preproc_idtab.tostring() + ") expected instead of " + errorTokenString( ch, tok));
				}
				break;

			case p_ENDPROC:
				if (mask != 0)
				{
					throw std::runtime_error( "preprocessing command not terminated with ';'");
				}
				return rt;
			case p_FOREACH:
				checkUniqOccurrence( p_FOREACH, mask, g_preproc_idtab);
				step.selector = parseSelectorPath( langdescr, si, se);
				break;
			case p_INTO:
				checkUniqOccurrence( p_INTO, mask, g_preproc_idtab);
				step.resultpath = parse_INTO_path( langdescr, si, se);
				break;
			case p_DO:
				checkUniqOccurrence( p_DO, mask, g_preproc_idtab);
				step.statement = PreProcCallStatement::parse( langdescr, si, se);
				break;
		}
	}
	throw std::runtime_error( "preprocessing block not terminated with 'ENDPROC'");
}

