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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file directmapCommandHandlerUnit.cpp
///\brief Implementation of the directmap command handler unit
#include "luaCommandHandlerUnit.hpp"
#include "luaCommandHandlerConfig.hpp"
#include "utils/parseUtils.hpp"
#include "logger-v1.hpp"
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::cmdbind;
using namespace _Wolframe::langbind;

CommandHandler* LuaCommandHandlerUnit::createCommandHandler( const std::string& cmdname, const std::string& docformat)
{
	langbind::LuaScriptInstanceR interp;
	if (!m_ctx.funcmap.getLuaScriptInstance( cmdname, interp))
	{
		throw std::runtime_error( std::string( "unknown lua script '") + cmdname + "'");
	}
	types::keymap<langbind::FilterDef>::const_iterator fi = m_filtermap.find( docformat);
	if (fi != m_filtermap.end())
	{
		return new LuaCommandHandler( interp, docformat, fi->second);
	}
	else
	{
		return new LuaCommandHandler( interp, docformat, langbind::FilterDef());
	}
}

bool LuaCommandHandlerUnit::loadPrograms( const proc::ProcessorProviderInterface* provider)
{
	bool rt = true;
	try
	{
		types::keymap<std::string>::const_iterator fi = m_config->filtermap().begin(), fe = m_config->filtermap().end();
		for (; fi != fe; ++fi)
		{
			std::string::const_iterator si = fi->second.begin(), se = fi->second.end();
			langbind::FilterDef fd = langbind::FilterDef::parse( si, se, provider);
			if (utils::gotoNextToken( si, se))
			{
				LOG_ERROR << "superfluous tokens after filter definition: '" << std::string(si,se) << "'";
			}
			m_filtermap.insert( fi->first, fd);
		}
		m_ctx.loadPrograms( m_config->programfiles());
	}
	catch (const std::runtime_error& e)
	{
		LOG_ERROR << "could not load all Lua command handler programs";
		rt = false;
	}
	return rt;
}


