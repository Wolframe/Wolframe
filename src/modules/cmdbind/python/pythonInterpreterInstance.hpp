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
///\file pythonInterpreterInstance.hpp
///\brief Interface to interpreter instances created for running one form function call to python
#ifndef _Wolframe_PYTHON_INTERPRETER_INSTANCE_HPP_INCLUDED
#define _Wolframe_PYTHON_INTERPRETER_INSTANCE_HPP_INCLUDED
#include "pythonStructure.hpp"
#include "types/variant.hpp"
#include "processor/procProviderInterface.hpp"
#include "processor/execContext.hpp"
#include <utility>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace langbind {
namespace python {

///\class Instance
///\brief Interpreter instance for executing a function
class InterpreterInstance
{
public:
	///\brief Constructor
	InterpreterInstance();
	///\brief Destructor
	virtual ~InterpreterInstance(){}

	///\brief Call a function written in 'Mylang'
	StructureR call( proc::ExecContext* ctx, const StructureR& arg);
};

typedef boost::shared_ptr<InterpreterInstance> InterpreterInstanceR;

}}} //namespace
#endif


