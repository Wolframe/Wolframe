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
///\file luaCommandHandler.hpp
///\brief Implementation of the lua command handler
#include "luaCommandHandler.hpp"
#include "luaDebug.hpp"
#include "luaObjects.hpp"
#include "logger-v1.hpp"
#include <stdexcept>
#include <cstddef>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

using namespace _Wolframe;
using namespace langbind;
using namespace cmdbind;

void LuaCommandHandler::initcall()
{
	if (!execContext()) throw std::logic_error( "execution context is not defined");
	const proc::ProcessorProviderInterface* provider = execContext()->provider();

	m_interp->init( Input(m_inputfilter,m_docformat), Output(m_outputfilter), execContext());

	if (!m_default_filter.empty())
	{
		types::CountedReference<langbind::Filter> filter( provider->filter( m_default_filter));
		if (!filter.get())
		{
			throw std::runtime_error( std::string( "filter not defined '") + m_default_filter + "'");
		}
		if (!filter->inputfilter().get())
		{
			throw std::runtime_error( std::string( "input filter not defined '") + m_default_filter + "'");
		}
		if (m_inputfilter.get())
		{
			setFilterAs( filter->inputfilter());
		}
		else
		{
			m_inputfilter = filter->inputfilter();
		}
		if (!filter->outputfilter().get())
		{
			throw std::runtime_error( std::string( "output filter not defined '") + m_default_filter + "'");
		}
		if (m_outputfilter.get())
		{
			setFilterAs( filter->outputfilter());
		}
		else
		{
			m_outputfilter = filter->outputfilter();
		}
	}
}

LuaCommandHandler::CallResult LuaCommandHandler::call( const char*& errorCode)
{
	int rt = 0;
	errorCode = 0;
	int nargs = 0;

	if (!m_called)
	{
		try
		{
			initcall();
		}
		catch (const std::exception& e)
		{
			LOG_ERROR << "Failed to load script and initialize execution context: " << e.what();
			errorCode = "init script";
			return Error;
		}
		// call the function (for the first time)
		lua_getglobal( m_interp->thread(), m_name.c_str());
		std::vector<std::string>::const_iterator itr=m_argBuffer.begin(),end=m_argBuffer.end();
		for (;itr != end; ++itr)
		{
			lua_pushlstring( m_interp->thread(), itr->c_str(), itr->size());
		}
		nargs = (int)m_argBuffer.size();
		m_called = true;
	}
	do
	{
		// call the function (subsequently until termination)
		rt = lua_resume( m_interp->thread(), NULL, nargs);
		if (rt == LUA_YIELD)
		{
			if ((m_inputfilter.get() && m_inputfilter->state() != InputFilter::Open)
			||  (m_outputfilter.get() && m_outputfilter->state() != OutputFilter::Open))
			{
				return Yield;
			}
		}
		nargs = 0;
	}
	while (rt == LUA_YIELD);
	if (rt != 0)
	{
		m_lasterror.append( m_interp->luaUserErrorMessage( m_interp->thread()));
		LOG_ERROR << "error calling lua function '" << m_name.c_str() << "':" << m_interp->luaErrorMessage( m_interp->thread());
		errorCode = m_lasterror.c_str();
		return Error;
	}
	return Ok;
}



