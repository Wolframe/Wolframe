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
/// \file database/vm/programInstance.hpp
/// \brief Interface for state of a program executing database transactions
#ifndef _DATABASE_VM_PROGRAM_INSTANCE_HPP_INCLUDED
#define _DATABASE_VM_PROGRAM_INSTANCE_HPP_INCLUDED
#include "database/vm/programImage.hpp"
#include "database/vm/instructionSet.hpp"
#include "database/vm/valueTupleSet.hpp"
#include "database/vm/output.hpp"
#include "database/vm/subroutineFrame.hpp"
#include "database/transactionExecStatemachine.hpp"
#include "database/databaseError.hpp"
#include "types/variant.hpp"
#include <string>
#include <vector>
#include <map>

namespace _Wolframe {
namespace db {
namespace vm {
/// \brief Forward Declaration
class Program;

/// \class ProgramInstance
/// \brief Instance of a program with statevariables
class ProgramInstance
	:public InstructionSet
{
public:
	/// \class LogTraceContext
	/// \brief Callback context structure for logging VM instructions in transaction execution
	struct LogTraceContext;
	typedef void (*LogTraceCallBack)( const LogTraceContext* prgcontext, unsigned int ip_);

public:
	/// \brief Default constructor
	ProgramInstance()
		:m_db_stm(0)
		,m_ip(0)
		,m_cond(false)
		,m_logTraceCallBack(0)
		,m_logTraceContext(0)
	{}
	/// \brief Copy constructor
	ProgramInstance( const ProgramInstance& o)
		:m_program(o.m_program)
		,m_db_stm(o.m_db_stm)
		,m_ip(o.m_ip)
		,m_cond(o.m_cond)
		,m_stack(o.m_stack)
		,m_output(o.m_output)
		,m_logTraceCallBack(o.m_logTraceCallBack)
		,m_logTraceContext(o.m_logTraceContext)
	{}

	/// \brief Constructor
	ProgramInstance( const ProgramImage& program_, TransactionExecStatemachine* db_stm_, LogTraceCallBack logTraceCallBack_=0, const LogTraceContext* logTraceContext_=0);

	/// \brief Execute the program
	bool execute();
	/// \brief Get the output of the executed program
	const OutputR& output() const			{return m_output;}
	/// \brief Get the last error of the executed program
	const DatabaseError* lastError() const		{return &m_lastError;}
	/// \brief Get the current IP of program execution (instruction pointer)
	unsigned int ip() const				{return m_ip;}

private:
	/// \class ResultFlags
	/// \brief Flags defined in the database command between DO and the statement (NONEMPTY,UNIQUE)
	struct ResultFlags
	{
		bool is_first;		///< true, if there has been no result set added to the set (result is created)
		bool unique;		///< UNIQUE flag
		bool nonempty;		///< NONEMPTY flag

		/// \brief Copy constructor
		ResultFlags( const ResultFlags& o)
			:is_first(o.is_first),unique(o.unique),nonempty(o.nonempty){}
		/// \brief Constructor
		ResultFlags( bool unique_=false, bool nonempty_=false)
			:is_first(true),unique(unique_),nonempty(nonempty_){}
		/// \brief Reset flags
		void clear()
		{
			is_first = true;
			unique = false;
			nonempty = false;
		}
		/// \brief Initialize flags
		void init( bool unique_, bool nonempty_)
		{
			is_first = true;
			unique = unique_;
			nonempty = nonempty_;
		}
		/// \brief Get and set of the first result flag
		bool touched()
		{
			bool rt = !is_first;
			is_first = false;
			return rt;
		}
	};

	/// \class StackElement
	/// \brief State stack element of the virtual machine executing a transaction
	struct StackElement
	{
		/// \brief Default constructor
		StackElement()
			:m_return_ip(0)
			,m_bindidx(0)
			,m_hintidx(0)
		{}
		/// \brief Constructor
		StackElement( Address return_ip_, const ValueTupleSetR& parameter_)
			:m_return_ip(return_ip_)
			,m_bindidx(0)
			,m_hintidx(0)
			,m_parameter(parameter_)
		{}
		/// \brief Copy constructor
		StackElement( const StackElement& o)
			:m_return_ip(o.m_return_ip)
			,m_bindidx(o.m_bindidx)
			,m_hintidx(o.m_hintidx)
			,m_parameter(o.m_parameter)
			,m_lastResult(o.m_lastResult)
			,m_resultFlags(o.m_resultFlags)
			,m_resultMap(o.m_resultMap)
			,m_selectedSet(o.m_selectedSet)
			,m_valueSet(o.m_valueSet)
			,m_valueIter(o.m_valueIter)
			,m_valueEnd(o.m_valueEnd)
		{}

		Address m_return_ip;					///< IP to set o RETURN
		std::size_t m_bindidx;					///< current bind index
		ArgumentIndex m_hintidx;				///< current database statement hint index
		ValueTupleSetR m_parameter;				///< parameter structure
		ValueTupleSetR m_lastResult;				///< last call result
		ResultFlags m_resultFlags;				///< conditions to be meet for each element of the next result
		std::map<ArgumentIndex,ValueTupleSetR> m_resultMap;	///< KEEP result map
		ValueTupleSetR m_selectedSet;				///< selected value set
		ValueTupleSetR m_valueSet;				///< currently iterated value set
		ValueTupleSet::const_iterator m_valueIter;		///< iterator on value set
		ValueTupleSet::const_iterator m_valueEnd;		///< end of value set
	};

private:
	ValueTupleSetR keptResult( ArgumentIndex idx) const;
	ValueTupleSetR tupleSet( ArgumentIndex idx) const;
	const types::Variant& constArgument( ArgumentIndex idx) const;
	const std::string& statementArgument( ArgumentIndex argidx) const;
	const std::string& tagnameArgument( ArgumentIndex argidx) const;
	types::Variant loopcntArgument() const;
	ArgumentIndex columnIndex( const ValueTupleSet* valueset, ArgumentIndex nameidx) const;
	const types::Variant& selectedArgument( ArgumentIndex idx) const;
	const types::Variant& iteratorArgument( ArgumentIndex idx) const;
	void initValueIteraror( const ValueTupleSetR& valueset);
	void initResult( const ValueTupleSetR& resultset);
	void printIteratorColumn();
	ValueTupleSetR fetchDatabaseResult( std::size_t nofColumns);
	void setDatabaseError();

private:
	ProgramImage m_program;				///< image of program to execute
	TransactionExecStatemachine* m_db_stm;		///< engine to process database instructions
	Address m_ip;					///< instruction pointer
	SubroutineFrame m_subroutine_frame;		///< prepared subroutine call parameter structure
	bool m_cond;					///< current condition flag for conditional execution ('InstructionSet::CondCode')
	std::vector<StackElement> m_stack;		///< execution stack
	OutputR m_output;				///< output
	DatabaseError m_lastError;			///< last database error reported
	LogTraceCallBack m_logTraceCallBack;		///< NULL or callback procedure for logging execution that is be called after every instruction executed
	const LogTraceContext* m_logTraceContext;	///< Context for 'm_logTraceCallBack'
};

}}}//namespace
#endif

