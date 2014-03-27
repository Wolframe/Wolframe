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
//\file virtualMachine.cpp
//\brief Implementation of the code executed as database input
#include "database/virtualMachineInstance.hpp"

using namespace _Wolframe;
using namespace _Wolframe::db;

vm::ValueTupleSetR VirtualMachineInstance::keptResult( ArgumentIndex idx) const
{
	const StackElement& top = m_stack.back();
	std::map<ArgumentIndex,vm::ValueTupleSetR>::const_iterator ki = top.m_resultMap.find( idx);
	if (ki == top.m_resultMap.end())
	{
		throw std::runtime_error("referencing named result not defined");
	}
	return ki->second;
}

vm::ValueTupleSetR VirtualMachineInstance::tupleSet( ArgumentIndex argidx) const
{
	if (argidx >= m_tuplesets.size()) throw std::runtime_error("tuple set reference out of bounds");
	return m_tuplesets.at( argidx);
}

const types::Variant& VirtualMachineInstance::constArgument( ArgumentIndex argidx) const
{
	if (argidx >= m_vm->constants.size()) throw std::runtime_error( "constant index out of bounds");
	return m_vm->constants.at( argidx);
}

const std::string& VirtualMachineInstance::nameArgument( ArgumentIndex argidx) const
{
	if (argidx >= m_vm->names.size()) throw std::runtime_error( "string index out of bounds");
	return m_vm->names.at( argidx);
}

VirtualMachineInstance::ArgumentIndex VirtualMachineInstance::columnIndex( const vm::ValueTupleSet* valueset, ArgumentIndex argidx) const
{
	if (argidx >= m_vm->names.size()) throw std::runtime_error( "name index out of bounds");
	if (!valueset) throw std::runtime_error( "illegal instruction: no value set selected for name access");
	return valueset->columnIndex( m_vm->names.at( argidx));
}

const types::Variant& VirtualMachineInstance::selectedArgument( ArgumentIndex argidx) const
{
	static const types::Variant null;
	const StackElement& top = m_stack.back();

	if (top.m_selectedSet->empty()) 
	{
		return null;
	}
	else if (argidx >= top.m_selectedSet->nofColumns())
	{
		throw std::runtime_error( "column index out of bounds");
	}
	else
	{
		return top.m_selectedSet->begin()->column( argidx);
	}
}

const types::Variant& VirtualMachineInstance::iteratorArgument( ArgumentIndex argidx) const
{
	const StackElement& top = m_stack.back();

	if (top.m_valueIter == top.m_valueEnd) throw std::runtime_error( "iterator element access out of bounds");
	return top.m_valueIter->column( argidx);
}

void VirtualMachineInstance::initValueIteraror( const vm::ValueTupleSetR& valueset)
{
	StackElement& top = m_stack.back();
	if (!valueset.get()) throw std::runtime_error( "opening iterator on undefined set");
	top.m_valueSet = valueset;
	top.m_valueIter = top.m_valueSet->begin();
	top.m_valueEnd = top.m_valueSet->end();
	m_cond = (top.m_valueIter != top.m_valueEnd);
}

void VirtualMachineInstance::initResult( const vm::ValueTupleSetR& resultset)
{
	StackElement& top = m_stack.back();

	// Check constraints on result:
	if (top.m_resultFlags.unique)
	{
		if (!resultset.get()) throw std::runtime_error("checking UNIQUE constraint on non existing result");
		resultset->checkConstraintUnique();
	}
	if (top.m_resultFlags.nonempty)
	{
		if (!resultset.get()) throw std::runtime_error("checking NONEMPTY constraint on non existing result");
		resultset->checkConstraintNonEmpty();
	}
	if (resultset.get())
	{
		// Append result in the Op_RESULT_INIT... context:
		if (top.m_resultFlags.touched())
		{
			top.m_lastResult->append( *resultset);
		}
		else
		{
			//... this is the first result
			top.m_lastResult = resultset;
		}
	}
}

vm::ValueTupleSetR VirtualMachineInstance::fetchDatabaseResult()
{
	if (m_db_stm->hasResult()) return vm::ValueTupleSetR();

	std::size_t ii,nofColumns = m_db_stm->nofColumns();
	if (m_db_stm->nofColumns() == 0) return vm::ValueTupleSetR();

	std::vector<std::string> colNames;
	for (ii=0; ii<nofColumns; ++ii)
	{
		const char* cn = m_db_stm->columnName( ii+1);
		colNames.push_back( cn?cn:"");
	}
	vm::ValueTupleSetR rt( new vm::ValueTupleSet( colNames));
	if (m_db_stm->hasResult())
	{
		do
		{
			std::vector<types::VariantConst> col;
			for (ii=0; ii<nofColumns; ++ii)
			{
				col.push_back( m_db_stm->get( ii+1));
			}
			rt->push( col);
		}
		while (m_db_stm->next());
	}
	return rt;
}

