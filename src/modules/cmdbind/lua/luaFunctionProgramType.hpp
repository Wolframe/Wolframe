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
///\file luaFunctionProgramType.hpp
///\brief Function to create a form function program type object for lua scripts
#ifndef _Wolframe_LUA_FUNCTION_PROGRAM_TYPE_HPP_INCLUDED
#define _Wolframe_LUA_FUNCTION_PROGRAM_TYPE_HPP_INCLUDED
#include "prgbind/program.hpp"
#include "luaScriptContext.hpp"

namespace _Wolframe {
namespace langbind {

class LuaProgramType
	:public prgbind::Program
{
public:
	LuaProgramType()
		:prgbind::Program( prgbind::Program::Function){}

	virtual ~LuaProgramType(){}

	virtual bool is_mine( const std::string& filename) const;

	virtual void loadProgram( prgbind::ProgramLibrary& library, db::Database* transactionDB, const std::string& filename);

private:
	LuaScriptContext m_context;
};

}} //namespace
#endif


