/************************************************************************

 Copyright (C) 2011 Project Wolframe.
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
///\file langbind_appConfig.cpp
///\brief Implementation of the language binding objects configuration
#include "langbind/appConfig.hpp"
#include "config/description.hpp"
#include <string>
#include <vector>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

const config::DescriptionBase* DDLCompilerConfigStruct::description()
{
	struct ThisDescription :public config::Description<DDLCompilerConfigStruct>
	{
		ThisDescription()
		{
			(*this)
			( "name",	&DDLCompilerConfigStruct::name)
			( "modulepath",	&DDLCompilerConfigStruct::modulepath)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const config::DescriptionBase* DDLFormConfigStruct::description()
{
	struct ThisDescription :public config::Description<DDLFormConfigStruct>
	{
		ThisDescription()
		{
			(*this)
			( "name",	&DDLFormConfigStruct::name)
			( "ddlname",	&DDLFormConfigStruct::ddlname)
			( "sourcepath",	&DDLFormConfigStruct::sourcepath)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const config::DescriptionBase* FilterConfigStruct::description()
{
	struct ThisDescription :public config::Description<FilterConfigStruct>
	{
		ThisDescription()
		{
			(*this)
			( "name",	&FilterConfigStruct::name)
			( "modulepath",	&FilterConfigStruct::modulepath)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const config::DescriptionBase* TransactionFunctionConfigStruct::description()
{
	struct ThisDescription :public config::Description<TransactionFunctionConfigStruct>
	{
		ThisDescription()
		{
			(*this)
			( "name",		&TransactionFunctionConfigStruct::name)
			( "cmdhandler",		&TransactionFunctionConfigStruct::cmdhandler)
			( "filtermodulepath",	&TransactionFunctionConfigStruct::filtermodulepath)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const config::DescriptionBase* PluginModuleAPIConfigStruct::description()
{
	struct ThisDescription :public config::Description<PluginModuleAPIConfigStruct>
	{
		ThisDescription()
		{
			(*this)
			( "name",	&PluginModuleAPIConfigStruct::name)
			( "modulepath",	&PluginModuleAPIConfigStruct::modulepath)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}


