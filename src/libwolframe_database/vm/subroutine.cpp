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
//\file vm/subroutine.cpp
//\brief Implementation of subroutine definition
#include "database/vm/subroutine.hpp"
#include "utils/parseUtils.hpp"
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;
using namespace _Wolframe::db::vm;

static const utils::CharTable g_optab( ";:-,.=)(<>[]{}/&%*|+-#?!$");

static bool isAlpha( char ch)
{
	if (ch >= 'A' && ch <= 'Z') return true;
	if (ch >= 'a' && ch <= 'z') return true;
	if (ch == '_') return true;
	return false;
}

static std::string substituteTemplateArguments( const std::string& cmd, const std::vector<std::string>& templateParamNames, const std::vector<std::string>& templateParamValues)
{
	std::string rt;
	std::string tok;
	std::string::const_iterator si = cmd.begin(), se = cmd.end();
	std::string::const_iterator start = si;
	char ch;

	while (0!=(ch=utils::parseNextToken( tok, si, se, g_optab)))
	{
		if (isAlpha(ch))
		{
			std::vector<std::string>::const_iterator ai = templateParamNames.begin(), ae = templateParamNames.end();
			std::vector<std::string>::const_iterator vi = templateParamValues.begin(), ve = templateParamValues.end();
			for (; ai != ae && vi != ve; ++ai,++vi)
			{
				if (boost::algorithm::iequals( tok, *ai))
				{
					// ... substiture identifier
					rt.append( start, si - tok.size());
					rt.append( *vi);
					start = si;
				}
			}
		}
	}
	if (start == cmd.begin())
	{
		return cmd;
	}
	else
	{
		rt.append( start, si);
		return rt;
	}
}

void Subroutine::substituteStatementTemplates( const std::vector<std::string>& templateParamValues)
{
	if (templateParamValues.size() != m_templateParams.size())
	{
		throw std::runtime_error( "number of template parameters do not match");
	}
	if (templateParamValues.size())
	{
		m_vm.reset( new VirtualMachine( *m_vm));
		std::vector<std::string>::iterator si = m_vm->statements.begin(), se = m_vm->statements.end();

		for (; si != se; ++si)
		{
			*si = substituteTemplateArguments( *si, m_templateParams, templateParamValues);
		}
	}
}


