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
///\brief Definition of transaction input built from TDL
///\file tdlTransactionInput.hpp
#ifndef _DATABASE_TDL_TRANSACTION_INPUT_HPP_INCLUDED
#define _DATABASE_TDL_TRANSACTION_INPUT_HPP_INCLUDED
#include "database/vmTransactionInput.hpp"
#include "utils/printFormats.hpp"
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace db {

///\brief Forward decarations
namespace vm {
class InputStructure;
}

///\class TdlTransactionInput
///\brief Input of a transaction
class TdlTransactionInput
	:public VmTransactionInput
{
public:
	///\brief Constructor
	TdlTransactionInput(){}
	///\brief Copy constructor
	TdlTransactionInput( const TdlTransactionInput& o)
		:VmTransactionInput(o){}
	TdlTransactionInput( const vm::Program& p, const vm::InputStructure& input);
};

}}//namespace
#endif
