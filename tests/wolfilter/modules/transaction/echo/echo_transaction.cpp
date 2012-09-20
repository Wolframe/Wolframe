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
///\brief Implementation of a fake transaction handler for tests
#include "echo_transaction.hpp"
#include <string>
#include <cstring>
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::test;

EchoTransactionResult::EchoTransactionResult( const EchoTransactionResult& o)
	:langbind::TransactionFunction::Result(o)
	,m_itemar(o.m_itemar)
	,m_itemitr(m_itemar.begin()){}

bool EchoTransactionResult::getNext( ElementType& type, langbind::TypedFilterBase::Element& element)
{
	if (m_itemitr == m_itemar.end()) return false;
	type = m_itemitr->first;
	element = langbind::TypedFilterBase::Element( m_itemitr->second);
	++m_itemitr;
	return true;
}

EchoTransactionInput::EchoTransactionInput( const EchoTransactionInput& o)
	:langbind::TransactionFunction::Input(o)
	,m_result(o.m_result){}

bool EchoTransactionInput::print( ElementType type, const Element& element)
{
	EchoTransactionResult::Item item( type, element.tostring());
	m_result.m_itemar.push_back( item);
	m_result.m_itemitr = m_result.m_itemar.begin();
	return true;
}

langbind::TransactionFunction::InputR EchoTransactionFunction::getInput() const
{
	return langbind::TransactionFunction::InputR( new EchoTransactionInput());
}

langbind::TransactionFunction::ResultR EchoTransactionFunction::execute( const langbind::TransactionFunction::Input* inputi) const
{
	const EchoTransactionInput* input = dynamic_cast<const EchoTransactionInput*>( inputi);
	return langbind::TransactionFunction::ResultR( new EchoTransactionResult( input->m_result));
}

