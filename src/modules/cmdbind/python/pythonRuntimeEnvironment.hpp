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
///\file pythonRuntimeEnvironment.hpp
///\brief Interface for python runtime environment
#ifndef _Wolframe_Python_RUNTIME_ENVIRONMENT_HPP_INCLUDED
#define _Wolframe_Python_RUNTIME_ENVIRONMENT_HPP_INCLUDED
#include "pythonRuntimeEnvironmentConfig.hpp"
#include "processor/procProviderInterface.hpp"
#include "langbind/runtimeEnvironment.hpp"
#include "types/keymap.hpp"
#include <string>
#include <vector>
#include <map>

#include <Python.h>

namespace _Wolframe {
namespace module {

///\brief Runtime environment for python
class PythonRuntimeEnvironment
	:public langbind::RuntimeEnvironment
{
public:
	PythonRuntimeEnvironment();
	PythonRuntimeEnvironment( const _Wolframe::config::NamedConfiguration& cfgi);
	virtual ~PythonRuntimeEnvironment();

	///\brief createClosure create a closure to execute function with name 'funcname'
	///\param [in] funcname function name
	virtual langbind::FormFunctionClosure* createClosure( const std::string& funcname) const;

	///\brief Get the names of the functions implemented
	///\return the function names
	virtual std::vector<std::string> functions() const
	{
		return m_funcmap.getkeys<std::vector<std::string> >();
	}

	virtual const char* name() const
	{
		return "python V2.7";
	}

private:
	PyInterpreterState* m_interp;
	PyInterpreterState* m_interp_saved;

	struct FunctionDescr
	{
		FunctionDescr()
			{}
		FunctionDescr( const FunctionDescr&)
			{}
	};
	types::keymap<FunctionDescr> m_funcmap;
};

}} //namespace
#endif

