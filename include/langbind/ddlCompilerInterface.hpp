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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file langbind/ddlCompilerInterface.hpp
///\brief Defines the compiler interface for definition languages (DDL) used for specifying forms

#ifndef _Wolframe_DDL_COMPILER_INTERFACE_HPP_INCLUDED
#define _Wolframe_DDL_COMPILER_INTERFACE_HPP_INCLUDED
#include "types/form.hpp"
#include "types/variant.hpp"
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace langbind {

///\class DDLCompiler
///\brief Interface for DDL compilers
struct DDLCompiler
{
	DDLCompiler( const std::string& n, const std::string& e)
		:m_ddlname(n),m_fileext(e) {}
	virtual ~DDLCompiler(){}

	///\brief Compile a source from a string. Throws in case of error.
	///\param[in] srcstring source as string
	///\param[in] typemap map with atomic type definitions
	///\return compilation result (a list of public and private forms).
	///\renmark Forms with an empty name are declared as private and not inserted into the map of public forms. But they are also not deleted and kept in a list, so that they can still be referenced by other forms in the list (for indirection).
	virtual std::vector<types::FormDescriptionR> compile( const std::string& srcstring, const types::NormalizeFunctionMap* typemap) const=0;

	///\brief Get the name of the ddl this compiler is for
	const std::string& ddlname() const		{return m_ddlname;}

	///\brief Get the file extension for files implementing this DDL
	const std::string& fileext() const		{return m_fileext;}
private:
	std::string m_ddlname;
	std::string m_fileext;
};

///\brief Reference to a DDL compiler
typedef boost::shared_ptr<DDLCompiler> DDLCompilerR;

///\brief Describes the creation of a DDL compiler object
typedef DDLCompiler* (*CreateDDLCompilerFunc)();


}}//namespace
#endif
