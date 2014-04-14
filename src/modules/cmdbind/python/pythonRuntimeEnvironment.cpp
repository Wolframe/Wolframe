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
//\file pythonRuntimeEnvironment.cpp
//\brief Interface for python runtime environment
#include "pythonRuntimeEnvironment.hpp"
#include "types/string.hpp"
#include <Python.h>

using namespace _Wolframe;
using namespace _Wolframe::module;

PythonRuntimeEnvironment::PythonRuntimeEnvironment()
	:m_interp(0),m_interp_saved(0)
{
	PyEval_AcquireLock();
	m_interp = PyInterpreterState_New();
	PyEval_ReleaseLock();
}

PythonRuntimeEnvironment::~PythonRuntimeEnvironment()
{
	PyEval_AcquireLock();
	PyInterpreterState_Clear( m_interp);
	PyEval_ReleaseLock();
}

PythonRuntimeEnvironment::PythonRuntimeEnvironment( const _Wolframe::config::NamedConfiguration& cfgi)
	:m_interp(0),m_interp_saved(0)
{
	const PythonRuntimeEnvironmentConfig& cfg = dynamic_cast<const PythonRuntimeEnvironmentConfig&>(cfgi);
	PyEval_AcquireLock();
	m_interp = PyInterpreterState_New();
	
	PyEval_ReleaseLock();
}

langbind::FormFunctionClosure* PythonRuntimeEnvironment::createClosure( const std::string& /*funcname*/) const
{
	return 0;
}


