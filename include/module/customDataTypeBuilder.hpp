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
///\file module/luaExtensionBuilder.hpp
///\brief Interface template for object builder of form functions
#ifndef _Wolframe_MODULE_LUA_LANGUAGE_EXTENSION_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#define _Wolframe_MODULE_LUA_LANGUAGE_EXTENSION_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#include "processor/moduleInterface.hpp"
#include "module/constructor.hpp"
#include "types/customDataType.hpp"
#include <string>
#include <cstring>
#include <stdexcept>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace module {

class CustomDataTypeConstructor :public SimpleObjectConstructor<types::CustomDataType>
{
public:
	CustomDataTypeConstructor( const char* classname_, const std::string& typename_, types::CreateCustomDataType cdt_constructor_)
		:m_classname(classname_)
		,m_typename(typename_)
		,m_cdt_constructor(cdt_constructor_) {}

	virtual ~CustomDataTypeConstructor(){}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return CUSTOM_DATA_TYPE_OBJECT;
	}
	virtual types::CustomDataType object() const
	{
		return (*m_cdt_constructor)( m_typename);
	}
	virtual const char* objectClassName() const
	{
		return m_classname;
	}

	const std::string& name() const
	{
		return m_typename;
	}

private:
	const char* m_classname;
	std::string m_typename;
	types::CreateCustomDataType m_cdt_constructor;
};

typedef boost::shared_ptr<CustomDataTypeConstructor> CustomDataTypeConstructorR;


class CustomDataTypeBuilder :public SimpleBuilder
{
public:
	CustomDataTypeBuilder( const char* classname_, const char* typename_, types::CreateCustomDataType cdt_constructor_)
		:SimpleBuilder(classname_)
		,m_typename(typename_)
		,m_cdt_constructor(cdt_constructor_)
	{}

	virtual ~CustomDataTypeBuilder(){}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return ObjectConstructorBase::CUSTOM_DATA_TYPE_OBJECT;
	}

	virtual ObjectConstructorBase* constructor()
	{
		return new CustomDataTypeConstructor( objectClassName(), m_typename, m_cdt_constructor);
	}

private:
	std::string m_typename;
	types::CreateCustomDataType m_cdt_constructor;
};

}}//namespace

#endif

