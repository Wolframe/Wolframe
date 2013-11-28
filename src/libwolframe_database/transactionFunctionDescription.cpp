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
///\brief Implementation of the description for transaction function constructor
///\file transactionFunctionDescription.hpp
#include "transactionFunctionDescription.hpp"
#include <sstream>
#include <iostream>

using namespace _Wolframe;
using namespace _Wolframe::db;

std::string TransactionFunctionDescription::MainProcessingStep::Call::tostring() const
{
	std::ostringstream out;
	if (embedded)
	{
		out << "[" << statement << "]( ";
	}
	else
	{
		out << statement << "( ";
	}
	std::vector<Param>::const_iterator pi = paramlist.begin(), pe = paramlist.end();

	for (int idx=0; pi != pe; ++pi)
	{
		if (idx++) out << ", ";
		out << pi->typeName() << " ";
		if (pi->namspace >= 0) out << "%" << pi->namspace << " ";
		out << "'" << pi->value << "'";
	}
	out << ")";
	return out.str();
}

std::string TransactionFunctionDescription::MainProcessingStep::tostring() const
{
	std::ostringstream out;
	if (selector_FOREACH.size())
	{
		out << "FOREACH " << selector_FOREACH << " ";
	}
	if (resultref_FOREACH >= 0)
	{
		out << "FOREACH %" << resultref_FOREACH << " ";
	}
	if (path_INTO.size())
	{
		out << "INTO ";
		std::vector<std::string>::const_iterator pi = path_INTO.begin(), pe = path_INTO.end();
		for (int idx=0; pi != pe; ++pi)
		{
			if (idx++) out << ".";
			out << *pi;
		}
		out << " ";
	}
	out << "DO ";
	if (nonempty) out << "NONEMPTY ";
	if (unique) out << "UNIQUE ";
	out << call.tostring() << ";";
	return out.str();
}


