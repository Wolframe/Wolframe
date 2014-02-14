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
//\file dotnetRuntimeEnvironment.cpp
//\brief Implementation .NET runtime environment
#include "dotnetRuntimeEnvironment.hpp"
#include "utils/fileUtils.hpp"
#include "logger-v1.hpp"
#include <algorithm>

using namespace _Wolframe;
using namespace _Wolframe::module;

DotnetRuntimeEnvironment::DotnetRuntimeEnvironment( const DotnetRuntimeEnvironmentConfig* cfg)
	:m_clr(cfg->clrversion())
{
	MOD_LOG_DEBUG << "Registering .NET functions";

	std::vector<DotnetRuntimeEnvironmentConfig::AssemblyDescription>::const_iterator li = cfg->assemblylist().begin(), le = cfg->assemblylist().end();
	for (int typelibidx=0; li != le; ++li,++typelibidx)
	{
		try
		{
			std::string path( utils::joinPath( cfg->typelibpath(), std::string(li->name) + ".tlb"));
			std::replace( path.begin(), path.end(), '/', '\\'); //PF:HACK: Substitution at wrong place because path may contain 'C:/'

			MOD_LOG_DEBUG << "Loading type library '" << li->name << "' from file '" << path << "'";
			m_typelibs.push_back( comauto::TypeLib( path));

			MOD_LOG_TRACE << "Loading functions from type library '" << li->name << "'";
			std::vector<comauto::DotnetFunctionR> funcs = comauto::loadFunctions( &m_typelibs.back(), &m_clr, li->description);
			std::vector<comauto::DotnetFunctionR>::const_iterator fi = funcs.begin(), fe = funcs.end();
		
			for (; fi != fe; ++fi)
			{
				std::string funcname( (*fi)->classname() + "." + (*fi)->methodname());
				m_functionmap.insert( funcname, FunctionDescr( typelibidx,*fi));
			}
		}
		catch (const std::runtime_error& e)
		{
			throw std::runtime_error( std::string("failed to load type library for assembly '") + li->description + "': " + e.what());
		}
	}
}

DotnetRuntimeEnvironment::~DotnetRuntimeEnvironment()
{
}

langbind::FormFunctionClosure* DotnetRuntimeEnvironment::createClosure( const std::string& funcname) const
{
	types::keymap<FunctionDescr>::const_iterator fi = m_functionmap.find( funcname);
	if (fi == m_functionmap.end())
	{
		throw std::runtime_error( std::string( ".NET function not found: '") + funcname + "'");
	}
	return fi->second.m_func->createClosure();
}


