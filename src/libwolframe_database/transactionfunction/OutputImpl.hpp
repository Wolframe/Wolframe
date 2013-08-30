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
		ResultElement::Type m_type;
		std::size_t m_cnt;
		ResultStructure::const_iterator m_structitr;
		std::size_t m_functionidx;

		StackElement( ResultElement::Type type_, ResultStructure::const_iterator structitr_, std::size_t functionidx_)
			:m_type(type_),m_cnt(0),m_structitr(structitr_),m_functionidx(functionidx_){}
		StackElement( const StackElement& o)
			:m_type(o.m_type),m_cnt(o.m_cnt),m_structitr(o.m_structitr),m_functionidx(o.m_functionidx){}
	};

	enum {StateColumnOpenTag=0, StateColumnValue=1, StateColumnCloseTag=2,StateColumnEndGroup=3};
	enum {StateIndexCloseTag=0, StateIndexNext=1};
	int m_valuestate;
	int m_colidx;
	int m_colend;
	bool m_endofoutput;
	bool m_started;
	std::string m_group;
	ResultStructureR m_resultstruct;
	ResultStructure::const_iterator m_structitr;
	ResultStructure::const_iterator m_structend;
	std::vector<StackElement> m_stack;
	db::TransactionOutput::result_const_iterator m_resitr;
	db::TransactionOutput::result_const_iterator m_resend;
	std::vector<db::TransactionOutput::CommandResult::Row>::const_iterator m_rowitr;
	std::vector<db::TransactionOutput::CommandResult::Row>::const_iterator m_rowend;
	db::TransactionOutputR m_data;

	Impl( const ResultStructureR& resultstruct_, const db::TransactionOutputR& data_);
	Impl( const Impl& o);

	void resetIterator();

	static std::string getResultGroup( const std::string& columnName);
	static bool hasResultGroup( const std::string& columnName);

	bool endOfGroup();

	bool getNext( ElementType& type, types::VariantConst& element, bool doSerializeWithIndices);
};

}}//namespace
#endif

