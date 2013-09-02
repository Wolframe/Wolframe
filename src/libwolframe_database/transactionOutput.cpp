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
///\brief Implementation of the transaction output
///\file transactionOutput.cpp
#include "database/transactionOutput.hpp"
#include <iostream>
#include <sstream>
#include <string>

using namespace _Wolframe;
using namespace _Wolframe::db;

std::string TransactionOutput::CommandResult::tostring() const
{
	std::ostringstream rt;
	std::size_t ci = 0,ce = nofColumns();
	rt << " COLUMNS " << ce << ":";
	for (; ci<ce; ++ci) rt << " " << columnName( ci);
	rt << std::endl;
	std::vector<CommandResult::Row>::const_iterator wi = begin(), we = end();
	for (;wi != we; ++wi)
	{
		rt << ">";
		for (ci=0; ci<ce; ++ci)
		{
			if (wi->at(ci).defined())
			{
				rt << " '" << wi->at(ci).tostring() << "'";
			}
			else
			{
				rt << " NULL";
			}
		}
	}
	return rt.str();
}

std::string TransactionOutput::tostring() const
{
	std::ostringstream rt;
	result_const_iterator ri = begin(), re = end();
	for (; ri != re; ++ri)
	{
		rt << "RESULT " << ri->functionidx() << " " << ri->tostring();
		rt << std::endl;
	}
	return rt.str();
}
