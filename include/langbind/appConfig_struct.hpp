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
///\file langbind/appConfig_struct.hpp
///\brief Data structures of the configuration of the processor environment
#ifndef _Wolframe_APPLICATION_ENVIRONMENT_CONFIGURATION_STRUCT_HPP_INCLUDED
#define _Wolframe_APPLICATION_ENVIRONMENT_CONFIGURATION_STRUCT_HPP_INCLUDED
#include "serialize/struct/filtermapBase.hpp"
#include <vector>
#include <string>
#include <boost/property_tree/ptree.hpp>

namespace _Wolframe {
namespace langbind {

struct DDLFormConfigStruct
{
	std::string DDL;						//< (optional) name of DDL referencing the DDL compiler (langbind::DDLCompilerConfigStruct)
	std::string file;						//< path of the DDL source of this form. The name of the function is defined in the source or given by the stem of the file name in 'sourcepath'

	///\brief Get the configuration structure description
	static const serialize::StructDescriptionBase* getStructDescription();
};

struct PrintLayoutConfigStruct
{
	std::string name;						//< (optional) name of the print function. If not defined then the the name of the print function is the stem of the file name in 'sourcepath'
	std::string type;						//< (optional) type of the print function. Defines the interpreter of the print layout description in the source file, creating a print function out of it. If not specified, then the file extension determines the type
	std::string file;						//< path of the source of this print function containing the print layout description.

	///\brief Get the configuration structure description
	static const serialize::StructDescriptionBase* getStructDescription();
};

struct TransactionFunctionConfigStruct
{
	std::string name;						//< name of the function
	std::string type;						//< name of the transaction function type (interpreter)
	std::string call;						//< the source of the transaction function

	///\brief Get the configuration structure description
	static const serialize::StructDescriptionBase* getStructDescription();
};

struct ScriptCommandConfigStruct
{
	std::string name;						//< name of the function in the script (globally unique)
	std::string type;						//< (optional) name of the transaction function type (interpreter). If not specified, then the file extension determines the type
	std::string file;						//< path of the script source

	///\brief Get the configuration structure description
	static const serialize::StructDescriptionBase* getStructDescription();
};


struct EnvironmentConfigStruct
{
	std::vector<DDLFormConfigStruct> form;				//< definitions of forms
	std::vector<PrintLayoutConfigStruct> printlayout;		//< definitions of print layouts for forms
	std::vector<TransactionFunctionConfigStruct> transaction;	//< definitions of transaction function
	std::vector<ScriptCommandConfigStruct> script;			//< definitions of script functions

	///\brief Get the configuration structure description
	static const serialize::StructDescriptionBase* getStructDescription();

	void setCanonicalPathes( const std::string& referencePath);

	boost::property_tree::ptree toPropertyTree() const;
	void initFromPropertyTree( const boost::property_tree::ptree& pt);
};

}}//namespace
#endif


