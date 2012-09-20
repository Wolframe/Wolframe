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
///\file tests/wolfilter/modules/transaction/echo/echo_transaction.cpp
///\brief Interface for a fake transaction handler for tests
#ifndef _Wolframe_TESTS_WOLFILTER_TRANSACTION_ECHO_HPP_INCLUDED
#define _Wolframe_TESTS_WOLFILTER_TRANSACTION_ECHO_HPP_INCLUDED
#include <string>
#include <vector>
#include "langbind/transactionFunction.hpp"
#include "protocol/ioblocks.hpp"

namespace _Wolframe {
namespace test {

class EchoTransactionResult
	:public langbind::TransactionFunction::Result
{
public:
	EchoTransactionResult(){}
	EchoTransactionResult( const EchoTransactionResult& o);

	virtual ~EchoTransactionResult(){}

	virtual bool getNext( ElementType& type, langbind::TypedFilterBase::Element& element);

private:
	friend class EchoTransactionInput;
	typedef std::pair< langbind::InputFilter::ElementType, std::string> Item;
	typedef std::vector<Item> ItemArray;
	ItemArray m_itemar;
	ItemArray::const_iterator m_itemitr;
};


class EchoTransactionInput
	:public langbind::TransactionFunction::Input
{
public:
	EchoTransactionInput(){}
	EchoTransactionInput( const EchoTransactionInput& o);
	virtual ~EchoTransactionInput(){}

	virtual bool print( ElementType type, const Element& element);

private:
	friend class EchoTransactionFunction;
	EchoTransactionResult m_result;
};


class EchoTransactionFunction
	:public langbind::TransactionFunction
{
public:
	EchoTransactionFunction( const TransactionFunction& o);
	EchoTransactionFunction(){}
	virtual ~EchoTransactionFunction(){}

	virtual langbind::TransactionFunction::InputR getInput() const;

	virtual langbind::TransactionFunction::ResultR execute( const langbind::TransactionFunction::Input* inputi) const;
};

}}//namespace
#endif

