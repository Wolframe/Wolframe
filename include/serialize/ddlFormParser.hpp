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
/// \file serialize/ddlFormParser.hpp
/// \brief Interface to DDL form parsing
#ifndef _Wolframe_serialize_DDL_FORM_PARSER_HPP_INCLUDED
#define _Wolframe_serialize_DDL_FORM_PARSER_HPP_INCLUDED
#include "utils/typeSignature.hpp"
#include "types/form.hpp"
#include "types/variantStruct.hpp"
#include "types/variantStructDescription.hpp"
#include "serialize/ddl/ddlStructSerializer.hpp"
#include "serialize/ddl/ddlStructParser.hpp"

namespace _Wolframe {
namespace serialize {

/// \class DDLFormParser
/// \brief Parser of a form from a serialization defined by a DDL
class DDLFormParser
	:public virtual utils::TypeSignature
	,public serialize::DDLStructParser
{
public:
	/// \brief Constructor
	explicit DDLFormParser( const types::FormR& form_)
		:utils::TypeSignature("langbind::DDLFormParser", __LINE__)
		,DDLStructParser(form_.get())
		,m_form(form_){}

	/// \brief Constructor
	DDLFormParser( const types::FormR& form_, types::VariantStruct* substructure)
		:utils::TypeSignature("langbind::DDLFormParser", __LINE__)
		,DDLStructParser(substructure)
		,m_form(form_){}

	/// \brief Copy constructor
	DDLFormParser( const DDLFormParser& o)
		:utils::TypeSignature(o)
		,DDLStructParser(o)
		,m_form(o.m_form){}

	/// \brief Destructor
	virtual ~DDLFormParser(){}

	/// \brief Assignment operator
	DDLFormParser& operator=( const DDLFormParser& o)
	{
		utils::TypeSignature::operator=( o);
		DDLStructParser::operator=( o);
		m_form = o.m_form;
		return *this;
	}

	/// \brief Get a shared reference to the form structure filled by this parser
	const types::FormR& form() const	{return m_form;}

private:
	types::FormR m_form;		///< reference to the form structure of this parser
};

}}//namespace
#endif

