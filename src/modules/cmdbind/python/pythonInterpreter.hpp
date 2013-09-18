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
///\file pythonInterpreter.hpp
///\brief Interpreter interface. The interpreter is the object collecting all resources and data shared in the interpreter context and it creates the instances doing the job.
#ifndef _Wolframe_PYTHON_INTERPRETER_HPP_INCLUDED
#define _Wolframe_PYTHON_INTERPRETER_HPP_INCLUDED
#include "pythonInterpreterInstance.hpp"
#include <string>
#include <vector>

namespace _Wolframe {
namespace langbind {
namespace python {

class Interpreter
{
public:
	Interpreter();

	virtual ~Interpreter( );

	const std::vector<std::string>& functions( ) const { return m_functions; }

	std::string lastErrorMsg( const std::string& str );

	std::vector<std::string> loadProgram( const std::string& name);
	InterpreterInstanceR getInstance( const std::string& name) const;

private:
	std::string m_modulePath;
	std::vector<std::string> m_functions;

	const char *pyGetRepr( PyObject *o );
	const char *pyGetStr( PyObject *o );
};

}//namespace python

}} //namespace
#endif