bool VirtualMachineInstance::execute()
{
	if (!m_vm || !m_db_stm || m_stack.empty()) return false;

	for(;;)
	{
		// Define abbreviation for the current state:
		StackElement& top = m_stack.back();

		// Evaluate conditional:
		Instruction instr = m_program.get( m_ip);
		switch (condCode( instr))
		{
			case Co_ALWAYS:
				break;
			case Co_IF_COND:
				if (m_cond) break;
				++m_ip;
				continue;
			case Co_NOT_IF_COND:
				if (!m_cond) break;
				++m_ip;
				continue;
		};
		ArgumentIndex argidx = VirtualMachine::argumentIndex( instr);

		switch (opCode( instr))
		{
			/*Control Flow Instructions:*/
			case Op_EXIT:
				return false;
			case Op_RETURN:
			{
				m_ip = top.m_return_ip;				//... set return address
				vm::ValueTupleSetR lastResult = top.m_lastResult;
				m_stack.pop_back();				//... destroy executed subroutine state
				if (m_stack.empty()) return true;		//... successful termination of program
				initResult( lastResult);
				break;
			}
			case Op_GOTO_ABSOLUTE:
				if (argidx > m_program.size()) throw std::runtime_error( "illegal goto instruction argument");
				m_ip = argidx;
				break;
			case Op_GOTO_SYMBOLIC:
				m_ip = m_vm->symboltab.getAddress( argidx);
				if (m_ip > m_program.size()) throw std::runtime_error( "illegal goto instruction argument");
				break;


			/*Print Instructions:*/
			case Op_PRINT_CONST:
				m_output.addValue( constArgument( argidx));
				++m_ip;
				break;
			case Op_PRINT_SEL_NAM:
				argidx = columnIndex( top.m_selectedSet.get(), argidx);
				m_program[ m_ip] = InstructionSet::instruction( 
					condCode( instr), Op_PRINT_SEL_IDX, argidx);
				//... rewrite program instruction in local copy to use column index instead of column name in the next iteration
				/*no break here!*/
			case Op_PRINT_SEL_IDX:
				m_output.addValue( selectedArgument( argidx));
				++m_ip;
				break;
			case Op_PRINT_ITR_NAM:
				argidx = columnIndex( top.m_valueSet.get(), argidx);
				m_program[ m_ip] = InstructionSet::instruction( 
					condCode( instr), Op_PRINT_ITR_IDX, argidx);
				//... rewrite program instruction in local copy to use column index instead of column name in the next iteration
				/*no break here!*/
			case Op_PRINT_ITR_IDX:
				m_output.addValue( iteratorArgument( argidx));
				++m_ip;
				break;
			case Op_PRINT_OPEN:
				m_output.add( vm::Output::Element( vm::Output::Element::Open, nameArgument( argidx)));
				++m_ip;
				break;
			case Op_PRINT_CLOSE:
				m_output.add( vm::Output::Element( vm::Output::Element::Close));
				++m_ip;
				break;


			/*Assignment Instructions:*/
			case Op_KEEP_RESULT:
				top.m_resultMap[ argidx] = top.m_lastResult;
				++m_ip;
				break;
			case Op_SELECT_PARAMETER:
				top.m_selectedSet = top.m_parameter;
				++m_ip;
				break;
			case Op_SELECT_LAST_RESULT:
				top.m_selectedSet = top.m_lastResult;
				if (top.m_selectedSet->size() > 1) throw std::runtime_error( "ambiguous value reference");
				++m_ip;
				break;
			case Op_SELECT_KEPT_RESULT:
				top.m_selectedSet = keptResult( argidx);
				if (top.m_selectedSet->size() > 1) throw std::runtime_error( "ambiguous value reference");
				++m_ip;
				break;


			/*Iterator Instructions:*/
			case Op_OPEN_ITER_LAST_RESULT:
				initValueIteraror( top.m_lastResult);
				++m_ip;
				break;

			case Op_OPEN_ITER_KEPT_RESULT:
				initValueIteraror( keptResult( argidx));
				++m_ip;
				break;

			case Op_OPEN_ITER_PATH:
				throw std::runtime_error("illegal instruction in this state: addressing unresolved path expression");

			case Op_OPEN_ITER_TUPLESET:
				initValueIteraror( tupleSet( argidx));
				++m_ip;
				break;

			case Op_NEXT:
				if (top.m_valueIter == top.m_valueEnd)
				{
					m_cond = false;
				}
				else
				{
					++top.m_valueIter;
					m_cond = true;
				}
				++m_ip;
				break;


			/*Subroutine Call Instructions:*/
			case Op_SUB_FRAME_OPEN:
				if (argidx >= m_vm->signatures.size()) throw std::runtime_error( "suroutine signatiure reference out of bounds");
				m_subroutine_frame.init( m_vm->signatures.at( argidx));
				++m_ip;
				break;
			case Op_SUB_ARG_CONST:
				m_subroutine_frame.push( constArgument( argidx));
				++m_ip;
				break;
			case Op_SUB_ARG_SEL_NAM:
				argidx = columnIndex( top.m_selectedSet.get(), argidx);
				m_program[ m_ip] = InstructionSet::instruction( 
					condCode( instr), Op_SUB_ARG_SEL_IDX, argidx);
				//... rewrite program instruction in local copy to use column index instead of column name in the next iteration
				/*no break here!*/
			case Op_SUB_ARG_SEL_IDX:
				m_subroutine_frame.push( selectedArgument( argidx));
				++m_ip;
				break;
			case Op_SUB_ARG_ITR_NAM:
				argidx = columnIndex( top.m_valueSet.get(), argidx);
				m_program[ m_ip] = InstructionSet::instruction( 
					condCode( instr), Op_SUB_ARG_ITR_IDX, argidx);
				//... rewrite program instruction in local copy to use column index instead of column name in the next iteration
				/*no break here!*/
			case Op_SUB_ARG_ITR_IDX:
				m_subroutine_frame.push( iteratorArgument( argidx));
				++m_ip;
				break;
			case Op_SUB_FRAME_CLOSE:
				++m_ip;
				m_stack.push_back( StackElement( m_ip + 1/*skip following GOTO*/, m_subroutine_frame.getParameters()));
				break;


			/*Database Instructions:*/
			case Op_STM_START:
				top.m_bindidx = 0;
				if (!m_db_stm->start( nameArgument( argidx)))
				{
					throw std::runtime_error( std::string("failed to start database statement '") + nameArgument( argidx) + "'");
				}
				++m_ip;
				break;
			case Op_STM_BIND_CONST:
				if (!m_db_stm->bind( ++top.m_bindidx, constArgument( argidx)))
				{
					throw std::runtime_error( std::string("failed to bind constant parameter [") + boost::lexical_cast<std::string>(top.m_bindidx) + "] '" + constArgument( argidx).tostring() + "'");
				}
				++m_ip;
				break;
			case Op_STM_BIND_SEL_NAM:
				argidx = columnIndex( top.m_selectedSet.get(), argidx);
				m_program[ m_ip] = InstructionSet::instruction( 
					condCode( instr), Op_STM_BIND_SEL_IDX, argidx);
				//... rewrite program instruction in local copy to use column index instead of column name in the next iteration
				/*no break here!*/

				/*no break here!*/
			case Op_STM_BIND_SEL_IDX:
				if (!m_db_stm->bind( ++top.m_bindidx, selectedArgument( argidx)))
				{
					throw std::runtime_error( std::string("failed to bind parameter [") + boost::lexical_cast<std::string>(top.m_bindidx) + "] '" + selectedArgument( argidx).tostring() + "'");
				}
				++m_ip;
				break;
			case Op_STM_BIND_ITR_NAM:
				argidx = columnIndex( top.m_valueSet.get(), argidx);
				m_program[ m_ip] = InstructionSet::instruction( 
					condCode( instr), Op_STM_BIND_ITR_IDX, argidx);
				//... rewrite program instruction in local copy to use column index instead of column name in the next iteration
				/*no break here!*/
			case Op_STM_BIND_ITR_IDX:
				if (!m_db_stm->bind( ++top.m_bindidx, iteratorArgument( argidx)))
				{
					throw std::runtime_error( std::string("failed to bind parameter [") + boost::lexical_cast<std::string>(top.m_bindidx) + "] '" + iteratorArgument( argidx).tostring() + "'");
				}
				++m_ip;
				break;
			case Op_STM_EXEC:
				if (!m_db_stm->execute())
				{
					throw std::runtime_error( "failed to execute database statement");
				}
				if (m_db_stm->hasResult())
				{
					initResult( fetchDatabaseResult());
				}
				top.m_bindidx = 0;
				++m_ip;
				break;


			/*Collect Results and Constraints:*/
			case Op_RESULT_INIT:
				top.m_resultFlags.init( false, false);
				++m_ip;
				break;
			case Op_RESULT_INIT_UNIQUE:
				top.m_resultFlags.init( true, false);
				++m_ip;
				break;
			case Op_RESULT_INIT_NONEMPTY:
				top.m_resultFlags.init( false, true);
				++m_ip;
				break;
			case Op_RESULT_INIT_NONEMPTY_UNIQUE:
				top.m_resultFlags.init( true, true);
				++m_ip;
				break;


			/*Others:*/
			case Op_NOP:
				++m_ip;
				break;
		}
	}
}
