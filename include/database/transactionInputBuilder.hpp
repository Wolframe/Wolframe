/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
///\brief Interface to build a transaction input object
///\file database/transactionInputBuilder.hpp
#ifndef _DATABASE_TRANSACTION_INPUT_BUILDER_HPP_INCLUDED
#define _DATABASE_TRANSACTION_INPUT_BUILDER_HPP_INCLUDED
#include "database/transactionObjects.hpp"
#include <string>
#include <cstdlib>

namespace _Wolframe {
namespace db {

///\class TransactionInputBuilder
///\brief Builder of transaction input object
class TransactionInputBuilder
	:public TransactionInput
{
public:
	TransactionInputBuilder(){}
	virtual ~TransactionInputBuilder(){}

	///\brief Start new command statement
	///\param[in] stmname name of prepared statement
	bool startCommand( const std::string& stmname)
	{
		cmd.push_back( Command( stmname));
	}

	///\brief Bind parameter value on current command statement
	bool bindCommandArgAsValue( const std::string value)
	{
		if (cmd.empty()) throw std::logic_error( "bind called with no command defined");
		cmd.back().push_back( Command::Element( value));
	}

	///\brief Bind parameter value on current command statement
	bool bindCommandArgAsResultReference( std::size_t resultref)
	{
		if (cmd.empty()) throw std::logic_error( "bind called with no command defined");
		cmd.back().push_back( Command::Element( resultref));
	}
};

}}//namespace
#endif

