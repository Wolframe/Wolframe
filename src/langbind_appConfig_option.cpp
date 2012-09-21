/***********************************************************************

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
///\file langbind_appConfig_option.cpp
///\brief Implementation of the data structures on command line for the processor environment configuration
#include "langbind/appConfig_option.hpp"
#include "langbind/appConfig_struct.hpp"
#include "serialize/structOptionParser.hpp"
#include "utils/miscUtils.hpp"

using namespace _Wolframe;
using namespace _Wolframe::langbind;

DDLFormOption::DDLFormOption( const std::string& opt)
{
	serialize::parseStructOptionString( *this, opt);
	if (DDL.empty())
	{
		std::string ext = utils::getFileExtension( opt);
		if (ext.empty()) throw std::runtime_error( "no DDL specified (file extension missing) for form file");
		DDL = std::string( ext.c_str()+1);
	}
}

PrintLayoutOption::PrintLayoutOption( const std::string& opt)
{
	serialize::parseStructOptionString( *this, opt);
	if (name.empty())
	{
		name = utils::getFileStem( opt);
		if (name.empty()) throw std::runtime_error( "no name specified for layout description (file stem empty)");
	}
	if (type.empty())
	{
		std::string ext = utils::getFileExtension( opt);
		if (ext.empty()) throw std::runtime_error( "no type specified for layout description (file extension missing)");
		type = std::string( ext.c_str()+1);
	}
}

TransactionFunctionOption::TransactionFunctionOption( const std::string& opt)
{
	std::string::const_iterator si = opt.begin(), se = opt.end();
	if (!utils::parseNextToken( name, si, se))
	{
		throw std::runtime_error( "illegal transaction function option (name)");
	}
	if (!utils::parseNextToken( type, si, se))
	{
		throw std::runtime_error( "illegal transaction function option (type)");
	}
	call.insert( call.end(), si, se);
}

DatabaseConfigOption::DatabaseConfigOption( const std::string& opt)
{
	m_tree = serialize::structOptionTree( opt);
}




