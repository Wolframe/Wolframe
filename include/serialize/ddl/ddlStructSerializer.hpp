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
///\file serialize/ddl/ddlStructSerializer.hpp
///\brief Defines the DDL structure serialization

#ifndef _Wolframe_SERIALIZE_DDL_STRUCT_SERIALIZER_HPP_INCLUDED
#define _Wolframe_SERIALIZE_DDL_STRUCT_SERIALIZER_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include "types/variant.hpp"
#include "serialize/mapContext.hpp"
#include "serialize/ddl/ddlSerializeStack.hpp"
#include "types/variantStruct.hpp"
#include <cstddef>

namespace _Wolframe {
namespace serialize {

//\class DDLStructSerializer
//\brief Iterator on a DDL structure (serializer of VariantStruct)
class DDLStructSerializer :public langbind::TypedInputFilter
{
public:
	DDLStructSerializer()
		:utils::TypeSignature("serialize::DDLStructSerializer", __LINE__){}
	explicit DDLStructSerializer( const types::VariantStruct* st);

	DDLStructSerializer( const DDLStructSerializer& o);
	virtual ~DDLStructSerializer(){}

	DDLStructSerializer& operator =( const DDLStructSerializer& o);

	void init( const langbind::TypedOutputFilterR& out, Context::Flags flags=Context::None);

	bool call();

	///\brief Get a self copy
	///\return allocated pointer to copy of this
	virtual TypedInputFilter* copy() const		{return new DDLStructSerializer(*this);}

	virtual bool getNext( langbind::FilterBase::ElementType& type, types::VariantConst& value);
	virtual bool setFlags( Flags f);

private:
	const types::VariantStruct* m_st;
	Context m_ctx;
	langbind::TypedOutputFilterR m_out;
	DDLSerializeStateStack m_stk;
};

}}//namespace
#endif

