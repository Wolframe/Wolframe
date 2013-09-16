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
///\file mylangInterpreterInstance.hpp
///\brief Function call for mylang scripts
#ifndef _Wolframe_Mylang_INTERPRETER_INSTANCE_HPP_INCLUDED
#define _Wolframe_Mylang_INTERPRETER_INSTANCE_HPP_INCLUDED
#include "mylangStructure.hpp"
#include "types/countedReference.hpp"
#include "processor/procProvider.hpp"

namespace _Wolframe {
namespace langbind {
namespace mylang {

///\class InterpreterInstance
///\brief Interpreter instance for executing a function
class InterpreterInstance
{
public:
	///\brief Constructor
	InterpreterInstance(){}
	///\brief Destructor
	virtual ~InterpreterInstance(){}

	///\brief Find out if the language binding needs indices (starting with '1') for array elements in the input to distinguish between single elements and arrays with one element
	///\return true, if yes
	bool needsArrayIndices() const		{return true;}

	///\brief Call a function written in 'Mylang'
	StructureR call( const proc::ProcessorProvider* provider, const StructureR& arg);
};

typedef types::CountedReference<InterpreterInstance> InterpreterInstanceR;

}}}//namespace
#endif


