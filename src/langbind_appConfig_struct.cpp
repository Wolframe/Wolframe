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
///\file langbind_appConfig_struct.cpp
///\brief Implementation of the data structures of the processor environment configuration
#include "langbind/appConfig_struct.hpp"
#include "config/structSerialize.hpp"
#include "serialize/struct/filtermapDescription.hpp"

using namespace _Wolframe;
using namespace _Wolframe::langbind;

const serialize::StructDescriptionBase* DDLCompilerConfigStruct::getStructDescription()
{
	struct ThisDescription :public serialize::StructDescription<DDLCompilerConfigStruct>
	{
		ThisDescription()
		{
			(*this)
			( "name",		&DDLCompilerConfigStruct::name)
			( "modulepath",		&DDLCompilerConfigStruct::modulepath)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const serialize::StructDescriptionBase* DDLFormConfigStruct::getStructDescription()
{
	struct ThisDescription :public serialize::StructDescription<DDLFormConfigStruct>
	{
		ThisDescription()
		{
			(*this)
			( "DDL",		&DDLFormConfigStruct::DDL)
			( "sourcepath",		&DDLFormConfigStruct::sourcepath)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const serialize::StructDescriptionBase* FilterConfigStruct::getStructDescription()
{
	struct ThisDescription :public serialize::StructDescription<FilterConfigStruct>
	{
		ThisDescription()
		{
			(*this)
			( "name",		&FilterConfigStruct::name)
			( "modulepath",		&FilterConfigStruct::modulepath)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}


const serialize::StructDescriptionBase* FormFunctionConfigStruct::getStructDescription()
{
	struct ThisDescription :public serialize::StructDescription<FormFunctionConfigStruct>
	{
		ThisDescription()
		{
			(*this)
			( "name",		&FormFunctionConfigStruct::name)
			( "modulepath",		&FormFunctionConfigStruct::modulepath)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const serialize::StructDescriptionBase* TransactionTypeConfigStruct::getStructDescription()
{
	struct ThisDescription :public serialize::StructDescription<TransactionTypeConfigStruct>
	{
		ThisDescription()
		{
			(*this)
			( "name",		&TransactionTypeConfigStruct::name)
			( "modulepath",		&TransactionTypeConfigStruct::modulepath)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const serialize::StructDescriptionBase* TransactionFunctionConfigStruct::getStructDescription()
{
	struct ThisDescription :public serialize::StructDescription<TransactionFunctionConfigStruct>
	{
		ThisDescription()
		{
			(*this)
			( "name",		&TransactionFunctionConfigStruct::name)
			( "interpreter",	&TransactionFunctionConfigStruct::interpreter)
			( "call",		&TransactionFunctionConfigStruct::call)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const serialize::StructDescriptionBase* ScriptCommandConfigStruct::getStructDescription()
{
	struct ThisDescription :public serialize::StructDescription<ScriptCommandConfigStruct>
	{
		ThisDescription()
		{
			(*this)
			( "name",		&ScriptCommandConfigStruct::name)
			( "sourcepath",		&ScriptCommandConfigStruct::sourcepath)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const serialize::StructDescriptionBase* EnvironmentConfigStruct::getStructDescription()
{
	struct ThisDescription :public serialize::StructDescription<EnvironmentConfigStruct>
	{
		ThisDescription()
		{
			(*this)
			( "DDL",		&EnvironmentConfigStruct::DDL)
			( "form",		&EnvironmentConfigStruct::form)
			( "filter",		&EnvironmentConfigStruct::filter)
			( "transactiontype",	&EnvironmentConfigStruct::transactiontype)
			( "transaction",	&EnvironmentConfigStruct::transaction)
#if WITH_LUA
			( "script",		&EnvironmentConfigStruct::script)
#endif
			( "formfunction",	&EnvironmentConfigStruct::formfunction)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

