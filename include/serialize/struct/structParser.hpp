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
///\file serialize/struct/structParser.hpp
///\brief Deserialization interface
#ifndef _Wolframe_SERIALIZE_STRUCT_PARSER_HPP_INCLUDED
#define _Wolframe_SERIALIZE_STRUCT_PARSER_HPP_INCLUDED
#include "serialize/struct/structDescriptionBase.hpp"

namespace _Wolframe {
namespace serialize {

//\class StructParser
//\brief Initializer of a structure based on a structure description feeded with a serialization
class StructParser
{
public:
	typedef boost::shared_ptr<void> ObjectReference;

	StructParser( void* obj, const StructDescriptionBase* descr);
	StructParser( const ObjectReference& obj, const StructDescriptionBase* descr);
	StructParser( const StructParser& o);
	virtual ~StructParser(){}

	static std::string getElementPath( const ParseStateStack& stk);

	void init( const langbind::TypedInputFilterR& i, Context::Flags flags=Context::None);

	const ObjectReference& object() const					{return m_obj;}
	const StructDescriptionBase* descr() const				{return m_descr;}

	bool call();

private:
	void* m_ptr;
	ObjectReference m_obj;
	const StructDescriptionBase* m_descr;
	Context m_ctx;
	langbind::TypedInputFilterR m_inp;
	ParseStateStack m_stk;
};

}}//namespace
#endif

