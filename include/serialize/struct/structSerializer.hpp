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
/// \file serialize/struct/structSerializer.hpp
/// \brief Serialization interface
#ifndef _Wolframe_SERIALIZE_STRUCT_SERIALIZER_HPP_INCLUDED
#define _Wolframe_SERIALIZE_STRUCT_SERIALIZER_HPP_INCLUDED
#include "serialize/struct/structDescriptionBase.hpp"

namespace _Wolframe {
namespace serialize {

/// \class StructSerializer
/// \brief Iterator on elements of structures based on a structure description
class StructSerializer :public langbind::TypedInputFilter
{
public:
	typedef boost::shared_ptr<void> ObjectReference;

	/// \brief Constructor
	StructSerializer( const ObjectReference& obj, const StructDescriptionBase* descr);
	/// \brief Constructor
	StructSerializer( const void* obj, const StructDescriptionBase* descr);

	/// \brief Copy constructor
	StructSerializer( const StructSerializer& o);
	/// \brief Destructor
	virtual ~StructSerializer(){}

	/// \brief Get the current element path defined by the stack 'stk' as string for error messages
	static std::string getElementPath( const SerializeStateStack& stk);

	/// \brief Serialize start initialization
	void init( const langbind::TypedOutputFilterR& out, serialize::Flags::Enum flags=serialize::Flags::None);
	void reset();

	/// \brief Call of one processing step the serializer
	/// \remark The processing is finished when the call returns true. In case of false returned you have to inspect the output filter state to determine what is to do next.
	/// \remark Do not mix 'call()' with 'init(const langbind::TypedOutputFilterR&,serialize::Flags)' and 'getNext(langbind::FilterBase::ElementType&,types::VariantConst&)'. Use either one or the other
	bool call();

	/// \brief Get a self copy
	/// \return allocated pointer to copy of this
	virtual langbind::TypedInputFilter* copy() const;

	/// \brief Get the next element of the serialization, implements langbind::TypedInputFilter::getNext(langbind::FilterBase::ElementType&,types::VariantConst&)
	/// \remark Do not mix 'call()' with 'init(const langbind::TypedOutputFilterR&,serialize::Flags)' and 'getNext(langbind::FilterBase::ElementType&,types::VariantConst&)'. Use either one or the other
	virtual bool getNext( langbind::FilterBase::ElementType& type, types::VariantConst& value);

	/// \brief Set the flags stearing the serialization, implements FilterBase::setFlags(FilterBase::Flags)
	/// \return false, if not all flags have the behaviour implemented and are accepted 
	virtual bool setFlags( FilterBase::Flags f);

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

