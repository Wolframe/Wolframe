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
///\file tprocHandlerConfig.hpp
///\brief tproc handler configuration
#ifndef _Wolframe_TPROC_HANDLER_CONFIGURATION_HPP_INCLUDED
#define _Wolframe_TPROC_HANDLER_CONFIGURATION_HPP_INCLUDED
#include <vector>
#include <string>
#include "protocol/commandHandler.hpp"
#include "config/descriptionBase.hpp"
#include "ddl/compilerInterface.hpp"
#include "standardConfigs.hpp"

namespace _Wolframe {
namespace langbind {

struct DDLCompilerConfigStruct
{
	std::string name;				//< name of DDL referenced by form definitions
	std::string modulepath;				//< path of the DDL compiler module

	///\brief Get the configuration structure description
	static const config::DescriptionBase* description();
};

struct DDLFormConfigStruct
{
	std::string name;				//< name of the form
	std::string ddlname;				//< name of DDL referencing the DDL compiler (langbind::DDLCompilerConfigStruct)
	std::string sourcepath;				//< path of the DDL source of this form

	///\brief Get the configuration structure description
	static const config::DescriptionBase* description();
};

struct FilterConfigStruct
{
	std::string name;				//< name of the form
	std::string modulepath;				//< path of filter module

	///\brief Get the configuration structure description
	static const config::DescriptionBase* description();
};

struct TransactionFunctionConfigStruct
{
	std::string name;				//< name of the transaction function
	std::string cmdhandler;				//< identifier of the transaction function handler (protocol::CommandHandler)
	std::string filtermodulepath;			//< name of the module defining the transaction command reader/writer

	///\brief Get the configuration structure description
	static const config::DescriptionBase* description();
};

struct PluginModuleAPIConfigStruct
{
	std::string name;				//< name of the plug-in function
	std::string modulepath;				//< path of filter module with the API form definition and the function implementation

	///\brief Get the configuration structure description
	static const config::DescriptionBase* description();
};

}}//namespace
#endif


