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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file ddl/compilerInterface.hpp
///\brief Defines the interface for different type of data definition languages used for forms

#ifndef _Wolframe_DDL_COMPILER_INTERFACE_HPP_INCLUDED
#define _Wolframe_DDL_COMPILER_INTERFACE_HPP_INCLUDED
#include "ddl/atomicType.hpp"
#include "ddl/structType.hpp"
#include "countedReference.hpp"
#include <string>

namespace _Wolframe {
namespace ddl {

///\class CompilerInterface
///\brief Interface for DDL compilers
struct CompilerInterface
{
	explicit CompilerInterface( const std::string& n) :m_ddlname(n) {}
	virtual ~CompilerInterface(){}

	///\brief Compile a source from a string
	///\param[in] srcstring source as string
	///\param[out] result compilation result (a map represented as StructType::Struct of exported structures representing the forms)
	///\param[out] error error message in case of failure
	virtual bool compile( const std::string& srcstring, StructType& result, std::string& error) const=0;

	///\brief Compile a source from a file
	///\param[in] filename path of the file as string
	///\param[out] result compilation result (a map represented as StructType::Struct of exported structures representing the forms)
	///\param[out] error error message in case of failure
	bool compileFile( const std::string& filename, StructType& result, std::string& error) const;

	///\brief Get the name of the ddl this compiler is for
	const std::string& ddlname() const		{return m_ddlname;}

private:
	std::string m_ddlname;
};

///\brief Reference to a DDL compiler
typedef CountedReference<CompilerInterface> CompilerInterfaceR;

}}//namespace
#endif
