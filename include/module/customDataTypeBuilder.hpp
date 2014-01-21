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
///\file module/customDataTypeBuilder.hpp
///\brief Interface template for object builder of form functions
#ifndef _Wolframe_MODULE_CUSTOM_DATA_TYPE_BUILDER_HPP_INCLUDED
#define _Wolframe_MODULE_CUSTOM_DATA_TYPE_BUILDER_HPP_INCLUDED
#include "processor/moduleInterface.hpp"
#include "module/constructor.hpp"
#include "types/customDataType.hpp"
#include "types/keymap.hpp"
#include <string>
#include <cstring>
#include <stdexcept>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace module {

class CustomDataTypeConstructor :public SimpleObjectConstructor<types::CustomDataType>
{
public:
	CustomDataTypeConstructor( const char* classname_, const char* name_, const types::keymap<types::CreateCustomDataType>& constructormap_)
		:m_classname(classname_)
		,m_name(name_)
	{
		types::keymap<types::CreateCustomDataType>::const_iterator ci = constructormap_.begin(), ce = constructormap_.end();
		for (; ci != ce; ++ci)
		{
			types::CustomDataTypeR dt( ci->second( ci->first));
			m_typemap.insert( ci->first, dt);
		}
	}

	virtual ~CustomDataTypeConstructor(){}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return CUSTOM_DATA_TYPE_OBJECT;
	}

	const types::CustomDataType* object( const std::string& name_) const
	{
		ConstructorMap::const_iterator fi = m_typemap.find( name_);
		if (fi == m_typemap.end()) return 0;
		return fi->second.get();
	}

	std::vector<std::string> types() const
	{
		return m_typemap.getkeys<std::vector<std::string> >();
	}

	virtual const char* objectClassName() const
	{
		return m_classname;
	}

	const char* domain() const
	{
		return m_name;
	}

private:
	const char* m_classname;
	const char* m_name;
	typedef types::keymap<types::CustomDataTypeR> ConstructorMap;
	ConstructorMap m_typemap;
};

typedef boost::shared_ptr<CustomDataTypeConstructor> CustomDataTypeConstructorR;


struct CustomDataTypeDef
{
	const char* name;
	types::CreateCustomDataType createFunc;
};

class CustomDataTypeBuilder :public SimpleBuilder
{
public:
	CustomDataTypeBuilder( const char* classname_, const char* name_, const CustomDataTypeDef* typedefs)
		:SimpleBuilder(classname_)
		,m_name(name_)
	{
		std::size_t ti = 0;
		for (; typedefs[ti].name && typedefs[ti].createFunc; ++ti)
		{
			m_constructormap.insert( std::string(typedefs[ti].name), typedefs[ti].createFunc);
		}
	}

	virtual ~CustomDataTypeBuilder(){}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return ObjectConstructorBase::CUSTOM_DATA_TYPE_OBJECT;
	}

	virtual ObjectConstructorBase* constructor()
	{
		return new CustomDataTypeConstructor( objectClassName(), m_name, m_constructormap);
	}

private:
	const char* m_name;
	typedef types::keymap<types::CreateCustomDataType> ConstructorMap;
	ConstructorMap m_constructormap;
};

}}//namespace

#endif

