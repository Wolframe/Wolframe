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
///\brief Implementation of the transaction input
///\file transactionInput.cpp
#include "database/transactionInput.hpp"
#include <iostream>
#include <sstream>
#include <string>

using namespace _Wolframe;
using namespace _Wolframe::db;

std::string TransactionInput::Command::Argument::tostring() const
{
	std::ostringstream rt;
	rt << typeName() << " " << m_value.typeName() << " '" << m_value.tostring() << "' :" << m_scope_functionidx;
	return rt.str();
}

std::string TransactionInput::Command::tostring() const
{
	std::ostringstream rt;
	rt << "[" << m_functionidx << "] L" << m_level << " ";
	if (m_foreach_functionidx>0) rt << "/" << m_foreach_functionidx << " ";
	if (flags(NonEmptyResult)) rt << "NONEMPTY ";
	if (flags(UniqueResult)) rt << "UNIQUE ";
	rt << m_statement << "( ";
	arg_const_iterator ai = begin(), ae = end();
	for (int idx=0;ai!=ae;++ai)
	{
		if (idx++) rt << ", ";
		rt << ai->tostring();
	}
	rt << ")";
	return rt.str();
}

std::string TransactionInput::tostring( const utils::PrintFormat* pformat) const
{
	std::ostringstream rt;
	std::vector<Command>::const_iterator ci = m_cmd.begin(), ce = m_cmd.end();
	for (;ci != ce; ++ci) rt << ci->tostring() << pformat->newitem;
	return rt.str();
}

