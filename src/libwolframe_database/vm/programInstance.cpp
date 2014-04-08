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
//\file vm/programInstance.cpp
//\brief Implementation of the code executed as database input
#include "database/vm/programInstance.hpp"

using namespace _Wolframe;
using namespace _Wolframe::db;
using namespace _Wolframe::db::vm;

ValueTupleSetR ProgramInstance::keptResult( ArgumentIndex idx) const
{
	const StackElement& top = m_stack.back();
	std::map<ArgumentIndex,ValueTupleSetR>::const_iterator ki = top.m_resultMap.find( idx);
	if (ki == top.m_resultMap.end())
	{
		throw std::runtime_error("referencing named result not defined");
	}
	return ki->second;
}

ValueTupleSetR ProgramInstance::tupleSet( ArgumentIndex argidx) const
{
	if (argidx >= m_program->tuplesets.size()) throw std::runtime_error("tuple set reference out of bounds");
	return m_program->tuplesets.at( argidx);
}

const types::Variant& ProgramInstance::constArgument( ArgumentIndex argidx) const
{
	if (argidx >= m_program->constants.size()) throw std::runtime_error( "constant index out of bounds");
	return m_program->constants.at( argidx);
}

const std::string& ProgramInstance::statementArgument( ArgumentIndex argidx) const
{
	if (argidx >= m_program->statements.size()) throw std::runtime_error( "string index out of bounds");
	return m_program->statements.at( argidx);
}

ProgramInstance::ArgumentIndex ProgramInstance::columnIndex( const ValueTupleSet* valueset, ArgumentIndex argidx) const
{
	if (!valueset) throw std::runtime_error( "illegal instruction: no value set selected for name access");
	return valueset->columnIndex( m_program->colnametab.getName( argidx));
}

const types::Variant& ProgramInstance::selectedArgument( ArgumentIndex argidx) const
{
	static const types::Variant null;
	const StackElement& top = m_stack.back();

	if (top.m_selectedSet->empty()) 
	{
		return null;
	}
	return top.m_selectedSet->begin()->column( argidx);
}

const types::Variant& ProgramInstance::iteratorArgument( ArgumentIndex argidx) const
{
	const StackElement& top = m_stack.back();

	if (top.m_valueIter == top.m_valueEnd) throw std::runtime_error( "iterator element access out of bounds");
	return top.m_valueIter->column( argidx);
}

types::Variant ProgramInstance::loopcntArgument() const
{
	const StackElement& top = m_stack.back();
	if (!top.m_valueSet.get())
	{
		throw std::runtime_error( "referencing loop count (Variable '#') outside a FOREACH context");
	}
	else
	{
		return types::Variant( top.m_valueIter->index());
	}
}

void ProgramInstance::initValueIteraror( const ValueTupleSetR& valueset)
{
	StackElement& top = m_stack.back();
	if (!valueset.get()) throw std::runtime_error( "opening iterator on undefined set");
	top.m_valueSet = valueset;
	top.m_valueIter = top.m_valueSet->begin();
	top.m_valueEnd = top.m_valueSet->end();
	m_cond = (top.m_valueIter != top.m_valueEnd);
}

void ProgramInstance::printIteratorColumn()
{
	const StackElement& top = m_stack.back();
	m_cond = (top.m_valueIter != top.m_valueEnd);
	if (m_cond)
	{
		std::size_t idx=1,nofColumns = top.m_valueSet->nofColumns();
		for (; idx<=nofColumns; ++idx)
		{
			m_output->add( Output::Element( Output::Element::Open, top.m_valueSet->columnName( idx)));
			m_output->add( Output::Element( Output::Element::Value, top.m_valueIter->column( idx)));
			m_output->add( Output::Element( Output::Element::Close));
		}
	}
}

void ProgramInstance::initResult( const ValueTupleSetR& resultset)
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

