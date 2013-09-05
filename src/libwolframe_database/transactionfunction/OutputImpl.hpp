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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\brief Internal interface for transaction function output filter
///\file transactionfunction/OutputImpl.hpp
#ifndef _DATABASE_TRANSACTION_FUNCTION_OUTPUT_IMPL_HPP_INCLUDED
#define _DATABASE_TRANSACTION_FUNCTION_OUTPUT_IMPL_HPP_INCLUDED
#include "database/transactionFunction.hpp"
#include "transactionfunction/OutputResultStructure.hpp"
#include <string>
#include <map>
#include <vector>

namespace _Wolframe {
namespace db {

struct TransactionFunctionOutput::Impl
{
	struct StackElement
	{
		ResultElement::Type m_type;				//< type of element expected in end of the section marked by this
		std::size_t m_cnt;					//< count of elements in this section (loop)
		ResultStructure::const_iterator m_structitr;		//< start of the structure of this section (for jump back in loop)
		std::size_t m_functionidx;				//< functionidx of current element that has to match for jumping back
		TransactionOutput::result_const_iterator m_resitr;	//< m_resitr at loop begin

		StackElement( ResultElement::Type type_, ResultStructure::const_iterator structitr_, std::size_t functionidx_, TransactionOutput::result_const_iterator resitr_)
			:m_type(type_),m_cnt(0),m_structitr(structitr_),m_functionidx(functionidx_),m_resitr(resitr_){}
		StackElement( const StackElement& o)
			:m_type(o.m_type),m_cnt(o.m_cnt),m_structitr(o.m_structitr),m_functionidx(o.m_functionidx),m_resitr(o.m_resitr){}
	};

	enum {StateColumnOpenTag=0, StateColumnValue=1, StateColumnCloseTag=2,StateColumnEndGroup=3};
	enum {StateIndexCloseTag=0, StateIndexNext=1};
	int m_valuestate;								//< sub statemachine state for StateColumn and StateIndex
	int m_colidx;									//< iterator on current result columns
	int m_colend;									//< end of result columns (nof columns)
	bool m_endofoutput;								//< flag set when final Close has been emited by last call of 'getNext'
	bool m_started;									//< getNext called at least once (flag for logging things on first call of get next)
	bool m_nextresult;								//< result iterator got to the following result
	std::string m_group;								//< group tag for output AS with one slash
	ResultStructureR m_resultstruct;						//< result structure
	ResultStructure::const_iterator m_structitr;					//< iterator on output structure
	ResultStructure::const_iterator m_structend;					//< end of output structure
	std::vector<StackElement> m_stack;						//< stack for loop on nested structures
	TransactionOutput::result_const_iterator m_resitr;				//< currently visited result
	TransactionOutput::result_const_iterator m_resend;				//< end of results
	std::vector<TransactionOutput::CommandResult::Row>::const_iterator m_rowitr;	//< currently visited result row
	std::vector<TransactionOutput::CommandResult::Row>::const_iterator m_rowend;	//< end of currently visited result
	TransactionOutputR m_data;

	Impl( const ResultStructureR& resultstruct_, const TransactionOutputR& data_);
	Impl( const Impl& o);

	void resetIterator();

	static std::string getResultGroup( const std::string& columnName);
	static bool hasResultGroup( const std::string& columnName);

	bool endOfGroup();
	TransactionOutput::result_const_iterator endOfOperation();

	bool getNext( ElementType& type, types::VariantConst& element, bool doSerializeWithIndices);

private:
	void nextResult();
	bool skipResult();
	bool skipStruct( int start, int end);
};

}}//namespace
#endif

