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
///\brief Definition of transaction input
///\file database/vmTransactionInput.hpp
#ifndef _DATABASE_VM_TRANSACTION_INPUT_HPP_INCLUDED
#define _DATABASE_VM_TRANSACTION_INPUT_HPP_INCLUDED
#include "database/vm/program.hpp"
#include "utils/printFormats.hpp"
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace db {

namespace tf {
//\brief Forward declaration
class InputStructure;
}//namespace tf


///\class TransactionInput
///\brief Input of a transaction
class VmTransactionInput
{
public:
	///\brief Constructor
	VmTransactionInput(){}
	///\brief Copy constructor
	VmTransactionInput( const VmTransactionInput& o)
		:m_program(o.m_program){}
	VmTransactionInput( const vm::Program& p, const tf::InputStructure& input);

	///\brief Return the input as readable serialization
	std::string tostring() const;

	void print( std::ostream& out) const;

	const vm::Program& program() const	{return m_program;}

private:
	vm::Program m_program;
};

typedef boost::shared_ptr<VmTransactionInput> VmTransactionInputR;

}}//namespace
#endif
