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
///\file mylangFunctionProgramType.hpp
///\brief Function call for mylang scripts
#ifndef _Wolframe_Mylang_FUNCTION_CALL_HPP_INCLUDED
#define _Wolframe_Mylang_FUNCTION_CALL_HPP_INCLUDED

namespace _Wolframe {
namespace langbind {

struct StructPointer;

struct StructReference
{
	StructReference()
		:ptr(0){}
	StructReference( const StructReference& o)
		:id(o.id),ptr(o.ptr){}
	StructReference( const types::Variant& id_, void* ptr_)
		:id(id_),ptr(ptr_){}

	///\brief Get a pointer to a substructure or 0 if not defined
	///\remark throws on error
	StructPointer* getSubstruct( const types::Variant& elemid_);

	///\brief Create a substructure and get a pointer to it
	///\remark throws on error
	StructPointer* addSubstruct( const types::Variant& elemid_);

	types::Variant id;
	StructPointer* ptr;
};

///\brief Mylang function call
StructPointer* callMylangFunction( const proc::ProcessorProvider* provider, const StructPointer& arg);

}} //namespace
#endif


