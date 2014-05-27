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
/// \file serialize/ddl/ddlStructParse.hpp
/// \brief Defines the DDL structure deserialization

#ifndef _Wolframe_SERIALIZE_DDL_STRUCT_PARSER_HPP_INCLUDED
#define _Wolframe_SERIALIZE_DDL_STRUCT_PARSER_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include "serialize/mapContext.hpp"
#include "serialize/ddl/ddlParseStack.hpp"
#include "types/variantStruct.hpp"
#include "types/variantStructDescription.hpp"
#include <cstddef>

namespace _Wolframe {
namespace serialize {

/// \class DDLStructParser
/// \brief Initializer of a DDL structure from an iterator (serialization)
class DDLStructParser
{
public:
	/// \brief Default constructor
	DDLStructParser(){}
	/// \brief Constructor
	explicit DDLStructParser( types::VariantStruct* st);

	/// \brief Copy constructor
	DDLStructParser( const DDLStructParser& o);
	/// \brief Destructor
	virtual ~DDLStructParser(){}

	/// \brief Assignment operator
	DDLStructParser& operator=( const DDLStructParser& o);

	/// \brief Parser start initialization
	void init( const langbind::TypedInputFilterR& i, Context::Flags flags=Context::None);

	/// \brief Call of one processing step the parser
	/// \remark The processing is finished when the call returns true. In case of false returned you have to inspect the input filter state to determine what is to do next. In case of a validation error, the function throws a SerializationErrorException
	bool call();

private:
	types::VariantStruct* m_st;
	Context m_ctx;
	langbind::TypedInputFilterR m_inp;
	DDLParseStateStack m_stk;
};

}}//namespace
#endif

