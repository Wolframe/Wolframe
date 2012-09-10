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
///\file langbind_appConfig_option.cpp
///\brief Implementation of the data structures on command line for the processor environment configuration
#include "langbind/appConfig_option.hpp"
#include "langbind/appConfig_struct.hpp"
#include "utils/miscUtils.hpp"

using namespace _Wolframe;
using namespace _Wolframe::langbind;

static void parseNameTypeSource( const std::string& opt, bool getFilenameStemAsName, std::string& name, std::string& type, std::string& file)
{
	const char* cc = std::strchr( opt.c_str(), ':');
	const char* xx = std::strchr( opt.c_str(), '.');
	if (cc && cc-opt.c_str() > 1)
	{
		if (xx && xx < cc)
		{
			std::string type_name( opt.c_str(), cc-opt.c_str());
			std::string ext = utils::getFileExtension( type_name);
			std::string stem = utils::getFileStem( type_name);

			if (!ext.empty()) type = std::string( ext.c_str() + 1);
			if (!stem.empty()) name = std::string( stem.c_str());
		}
		else
		{
			type = std::string( opt.c_str(), cc-opt.c_str());
			name = "";
		}
		file = std::string( cc+1);
		if (type.empty())
		{
			std::string ext = utils::getFileExtension( file);
			if (!ext.empty()) type = std::string( ext.c_str() + 1);
		}
		if (name.empty() && getFilenameStemAsName)
		{
			std::string stem = utils::getFileStem( file);
			if (!stem.empty()) name = std::string( stem.c_str());
		}
	}
	else
	{
		file = opt;
		std::string ext = utils::getFileExtension( opt);
		if (ext.empty()) throw std::runtime_error( "no type of print layout specified (file extension missing)");
		type = std::string( ext.c_str() + 1);

		if (getFilenameStemAsName)
		{
			std::string stem = utils::getFileStem( opt);
			if (stem.empty()) throw std::runtime_error( "no name of print layout specified (file stem missing)");
			name = std::string( stem.c_str());
		}
	}
}

DDLFormOption::DDLFormOption( const std::string& src)
{
	const char* cc = std::strchr( src.c_str(), ':');
	if (cc && cc-src.c_str() > 1)
	{
		DDL = std::string( src.c_str(), cc-src.c_str());
		file = std::string( cc+1);
	}
	else
	{
		std::string ext = utils::getFileExtension( src);
		if (!ext.size()) throw std::runtime_error( "no DDL specified (file extension missing) for form file");

		DDL = std::string( ext.c_str()+1);
		file = src;
	}
}

PrintLayoutOption::PrintLayoutOption( const std::string& opt)
{
	parseNameTypeSource( opt, true, name, type, file);
}

TransactionFunctionOption::TransactionFunctionOption( const std::string& src)
{
	std::string::const_iterator si = src.begin(), se = src.end();
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

ScriptCommandOption::ScriptCommandOption( const std::string& opt)
{
	parseNameTypeSource( opt, false, name, type, file);
}


