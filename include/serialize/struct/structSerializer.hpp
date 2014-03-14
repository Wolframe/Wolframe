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
///\file serialize/struct/structSerializer.hpp
///\brief Serialization interface
#ifndef _Wolframe_SERIALIZE_STRUCT_SERIALIZER_HPP_INCLUDED
#define _Wolframe_SERIALIZE_STRUCT_SERIALIZER_HPP_INCLUDED
#include "serialize/struct/structDescriptionBase.hpp"

namespace _Wolframe {
namespace serialize {

//\class StructSerializer
//\brief Iterator on elements of structures based on a structure description
class StructSerializer :public langbind::TypedInputFilter
{
public:
	typedef boost::shared_ptr<void> ObjectReference;

	StructSerializer( const ObjectReference& obj, const StructDescriptionBase* descr);
	StructSerializer( const void* obj, const StructDescriptionBase* descr);

	StructSerializer( const StructSerializer& o);
	virtual ~StructSerializer(){}

	static std::string getElementPath( const SerializeStateStack& stk);

	void init( const langbind::TypedOutputFilterR& out, Context::Flags flags=Context::None);
	void reset();

	bool call();

	///\brief Get a self copy
	///\return allocated pointer to copy of this
	virtual langbind::TypedInputFilter* copy() const;

	///\brief Implements langbind::TypedInputFilter::getNext(langbind::FilterBase::ElementType&,types::VariantConst&)
	virtual bool getNext( langbind::FilterBase::ElementType& type, types::VariantConst& value);
	virtual bool setFlags( Flags f);

private:
	const void* m_ptr;
	const ObjectReference m_obj;
	const StructDescriptionBase* m_descr;
	Context m_ctx;
	langbind::TypedOutputFilterR m_out;
	SerializeStateStack m_stk;
};

}}//namespace
#endif

