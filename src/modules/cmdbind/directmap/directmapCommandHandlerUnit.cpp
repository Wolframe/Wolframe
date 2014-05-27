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
#include "filter/filterdef.hpp"
#include "directmapCommandHandlerUnit.hpp"
#include "directmapCommandHandlerConfig.hpp"
#include "directmapProgram.hpp"
#include "utils/parseUtils.hpp"
#include "logger-v1.hpp"
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::cmdbind;
using namespace _Wolframe::langbind;

bool DirectmapCommandHandlerUnit::loadPrograms( const proc::ProcessorProviderInterface* provider)
{
	bool rt = true;

	types::keymap<std::string>::const_iterator fi = m_config->filtermap().begin(), fe = m_config->filtermap().end();
	for (; fi != fe; ++fi)
	{
		try
		{
			std::string::const_iterator si = fi->second.begin(), se = fi->second.end();
			langbind::FilterDef fd = langbind::FilterDef::parse( si, se, provider);
			if (utils::gotoNextToken( si, se))
			{
				LOG_ERROR << "superfluous tokens after filter definition: '" << std::string(si,se) << "'";
			}
			m_filtermap.insert( fi->first, fd);
		}
		catch (const std::runtime_error& e)
		{
			rt = false;
			LOG_ERROR << "error in configured default filter: " << e.what();
		}
	}
	std::vector<std::string>::const_iterator ci = m_config->programfiles().begin(), ce = m_config->programfiles().end();
	for (; ci != ce; ++ci)
	{
		if (!m_program.loadProgram( *ci, provider))
		{
			rt = false;
		}
	}
	return rt;
}

CommandHandler* DirectmapCommandHandlerUnit::createCommandHandler( const std::string& cmdname, const std::string& docformat)
{
	const DirectmapCommandDescription* descr = m_program.get( cmdname);
	if (!descr) throw std::runtime_error( std::string( "command is not defined '") + cmdname + "'");

	InputFilterR inputfilter;
	OutputFilterR outputfilter;

	if (descr->inputfilterdef.filtertype)
	{
		langbind::FilterR filter( descr->inputfilterdef.filtertype->create( descr->inputfilterdef.arg));
		inputfilter = filter->inputfilter();
	}
	if (descr->outputfilterdef.filtertype)
	{
		langbind::FilterR filter( descr->outputfilterdef.filtertype->create( descr->outputfilterdef.arg));
		outputfilter = filter->outputfilter();
	}
	if (!descr->inputfilterdef.filtertype || !descr->outputfilterdef.filtertype)
	{
		langbind::FilterR filter;

		types::keymap<langbind::FilterDef>::const_iterator fi = m_filtermap.find( docformat);
		if (fi == m_filtermap.end()) fi = m_filtermap.find( "");
		if (fi == m_filtermap.end())
		{
			throw std::runtime_error( std::string( "filter not defined for command '") + cmdname + "' and no default filter defined for document format '" + docformat + "'");
		}
		filter.reset( fi->second.create());

		if (!descr->outputfilterdef.filtertype)
		{
			outputfilter = filter->outputfilter();
		}
		if (!descr->inputfilterdef.filtertype)
		{
			inputfilter = filter->inputfilter();
		}
	}
	return new DirectmapCommandHandler( descr, inputfilter, outputfilter);
}




