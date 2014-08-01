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
#include <boost/shared_ptr.hpp>

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

	if (m_default_filter.filtertype)
	{
		boost::shared_ptr<langbind::Filter> filter( m_default_filter.create());
		setInputFilter( filter->inputfilter());
		setOutputFilter( filter->outputfilter());
	}
	m_interp->init( input(), output(), execContext());
}

LuaCommandHandler::CallResult LuaCommandHandler::call( const char*& errorCode)
{
	int rt = 0;
	errorCode = 0;

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
		lua_getglobal( m_interp->thread(), m_cmdname.c_str());
		m_called = true;
	}
	if (!m_done)
	{
		do
		{
			// call the function (subsequently until termination)
			rt = lua_resume( m_interp->thread(), NULL, 0);
			if (rt == LUA_YIELD)
			{
				if ((inputfilter().get() && inputfilter()->state() != InputFilter::Open)
				||  (outputfilter().get() && outputfilter()->state() != OutputFilter::Open))
				{
					return Yield;
				}
			}
		}
		while (rt == LUA_YIELD);
		m_done = true;
	}
	if (rt != 0)
	{
		m_lasterror.append( m_interp->luaUserErrorMessage( m_interp->thread()));
		LOG_ERROR << "error calling lua function '" << m_cmdname.c_str() << "':" << m_interp->luaErrorMessage( m_interp->thread());
		errorCode = m_lasterror.c_str();
		return Error;
	}
	else
	{
		if (inputfilter().get())
		{
			if (!inputfilter()->getMetaData())
			{
				return Yield;
			}
			if (outputfilter().get())
			{
				if (!outputfilter()->close())
				{
					return Yield;
				}
			}
		}
		else
		{
			LOG_WARNING << "lua script terminated without input filter not defined";
		}
	}
	return Ok;
}



