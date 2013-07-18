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
///\file serialize/ddl/filtermapDDLParse.hpp
///\brief Defines the DDL structure deserialization for filters

#ifndef _Wolframe_SERIALIZE_DDL_FILTERMAP_PARSE_HPP_INCLUDED
#define _Wolframe_SERIALIZE_DDL_FILTERMAP_PARSE_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include "serialize/mapContext.hpp"
#include "serialize/ddl/filtermapDDLParseStack.hpp"
#include "ddl/structType.hpp"
#include <cstddef>

namespace _Wolframe {
namespace serialize {

class DDLStructParser
{
public:
	DDLStructParser(){}
	explicit DDLStructParser( types::VariantStruct* st);

	DDLStructParser( const DDLStructParser& o);
	virtual ~DDLStructParser(){}

	DDLStructParser& operator=( const DDLStructParser& o);

	void init( const langbind::TypedInputFilterR& i, Context::Flags flags=Context::None);

	bool call();

private:
	types::VariantStruct* m_st;
	Context m_ctx;
	langbind::TypedInputFilterR m_inp;
	FiltermapDDLParseStateStack m_stk;
};

}}//namespace
#endif

