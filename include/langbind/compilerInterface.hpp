/************************************************************************
Copyright (C) 2011 - 2013 Project Wolframe.
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
///\file langbind/compilerInterface.hpp
///\brief Defines the interface for different type of data definition languages used for forms

#ifndef _Wolframe_DDL_COMPILER_INTERFACE_HPP_INCLUDED
#define _Wolframe_DDL_COMPILER_INTERFACE_HPP_INCLUDED
#include "types/form.hpp"
#include "types/variant.hpp"
#include "types/countedReference.hpp"
#include <string>
#include <vector>

namespace _Wolframe {
namespace langbind {

///\class DDLCompiler
///\brief Interface for DDL compilers
struct DDLCompiler
{
	explicit DDLCompiler( const std::string& n) :m_ddlname(n) {}
	virtual ~DDLCompiler(){}

	///\brief Compile a source from a string. Throws in case of error.
	///\param[in] srcstring source as string
	///\param[in] typemap map with atomic type definitions
	///\return compilation result (a list of forms)
	virtual std::vector<types::FormDescriptionR> compile( const std::string& srcstring, const types::NormalizeFunctionMap* typemap) const=0;

	///\brief Get the name of the ddl this compiler is for
	const std::string& ddlname() const		{return m_ddlname;}

private:
	std::string m_ddlname;
};

///\brief Reference to a DDL compiler
typedef types::CountedReference<DDLCompiler> DDLCompilerR;

///\brief Describes the creation of a DDL compiler object
typedef DDLCompiler* (*CreateDDLCompilerFunc)();


}}//namespace
#endif
