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
///\file serialize/ddlFormSerializer.hpp
///\brief Interface to DDL form serialization
#ifndef _Wolframe_serialize_DDL_FORM_SERIALIZER_HPP_INCLUDED
#define _Wolframe_serialize_DDL_FORM_SERIALIZER_HPP_INCLUDED
#include "types/form.hpp"
#include "types/variantStruct.hpp"
#include "types/variantStructDescription.hpp"
#include "serialize/ddl/ddlStructSerializer.hpp"

namespace _Wolframe {
namespace serialize {


/// \class DDLFormSerializer
/// \brief Serializer of a form defined by a DDL
class DDLFormSerializer
	:public serialize::DDLStructSerializer
{
public:
	/// \brief Default constructor
	DDLFormSerializer(){}

	/// \brief Constructor
	explicit DDLFormSerializer( const types::FormR& form_)
		:DDLStructSerializer(form_.get())
		,m_form(form_){}

	/// \brief Constructor
	explicit DDLFormSerializer( const types::FormR& form_, const types::VariantStruct* substructure)
		:DDLStructSerializer(substructure)
		,m_form(form_){}

	/// \brief Copy constructor
	DDLFormSerializer( const DDLFormSerializer& o)
		:DDLStructSerializer(o)
		,m_form(o.m_form){}
	/// \brief Destructor
	virtual ~DDLFormSerializer(){}

	/// \brief Assignment operator
	DDLFormSerializer& operator =( const DDLFormSerializer& o)
	{
		DDLStructSerializer::operator=( o);
		m_form = o.m_form;
		return *this;
	}

	/// \brief Get a shared reference to the form of this serializer
	const types::FormR& form() const	{return m_form;}

private:
	types::FormR m_form;		///< reference to the form of this serializer
};

}}//namespace
#endif

