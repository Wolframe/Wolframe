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
///\brief Implementation of the transaction output
///\file transactionOutput.cpp
#include "database/transactionOutput.hpp"
#include <iostream>
#include <sstream>
#include <string>

using namespace _Wolframe;
using namespace _Wolframe::db;

std::string TransactionOutput::CommandResult::tostring( const utils::PrintFormat* pformat) const
{
	std::ostringstream rt;
	std::size_t ci = 0,ce = nofColumns();
	rt << "columns" << pformat->itemdelimiter << ce << pformat->assign;
	for (; ci<ce; ++ci) rt << pformat->itemdelimiter << columnName( ci);
	std::vector<CommandResult::Row>::const_iterator wi = begin(), we = end();
	for (;wi != we; ++wi)
	{
		rt << pformat->newitem;
		for (ci=0; ci<ce; ++ci)
		{
			if (wi->at(ci).defined())
			{
				rt << pformat->itemdelimiter << pformat->startvalue;
				if (pformat->maxitemsize)
				{
					rt << utils::getLogString( wi->at(ci), pformat->maxitemsize);
				}
				else
				{
					rt << wi->at(ci).tostring();
				}
				rt << pformat->endvalue;
			}
			else
			{
				rt << pformat->itemdelimiter << "NULL";
			}
		}
		rt << pformat->decldelimiter;
	}
	return rt.str();
}

std::string TransactionOutput::tostring( const utils::PrintFormat* pformat) const
{
	std::ostringstream rt;
	result_const_iterator ri = begin(), re = end();
	for (; ri != re; ++ri)
	{
		rt << "function" << pformat->itemdelimiter << ri->functionidx() << pformat->itemdelimiter << ri->tostring( pformat);
	}
	return rt.str();
}

TransactionOutput::result_const_iterator TransactionOutput::resultIterator( std::size_t functionidx) const
{
	if (m_result.empty()) return end();
	result_const_iterator rt = m_result.begin() + m_result.size() -1, begin_ = begin();
	while (rt->functionidx() > functionidx && rt != begin_) --rt;
	if (rt->functionidx() != functionidx) return end();
	return rt;
}

