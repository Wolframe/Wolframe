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
//\file virtualMachineTdlTranslatorInterface.hpp
//\brief Helper interface for building a virtual machine for database transactions out of TDL
#ifndef _DATABASE_VIRTUAL_MACHINE_TDL_TRANSLATOR_INTERFACE_HPP_INCLUDED
#define _DATABASE_VIRTUAL_MACHINE_TDL_TRANSLATOR_INTERFACE_HPP_INCLUDED
#include "database/virtualMachine.hpp"
#include "database/vm/subroutine.hpp"
#include "types/keymap.hpp"
#include <boost/algorithm/string.hpp>

namespace _Wolframe {
namespace db {

class VirtualMachineTdlTranslatorInterface
	:public vm::InstructionSet
{
public:
	explicit VirtualMachineTdlTranslatorInterface( const types::keymap<vm::Subroutine>* soubroutinemap_)
		:m_soubroutinemap(soubroutinemap_)
		,m_vm( new VirtualMachine())
	{}
	VirtualMachineTdlTranslatorInterface( const VirtualMachineTdlTranslatorInterface& o)
		:m_stateStack(o.m_stateStack)
		,m_unresolvedSubroutineCalls(o.m_unresolvedSubroutineCalls)
		,m_soubroutinemap(o.m_soubroutinemap)
		,m_calledSubroutines(o.m_calledSubroutines)
		,m_vm(o.m_vm)
	{}

	void begin_FOREACH( const std::string& selector);
	void end_FOREACH();

	void begin_DO_statement( const std::string& stm);
	void end_DO_statement();

	void begin_DO_subroutine( const std::string& name, const std::vector<std::string>& templateParamValues);
	void end_DO_subroutine();

	void push_ARGUMENT_PATH( const std::string& selector);

private:
	struct State
	{
		enum Id {None,OpenForeach,OpenBlock,OpenStatementCall,OpenSubroutineCall};
		Id id;
		vm::InstructionSet::ArgumentIndex value;

		State()
			:id(None),value(0){}
		State( const State& o)
			:id(o.id),value(o.value){}
		State( Id i, vm::InstructionSet::ArgumentIndex v)
			:id(i),value(v){}
	};

	std::vector<State> m_stateStack;
	std::vector<vm::InstructionSet::Address> m_unresolvedSubroutineCalls;
	const types::keymap<vm::Subroutine>* m_soubroutinemap;
	std::vector<vm::Subroutine> m_calledSubroutines;
	VirtualMachineR m_vm;
};

}}//namespace
#endif

