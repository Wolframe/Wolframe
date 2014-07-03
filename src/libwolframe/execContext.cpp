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
/// \file execContext.cpp
/// \brief Implementation execution context
#include "processor/execContext.hpp"
#include "filter/typedfilter.hpp"
#include "types/authorizationFunction.hpp"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::proc;

db::Transaction* ExecContext::transaction( const std::string& name)
{
	m_transaction_env.clear();
	if (m_dbstack.empty())
	{
		return m_provider->transaction( name);
	}
	else
	{
		return m_provider->transaction( m_dbstack.back(), name);
	}
}

bool ExecContext::checkAuthorization( const std::string& funcname, const std::string& resource, std::string& errmsg)
{
	if (funcname.empty()) return true;
	try
	{
		const types::AuthorizationFunction* func = m_provider->authorizationFunction( funcname);
		if (func == 0)
		{
			errmsg = std::string("authorization function '") + funcname + "' is not defined";
			return false;
		}
		return func->call( this, resource);
	}
	catch (std::runtime_error& e)
	{
		errmsg = std::string("authorization function '") + funcname + "' failed: " + e.what();
		return false;
	}
}

