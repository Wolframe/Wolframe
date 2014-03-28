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
//\file database/virtualMachineInstance.hpp
//\brief Interface for an instance of a virtual machine defining database transactions
#ifndef _DATABASE_VIRTUAL_MACHINE_INSTANCE_HPP_INCLUDED
#define _DATABASE_VIRTUAL_MACHINE_INSTANCE_HPP_INCLUDED
#include "database/virtualMachine.hpp"
#include "database/vm/instructionSet.hpp"
#include "database/vm/valueTupleSet.hpp"
#include "database/vm/symbolTable.hpp"
#include "database/vm/program.hpp"
#include "database/vm/output.hpp"
#include "database/transactionExecStatemachine.hpp"
#include "types/variant.hpp"
#include <string>
#include <vector>

namespace _Wolframe {
namespace db {

class VirtualMachineInstance
	:public vm::InstructionSet
{
public:
	VirtualMachineInstance()
		:m_vm(0),m_db_stm(0),m_ip(0),m_cond(false)
	{}
	VirtualMachineInstance( const VirtualMachineInstance& o)
		:m_vm(o.m_vm)
		,m_db_stm(o.m_db_stm)
		,m_ip(o.m_ip)
		,m_cond(o.m_cond)
		,m_stack(o.m_stack)
		,m_output(o.m_output)
		,m_program(o.m_program)
		,m_tuplesets(o.m_tuplesets)
	{}
	VirtualMachineInstance( const VirtualMachine* vm_, const std::vector<vm::ValueTupleSetR>& tuplesets_, TransactionExecStatemachine* db_stm_)
		:m_vm(vm_)
		,m_db_stm(db_stm_)
		,m_ip(0)
		,m_cond(false)
		,m_program(m_vm->program)
		,m_tuplesets(tuplesets_)
	{
		m_stack.push_back( StackElement());
	}

	bool execute();

private:
	struct ResultFlags
	{
		bool is_first;		//< true, if there has been no result set added to the set (result is created)
		bool unique;		//< UNIQUE flag
		bool nonempty;		//< NONEMPTY flag

		ResultFlags( const ResultFlags& o)
			:is_first(o.is_first),unique(o.unique),nonempty(o.nonempty){}
		ResultFlags( bool unique_=false, bool nonempty_=false)
			:is_first(true),unique(unique_),nonempty(nonempty_){}
		void clear()
		{
			is_first = true;
			unique = false;
			nonempty = false;
		}
		void init( bool unique_, bool nonempty_)
		{
			is_first = true;
			unique = unique_;
			nonempty = nonempty_;
		}
		bool touched()
		{
			bool rt = !is_first;
			is_first = false;
			return rt;
		}
	};

	struct StackElement
	{
		StackElement()
			:m_return_ip(0)
			,m_bindidx(0)
		{}
		StackElement( Address return_ip_, const vm::ValueTupleSetR& parameter_)
			:m_return_ip(return_ip_)
			,m_bindidx(0)
			,m_parameter(parameter_)
		{}
		StackElement( const StackElement& o)
			:m_return_ip(o.m_return_ip)
			,m_bindidx(o.m_bindidx)
			,m_lastResult(o.m_lastResult)
			,m_resultMap(o.m_resultMap)
			,m_selectedSet(o.m_selectedSet)
			,m_valueSet(o.m_valueSet)
			,m_valueIter(o.m_valueIter)
			,m_valueEnd(o.m_valueEnd)
		{}

		Address m_return_ip;						//< IP to set o RETURN
		std::size_t m_bindidx;						//< current bind index
		vm::ValueTupleSetR m_parameter;					//< parameter structure
		vm::ValueTupleSetR m_lastResult;				//< last call result
		ResultFlags m_resultFlags;					//< conditions to be meet for each element of the next result
		std::map<ArgumentIndex,vm::ValueTupleSetR> m_resultMap;		//< KEEP result map
		vm::ValueTupleSetR m_selectedSet;				//< selected value set
		vm::ValueTupleSetR m_valueSet;					//< currently iterated value set
		vm::ValueTupleSet::const_iterator m_valueIter;			//< iterator on value set
		vm::ValueTupleSet::const_iterator m_valueEnd;			//< end of value set
	};

private:
	vm::ValueTupleSetR keptResult( ArgumentIndex idx) const;
	vm::ValueTupleSetR tupleSet( ArgumentIndex idx) const;
	const types::Variant& constArgument( ArgumentIndex idx) const;
	const std::string& statementArgument( ArgumentIndex argidx) const;
	ArgumentIndex columnIndex( const vm::ValueTupleSet* valueset, ArgumentIndex nameidx) const;
	const types::Variant& selectedArgument( ArgumentIndex idx) const;
	const types::Variant& iteratorArgument( ArgumentIndex idx) const;
	void initValueIteraror( const vm::ValueTupleSetR& valueset);
	void initResult( const vm::ValueTupleSetR& resultset);
	vm::ValueTupleSetR fetchDatabaseResult();

private:
	const VirtualMachine* m_vm;			//< virtual machine reference
	TransactionExecStatemachine* m_db_stm;		//< engine to process database instructions
	Address m_ip;					//< instruction pointer
	vm::SubroutineFrame m_subroutine_frame;		//< prepared subroutine call parameter structure
	bool m_cond;					//< current condition flag for conditional execution ('InstructionSet::CondCode')
	std::vector<StackElement> m_stack;		//< execution stack
	vm::Output m_output;				//< output
	vm::Program m_program;				//< program copy
	std::vector<vm::ValueTupleSetR> m_tuplesets;	//< values from path expressions
};

}}//namespace
#endif

