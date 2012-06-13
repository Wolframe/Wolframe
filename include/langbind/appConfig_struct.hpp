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
///\file langbind/appConfig_struct.hpp
///\brief Data structures of the configuration of the processor environment
#ifndef _Wolframe_APPLICATION_ENVIRONMENT_CONFIGURATION_STRUCT_HPP_INCLUDED
#define _Wolframe_APPLICATION_ENVIRONMENT_CONFIGURATION_STRUCT_HPP_INCLUDED
#include <vector>
#include <string>
#include "config/descriptionBase.hpp"

namespace _Wolframe {
namespace langbind {

struct DDLCompilerConfigStruct
{
	std::string name;						//< name of DDL referenced by form definitions
	std::string modulepath;						//< path of the DDL compiler module

	///\brief Get the configuration structure description
	static const config::DescriptionBase* description();
};

struct DDLFormConfigStruct
{
	std::string name;						//< name of the form
	std::string DDL;						//< name of DDL referencing the DDL compiler (langbind::DDLCompilerConfigStruct)
	std::string sourcepath;						//< path of the DDL source of this form

	///\brief Get the configuration structure description
	static const config::DescriptionBase* description();
};

struct FilterConfigStruct
{
	std::string name;						//< name of the form
	std::string modulepath;						//< path of filter module

	///\brief Get the configuration structure description
	static const config::DescriptionBase* description();
};

struct TransactionFunctionConfigStruct
{
	std::string name;						//< name of the transaction function
	std::string filter;						//< name of the filter defining the transaction command reader/writer
	std::string modulepath;						//< path of the transaction function handler module (cmdbind::CommandHandler)

	///\brief Get the configuration structure description
	static const config::DescriptionBase* description();
};

struct ScriptCommandConfigStruct
{
	std::string name;						//< name of the function in the script (globally unique)
	std::string sourcepath;						//< path of the script source

	///\brief Get the configuration structure description
	static const config::DescriptionBase* description();
};

struct FormFunctionConfigStruct
{
	std::string name;						//< name of the form function
	std::string modulepath;						//< path of module with the function definition and implementation

	///\brief Get the configuration structure description
	static const config::DescriptionBase* description();
};

struct EnvironmentConfigStruct
{
	std::vector<DDLCompilerConfigStruct> DDL;			//< definitions of DDLs
	std::vector<DDLFormConfigStruct> form;				//< definitions of forms
	std::vector<FilterConfigStruct> filter;				//< definitions of filters
	std::vector<TransactionFunctionConfigStruct> transaction;	//< definitions of transaction functions
	std::vector<ScriptCommandConfigStruct> script;			//< definitions of script functions
	std::vector<FormFunctionConfigStruct> formfunction;		//< definitions of the form functions

	///\brief Get the configuration structure description
	static const config::DescriptionBase* description();
};

}}//namespace
#endif


