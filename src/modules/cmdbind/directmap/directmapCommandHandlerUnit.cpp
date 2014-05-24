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
#include "directmapCommandHandlerUnit.hpp"
#include "directmapCommandHandlerConfig.hpp"
#include "directmapProgram.hpp"
#include "logger-v1.hpp"
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::cmdbind;
using namespace _Wolframe::langbind;

bool DirectmapCommandHandlerUnit::loadPrograms( const proc::ProcessorProviderInterface* provider)
{
	bool rt = true;

	m_filtermap = m_config->filtermap();
	types::keymap<std::string>::const_iterator fi = m_filtermap.begin(), fe = m_filtermap.end();
	for (; fi != fe; ++fi)
	{
		if (!provider->filterType( fi->second))
		{
			rt = false;
			LOG_ERROR << "undefined configured filter type '" << fi->second << "'";
		}
	}
	std::vector<std::string>::const_iterator ci = m_config->programfiles().begin(), ce = m_config->programfiles().end();
	for (; ci != ce; ++ci)
	{
		try
		{
			m_program.loadProgram( *ci, provider);
		}
		catch (const std::runtime_error& e)
		{
			rt = false;
			LOG_ERROR << "Failed to load standard command handler program '" << *ci << "'";
		}
	}
	return rt;
}

CommandHandler* DirectmapCommandHandlerUnit::createCommandHandler( const std::string& cmdname, const std::string& docformat)
{
	DirectmapCommandHandler* rt = 0;
	const DirectmapCommandDescription* descr = m_program.get( cmdname);
	if (!descr) throw std::runtime_error( std::string( "command is not defined '") + cmdname + "'");

	types::keymap<std::string>::const_iterator fi = m_filtermap.find( docformat);
	if (fi == m_filtermap.end()) fi = m_filtermap.find( "");
	if (fi == m_filtermap.end())
	{
		LOG_WARNING << "Default filter for document format '" << docformat << "' is not defined";
		rt = new DirectmapCommandHandler( descr, "");
	}
	else
	{
		rt = new DirectmapCommandHandler( descr, fi->second);
	}
	return rt;
}