ValueTupleSetR ProgramInstance::fetchDatabaseResult()
{
	if (!m_db_stm->hasResult()) throw std::logic_error("assertion failed: call fetchDatabaseResult() without result");

	std::size_t ii,nofColumns = m_db_stm->nofColumns();
	if (nofColumns == 0) return ValueTupleSetR();

	std::vector<std::string> colNames;
	for (ii=0; ii<nofColumns; ++ii)
	{
		const char* cn = m_db_stm->columnName( ii+1);
		colNames.push_back( cn?cn:"");
	}
	ValueTupleSetR rt( new ValueTupleSet( colNames));
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

void ProgramInstance::setDatabaseError()
{
	const DatabaseError* err = m_db_stm->getLastError();
	if (!err)
	{
		m_lastError = DatabaseError( "UNKNOWN", 0, "unknown error");
	}
	else
	{
		const StackElement& top = m_stack.back();
	
		m_lastError = *err;
		m_lastError.dbname = m_db_stm->databaseID();
		m_lastError.transaction = m_db_stm->transactionName();
		m_lastError.ip = m_ip;

		if (top.m_hintidx)
		{
			const char* hintstr = m_program->hinttab.findHint( top.m_hintidx, err->errorclass);
			if (hintstr)
			{
				m_lastError.errorhint = hintstr;
			}
		}
	}
}

bool ProgramInstance::execute()
{
	if (!m_program || !m_db_stm || m_stack.empty()) return false;

	for(;;)
	{
		// Define abbreviation for the current state:
		StackElement& top = m_stack.back();

		// Evaluate conditional:
		Instruction instr = m_code.get( m_ip);
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
		ArgumentIndex argidx = Program::argumentIndex( instr);

		switch (opCode( instr))
		{
			/*Control Flow Instructions:*/
			case Op_EXIT:
				m_lastError = DatabaseError( "VM", 0, "aborted program execution");
				return false;
			case Op_RETURN:
			{
				m_ip = top.m_return_ip;				//... set return address
				ValueTupleSetR lastResult = top.m_lastResult;
				m_stack.pop_back();				//... destroy executed subroutine state
				if (m_stack.empty()) return true;		//... successful termination of program
				initResult( lastResult);
				break;
			}
			case Op_GOTO:
				if (argidx > m_code.size()) throw std::runtime_error( "illegal goto instruction argument");
				m_ip = argidx;
				break;

			/*Print Instructions:*/
			case Op_OUTPUT_CONST:
				m_output->addValue( constArgument( argidx));
				++m_ip;
				break;
			case Op_OUTPUT_PATH:
				throw std::runtime_error("illegal instruction in this state: addressing unresolved path expression");
			case Op_OUTPUT_LOOPCNT:
				m_output->addValue( loopcntArgument());
				++m_ip;
				break;
			case Op_OUTPUT_SEL_NAM:
				argidx = columnIndex( top.m_selectedSet.get(), argidx);
				m_code[ m_ip] = InstructionSet::instruction( 
					condCode( instr), Op_OUTPUT_SEL_IDX, argidx);
				//... rewrite program instruction in local copy to use column index instead of column name in the next iteration
				/*no break here!*/
			case Op_OUTPUT_SEL_IDX:
				m_output->addValue( selectedArgument( argidx));
				++m_ip;
				break;
			case Op_OUTPUT_ITR_NAM:
				argidx = columnIndex( top.m_valueSet.get(), argidx);
				m_code[ m_ip] = InstructionSet::instruction( 
					condCode( instr), Op_OUTPUT_ITR_IDX, argidx);
				//... rewrite program instruction in local copy to use column index instead of column name in the next iteration
				/*no break here!*/
			case Op_OUTPUT_ITR_IDX:
				m_output->addValue( iteratorArgument( argidx));
				++m_ip;
				break;
			case Op_OUTPUT_ITR_COLUMN:
				printIteratorColumn();
				++m_ip;
				break;
			case Op_OUTPUT_OPEN:
				m_output->add( Output::Element( Output::Element::Open, m_program->tagnametab.getName( argidx)));
				++m_ip;
				break;
			case Op_OUTPUT_CLOSE:
				m_output->add( Output::Element( Output::Element::Close));
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
				if (!top.m_lastResult.get()) throw std::runtime_error( "selecting result set that is not defined");
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
					m_cond = (top.m_valueIter != top.m_valueEnd);
				}
				++m_ip;
				break;


			/*Subroutine Call Instructions:*/
			case Op_SUB_FRAME_OPEN:
				if (argidx >= m_program->signatures.size()) throw std::runtime_error( "suroutine signatiure reference out of bounds");
				m_subroutine_frame.init( m_program->signatures.at( argidx));
				++m_ip;
				break;
			case Op_SUB_ARG_CONST:
				m_subroutine_frame.push( constArgument( argidx));
				++m_ip;
				break;
			case Op_SUB_ARG_PATH:
				throw std::runtime_error("illegal instruction in this state: addressing unresolved path expression");
			case Op_SUB_ARG_LOOPCNT:
				m_subroutine_frame.push( loopcntArgument());
				++m_ip;
				break;
			case Op_SUB_ARG_SEL_NAM:
				argidx = columnIndex( top.m_selectedSet.get(), argidx);
				m_code[ m_ip] = InstructionSet::instruction( 
					condCode( instr), Op_SUB_ARG_SEL_IDX, argidx);
				//... rewrite program instruction in local copy to use column index instead of column name in the next iteration
				/*no break here!*/
			case Op_SUB_ARG_SEL_IDX:
				m_subroutine_frame.push( selectedArgument( argidx));
				++m_ip;
				break;
			case Op_SUB_ARG_ITR_NAM:
				argidx = columnIndex( top.m_valueSet.get(), argidx);
				m_code[ m_ip] = InstructionSet::instruction( 
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


			/*Database Statements:*/
			case Op_DBSTM_START:
				top.m_bindidx = 0;
				top.m_hintidx = 0;
				if (!m_db_stm->start( statementArgument( argidx)))
				{
					setDatabaseError();
					return false;
				}
				++m_ip;
				break;
			case Op_DBSTM_BIND_CONST:
				if (!m_db_stm->bind( ++top.m_bindidx, constArgument( argidx)))
				{
					setDatabaseError();
					return false;
				}
				++m_ip;
				break;
			case Op_DBSTM_BIND_PATH:
				throw std::runtime_error("illegal instruction in this state: addressing unresolved path expression");
			case Op_DBSTM_BIND_LOOPCNT:
				if (!m_db_stm->bind( ++top.m_bindidx, loopcntArgument()))
				{
					setDatabaseError();
					return false;
				}
				++m_ip;
				break;
			case Op_DBSTM_BIND_SEL_NAM:
				argidx = columnIndex( top.m_selectedSet.get(), argidx);
				m_code[ m_ip] = InstructionSet::instruction( 
					condCode( instr), Op_DBSTM_BIND_SEL_IDX, argidx);
				//... rewrite program instruction in local copy to use column index instead of column name in the next iteration
				/*no break here!*/

				/*no break here!*/
			case Op_DBSTM_BIND_SEL_IDX:
				if (!m_db_stm->bind( ++top.m_bindidx, selectedArgument( argidx)))
				{
					setDatabaseError();
					return false;
				}
				++m_ip;
				break;
			case Op_DBSTM_BIND_ITR_NAM:
				argidx = columnIndex( top.m_valueSet.get(), argidx);
				m_code[ m_ip] = InstructionSet::instruction( 
					condCode( instr), Op_DBSTM_BIND_ITR_IDX, argidx);
				//... rewrite program instruction in local copy to use column index instead of column name in the next iteration
				/*no break here!*/
			case Op_DBSTM_BIND_ITR_IDX:
				if (!m_db_stm->bind( ++top.m_bindidx, iteratorArgument( argidx)))
				{
					setDatabaseError();
					return false;
				}
				++m_ip;
				break;
			case Op_DBSTM_HINT:
				top.m_hintidx = argidx;
				++m_ip;
				break;
			case Op_DBSTM_EXEC:
				if (!m_db_stm->execute())
				{
					setDatabaseError();
					return false;
				}
				if (m_db_stm->hasResult())
				{
					initResult( fetchDatabaseResult());
				}
				top.m_bindidx = 0;
				top.m_hintidx = 0;
				++m_ip;
				break;


			/*Collect Results and Constraints:*/
			case Op_RESULT_SET_INIT:
				top.m_resultFlags.init( false, false);
				++m_ip;
				break;
			case Op_RESULT_CONSTRAINT_UNIQUE:
				top.m_resultFlags.unique = true;
				++m_ip;
				break;
			case Op_RESULT_CONSTRAINT_NONEMPTY:
				top.m_resultFlags.nonempty = true;
				++m_ip;
				break;


			/*Others:*/
			case Op_NOP:
				++m_ip;
				break;
		}
	}
}
