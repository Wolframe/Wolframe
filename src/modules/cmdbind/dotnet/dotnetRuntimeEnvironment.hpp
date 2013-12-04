/************************************************************************
Copyright (C) 2011 - 2013 Project Wolframe.
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
//\file dotnetRuntimeEnvironment.hpp
//\brief Interface .NET runtime environment
#ifndef _Wolframe_DOTNET_RUNTIME_ENVIRONMENT_HPP_INCLUDED
#define _Wolframe_DOTNET_RUNTIME_ENVIRONMENT_HPP_INCLUDED
#include "dotnetRuntimeEnvironmentConfig.hpp"
#include "langbind/formFunction.hpp"
#include "types/keymap.hpp"
#include "comauto/clr.hpp"
#include "comauto/typelib.hpp"
#include <string>
#include <map>
#include <vector>

namespace _Wolframe {
namespace module {

//\class DotnetRuntimeEnvironment
class DotnetRuntimeEnvironment
	:public prgbind::RuntimeEnvironment
{
public:
	DotnetRuntimeEnvironment( const DotnetRuntimeEnvironmentConfig* cfg);
	virtual ~DotnetRuntimeEnvironment();

	virtual langbind::FormFunctionClosure* createClosure( const std::string& funcname) const;

	virtual std::vector<std::string> functions() const
	{
		return m_functionmap.getkeys<std::vector<std::string> >();
	}

private:
	comauto::CommonLanguageRuntime m_clr;
	std::vector<comauto::TypeLib> m_typelibs;

	struct FunctionDescr
	{
		std::size_t m_typelibidx;
		comauto::FunctionR m_func;

		FunctionDescr()
			:m_typelibidx(0){}
		FunctionDescr( std::size_t typelibidx_, const comauto::FunctionR& func_)
			:m_typelibidx(typelibidx_),m_func(func_){}
		FunctionDescr( const FunctionDescr& o)
			:m_typelibidx(o.m_typelibidx),m_func(o.m_func){}
	};
	types::keymap<FunctionDescr> m_functionmap;
};

}} namespace
#endif


