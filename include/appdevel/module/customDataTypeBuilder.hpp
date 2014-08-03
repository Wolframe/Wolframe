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
/// \file appdevel/module/customDataTypeBuilder.hpp
/// \brief Interface template for object builder of form functions
#ifndef _Wolframe_MODULE_CUSTOM_DATA_TYPE_BUILDER_HPP_INCLUDED
#define _Wolframe_MODULE_CUSTOM_DATA_TYPE_BUILDER_HPP_INCLUDED
#include "module/moduleInterface.hpp"
#include "module/constructor.hpp"
#include "types/customDataType.hpp"
#include "types/keymap.hpp"
#include <string>
#include <cstring>
#include <stdexcept>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace module {

/// \class CustomDataTypeConstructor
/// \brief Constructor of a custom data type for language bindings
class CustomDataTypeConstructor
	:public SimpleObjectConstructor<types::CustomDataType>
{
public:
	CustomDataTypeConstructor( const char* classname_, const std::string& identifier_, types::CreateCustomDataType createFunc_)
		:m_classname(classname_)
	{
		types::CustomDataType* cdt = (*createFunc_)( identifier_);
		m_datatype.reset( cdt);
	}

	virtual ~CustomDataTypeConstructor(){}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return CUSTOM_DATA_TYPE_OBJECT;
	}

	const std::string& identifier() const
	{
		return m_datatype->name();
	}

	virtual const char* objectClassName() const
	{
		return m_classname;
	}

	virtual types::CustomDataType* object() const
	{
		return new types::CustomDataType( *m_datatype);
	}

private:
	const char* m_classname;
	types::CustomDataTypeR m_datatype;
};

typedef boost::shared_ptr<CustomDataTypeConstructor> CustomDataTypeConstructorR;


/// \class CustomDataTypeBuilder
/// \brief Builder of a custom data type constructor for language bindings
class CustomDataTypeBuilder :public SimpleBuilder
{
public:
	CustomDataTypeBuilder( const char* classname_, const char* identifier_, types::CreateCustomDataType createFunc_)
		:SimpleBuilder(classname_),m_identifier(identifier_),m_createFunc(createFunc_)
	{}

	virtual ~CustomDataTypeBuilder(){}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return ObjectConstructorBase::CUSTOM_DATA_TYPE_OBJECT;
	}

	virtual ObjectConstructorBase* constructor()
	{
		return new CustomDataTypeConstructor( objectClassName(), m_identifier, m_createFunc);
	}

private:
	std::string m_identifier;
	types::CreateCustomDataType m_createFunc;
};

}}//namespace

#endif

