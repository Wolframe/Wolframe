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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
//\file database/virtualMachine.hpp
//\brief Interface for virtual machine defining database transactions
#ifndef _DATABASE_VIRTUAL_MACHINE_HPP_INCLUDED
#define _DATABASE_VIRTUAL_MACHINE_HPP_INCLUDED
#include "database/vm/instructionSet.hpp"
#include "database/vm/symbolTable.hpp"
#include "database/vm/program.hpp"
#include "database/vm/subroutineFrame.hpp"
#include "types/keymap.hpp"
#include "types/variant.hpp"
#include <string>
#include <vector>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace db {

class VirtualMachine
	:public vm::InstructionSet
{
public:
	typedef std::vector<std::string> SubroutineSignature;

public:
	vm::Program program;						//< symbol table
	vm::SymbolTable symboltab;					//< symbol table
	std::vector<types::Variant> constants;				//< constants
	std::vector<std::string> names;					//< names
	std::map<vm::SymbolTable::Index,SubroutineSignature> signatures;//< subroutine signutures

public:
	VirtualMachine(){}
	VirtualMachine( const VirtualMachine& o)
		:program(o.program)
		,symboltab(o.symboltab)
		,constants(o.constants)
		,names(o.names){}
};

}}//namespace
#endif

