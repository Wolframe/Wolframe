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
//\file types/customDataNormalizer.hpp
//\brief Custom data normalization function
#ifndef _Wolframe_TYPES_CUSTOM_DATA_NORMALIZER_HPP_INCLUDED
#define _Wolframe_TYPES_CUSTOM_DATA_NORMALIZER_HPP_INCLUDED
#include "types/customDataType.hpp"
#include "types/normalizeFunction.hpp"
#include "types/variant.hpp"
#include <string>
#include <vector>

namespace _Wolframe {
namespace types {

//\class CustomDataNormalizer
//\brief Normalizer function created from a custom data type initializer
class CustomDataNormalizer
	:public types::NormalizeFunction
{
public:
	//\brief Copy constructor
	CustomDataNormalizer( const CustomDataNormalizer& o)
		:m_name(o.m_name)
		,m_type(o.m_type)
		,m_initializer(o.m_initializer)
	{}
	//\brief Constructor
	CustomDataNormalizer( const std::string& name_, const std::vector<types::Variant>& arg, const types::CustomDataType* type_)
		:m_name(name_)
		,m_type(type_)
		,m_initializer(type_->hasInitializer()?type_->createInitializer(arg):0)
	{
		if (!arg.empty() && !type_->hasInitializer())
		{
			throw std::runtime_error("arguments passed to custom data type without initializer defined");
		}
	}

	//\brief Destructor
	virtual ~CustomDataNormalizer()
	{
		if (m_initializer) delete m_initializer;
	}

	//\brief Implementation of types::NormalizeFunction::name()const
	virtual const char* name() const
	{
		return m_name.c_str();
	}

	//\brief Get the custom data type
	const types::CustomDataType* type() const
	{
		return m_type;
	}

	//\brief Implementation of types::NormalizeFunction::execute(const types::Variant&)const
	types::Variant execute( const types::Variant& i) const
	{
		types::Variant rt( m_type, m_initializer);
		if (i.defined())
		{
			rt.data().value.Custom->assign( i);
		}
		return rt;
	}

	//\brief Implementation of types::NormalizeFunction::copy()const
	virtual types::NormalizeFunction* copy() const
	{
		return new CustomDataNormalizer(*this);
	}

private:
	std::string m_name;
	const types::CustomDataType* m_type;
	types::CustomDataInitializer* m_initializer;
};


//\class CustomDataMethodCallNormalizer
//\brief Normalizer function created from a custom data method call
class CustomDataMethodCallNormalizer
	:public types::NormalizeFunction
{
public:
	CustomDataMethodCallNormalizer( const CustomDataMethodCallNormalizer& o)
		:m_name(o.m_name)
		,m_arg(o.m_arg)
	{}
	CustomDataMethodCallNormalizer( const std::string& name_, const std::vector<types::Variant>& arg_)
		:m_name(name_)
		,m_arg(arg_)
	{}

	virtual ~CustomDataMethodCallNormalizer()
	{}

	virtual const char* name() const
	{
		return m_name.c_str();
	}

	types::Variant execute( const types::Variant& i) const
	{
		const CustomDataValue* cv = i.customref();
		const CustomDataType* dt = cv->type();
		CustomDataValueMethod method = dt->getMethod( m_name);
		if (!method) throw std::logic_error( "internal: calling undefined method");
		return method( *cv, m_arg);
	}

	virtual types::NormalizeFunction* copy() const
	{
		return new CustomDataMethodCallNormalizer(*this);
	}

private:
	std::string m_name;
	std::vector<types::Variant> m_arg;
};

}}//namespace
#endif


