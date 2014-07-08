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
/// \brief Definition of transaction output
/// \file database/vmTransactionOutput.hpp
#ifndef _DATABASE_VM_TRANSACTION_OUTPUT_HPP_INCLUDED
#define _DATABASE_VM_TRANSACTION_OUTPUT_HPP_INCLUDED
#include "database/vm/output.hpp"
#include "filter/typedfilter.hpp"
#include "types/variant.hpp"
#include <string>
#include <iostream>

namespace _Wolframe {
namespace utils {
/// \brief Forward declaration
struct PrintFormat;
}//namespace utils

namespace db {

/// \class TransactionOutput
/// \brief Output of a transaction
class VmTransactionOutput
{
public:
	/// \brief Constructor
	VmTransactionOutput(){}
	/// \brief Copy constructor
	VmTransactionOutput( const VmTransactionOutput& o)
		:m_impl(o.m_impl){}
	/// \brief Copy constructor
	VmTransactionOutput( const vm::OutputR& output)
		:m_impl(output){}

	/// \brief Return the result as readable serialization
	std::string tostring( const utils::PrintFormat* pformat=0) const;

	/// \brief Print this transaction output
	void print( std::ostream& out, const utils::PrintFormat* pformat=0) const;

	/// \brief Evaluate if the underlying database is case sensitive
	/// \return true if yes, false else
	bool isCaseSensitive() const
	{
		return false;
	}

	/// \brief Get the result of one execution block
	/// \param[in] index index of the execution block (0=transaction result, 1..=input for audit operations)
	langbind::TypedInputFilterR get( std::size_t index=0) const;

private:
	vm::OutputR m_impl;
};

}}//namespace
#endif

