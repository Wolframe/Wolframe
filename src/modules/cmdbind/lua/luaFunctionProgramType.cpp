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
///\file luaFunctionProgramType.cpp
///\brief Implementation of the function to create a form function program type object for lua scripts
#include "luaFunctionProgramType.hpp"
#include "langbind/formFunction.hpp"
#include "luaScriptContext.hpp"
#include "processor/procProvider.hpp"
#include "processor/execContext.hpp"
#include "luaObjects.hpp"
#include "logger-v1.hpp"
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

using namespace _Wolframe;
using namespace _Wolframe::langbind;

namespace {
struct ResultData
	:public langbind::TypedInputFilter::Data
{
	ResultData( const langbind::LuaScriptInstanceR& interp_)
		:m_interp(interp_){}

	ResultData( const ResultData& o)
		:m_interp(o.m_interp){}

	virtual ~ResultData(){}

	virtual Data* copy() const
		{return new ResultData(*this);}

private:
	langbind::LuaScriptInstanceR m_interp;
};

class LuaFormFunctionClosure
	:public langbind::FormFunctionClosure
{
public:
	LuaFormFunctionClosure( const langbind::LuaScriptInstanceR& interp_, const std::string& name_)
		:m_interp(interp_),m_name(name_),m_firstcall(false)
	{}

	virtual ~LuaFormFunctionClosure()
	{}

	virtual bool call()
	{
		if (m_firstcall)
		{
			lua_getglobal( m_interp->thread(), m_name.c_str());
			if (!m_arg.get())
			{
				LOG_ERROR << "lua function got no valid argument";
			}
			m_interp->pushObject( m_arg);
			m_firstcall = false;
		}
		// call the lua form function (subsequently until termination)
		int rt = lua_resume( m_interp->thread(), NULL, 1);
		if (rt == LUA_YIELD) return false;
		if (rt != 0)
		{
			LOG_ERROR << "error calling lua form function '" << m_name.c_str() << "':" << m_interp->luaErrorMessage( m_interp->thread());
			throw std::runtime_error( m_interp->luaUserErrorMessage( m_interp->thread()));
		}
		m_result = m_interp->getObject( -1);
		if (!m_result.get())
		{
			LOG_ERROR << "lua function returned no result or nil (structure expected)";
			throw std::runtime_error( "called lua function without result");
		}
		m_result->setData( new ResultData( m_interp));
		return true;
	}

	///\remark Flags ignored because lua has no strict typing does not validate input parameter structure on its own
	virtual void init( proc::ExecContext* ctx, const TypedInputFilterR& arg, serialize::Flags::Enum)
	{
		m_interp->init( ctx);
		m_arg = arg;
		m_arg->setFlags( TypedInputFilter::SerializeWithIndices);
		//... SerializeWithIndices because lua has no strict typing and needs arrays to be delivered with indices to make single element arrays to appear as arrays too
		m_firstcall = true;
	}

	virtual TypedInputFilterR result() const
	{
		return m_result;
	}

private:
	langbind::LuaScriptInstanceR m_interp;
	TypedInputFilterR m_result;
	std::string m_name;
	TypedInputFilterR m_arg;
	bool m_firstcall;
};

class LuaFormFunction
	:public langbind::FormFunction
{
public:
	LuaFormFunction( const LuaScriptContext* context_, const std::string& name_)
		:m_context(context_),m_name(name_){}

	virtual ~LuaFormFunction(){}

	virtual FormFunctionClosure* createClosure() const
	{
		langbind::LuaScriptInstanceR interp;
		if (!m_context->funcmap.getLuaScriptInstance( m_name, interp)) return 0;
		return new LuaFormFunctionClosure( interp, m_name);
	}

private:
	const LuaScriptContext* m_context;
	std::string m_name;
};
}//anonymous namespace


bool LuaProgramType::is_mine( const std::string& filename) const
{
	boost::filesystem::path p( filename);
	return p.extension().string() == ".lua";
}

void LuaProgramType::loadProgram( prgbind::ProgramLibrary& library, db::Database* /*transactionDB*/, const std::string& filename)
{
	std::vector<std::string> funcs = m_context.loadProgram( filename);
	std::vector<std::string>::const_iterator fi = funcs.begin(), fe = funcs.end();
	for (; fi != fe; ++fi)
	{
		langbind::FormFunctionR ff( new LuaFormFunction( &m_context, *fi));
		library.defineFormFunction( *fi, ff);
	}
}



