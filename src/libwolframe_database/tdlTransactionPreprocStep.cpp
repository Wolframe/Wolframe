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
///\brief Implementing of the methods of the transaction function preprocessing step based on TDL
///\file tdlTransactionPreprocStep.cpp
#include "database/tdlTransactionPreprocStep.hpp"
#include <string>
#include <vector>
#include <iostream>

using namespace _Wolframe;
using namespace _Wolframe::db;

void TdlTransactionPreprocStep::print( std::ostream& out, const tf::TagTable* tagmap) const
{
	out << "INTO " ;
	std::vector<std::string>::const_iterator ri = m_resultpath.begin(), re = m_resultpath.end();
	if (m_resultpath.empty()) out << ".";
	for (; ri != re; ++ri)
	{
		if (ri != m_resultpath.begin()) out << "/";
		out << *ri;
	}
	out << " FOREACH ";
	selector().print( out, tagmap);
	out << " DO " << function() << "(";
	std::size_t ai = 0, ae = m_arguments.size();
	for (; ai != ae; ++ai)
	{
		if (ai != 0) out << ", ";
		switch (arg_type( ai))
		{
			case Argument::SelectorPath:
				if (arg_name(ai).size()) out << arg_name(ai) << "=";
				arg_selector(ai).print( out, tagmap);
				break;
			case Argument::LoopCounter:
				if (arg_name(ai).size()) out << arg_name(ai) << "=";
				out << "#";
				break;
			case Argument::Constant:
				if (arg_name(ai).size()) out << arg_name(ai) << "=";
				out << "\"" << arg_constant(ai) << "\"";
				break;
		}
	}
	out << ")";
}

