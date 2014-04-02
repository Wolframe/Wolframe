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
///\brief Implementaion of the methods of a transaction function based on TDL
///\file tdlTransactionFunction.cpp
#include "database/tdlTransactionFunction.hpp"
#include <string>
#include <vector>

using namespace _Wolframe;
using namespace _Wolframe::db;

void TdlTransactionFunction::print( std::ostream& out) const
{
	out << "TRANSACTION " << m_name << std::endl;
	if (!m_preproc.empty())
	{
		if (!m_authfunction.empty())
		{
			out << "AUTHORIZE (" << m_authfunction;
			if (!m_authresource.empty())
			{
				out << ", " << m_authresource;
			}
			out << ")" << std::endl;
		}
		if (!m_resultfilter.empty())
		{
			out << "RESULT FILTER " << m_resultfilter << std::endl;
		}

		out << "PREPROC" << std::endl;
		std::vector<TdlTransactionPreprocStep>::const_iterator pi = m_preproc.begin(), pe = m_preproc.end();
		for (; pi != pe; ++pi)
		{
			pi->print( out, &m_program->pathset.tagtab()) << std::endl;
		}
		out << "ENDPROC" << std::endl;

		out << "CODE RAW" << std::endl;
		m_program->code.printRaw( out);
		out << "END RAW" << std::endl;
	}
}


