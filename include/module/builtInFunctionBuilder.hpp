/************************************************************************
Copyright (C) 2011, 2012 Project Wolframe.
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
///\file module/builtInFunctionBuilder.hpp
///\brief Interface template for object builder of built-in functions
#ifndef _Wolframe_MODULE_BUILT_IN_FUNCTION_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#define _Wolframe_MODULE_BUILT_IN_FUNCTION_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#include "langbind/builtInFunction.hpp"
#include "moduleInterface.hpp"
#include "constructor.hpp"

namespace _Wolframe {
namespace module {

class BuiltInFunctionConstructor :public SimpleObjectConstructor< langbind::BuiltInFunction >
{
public:
	BuiltInFunctionConstructor( const char* name_, const langbind::BuiltInFunction& func_)
		: m_name(name_)
		, m_func(func_) {}

	virtual ~BuiltInFunctionConstructor(){}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return FORM_FUNCTION_OBJECT;
	}
	virtual const char* objectClassName() const
	{
		return m_name.c_str();
	}
	virtual langbind::BuiltInFunction* object() const
	{
		return new langbind::BuiltInFunction( m_func);
	}

private:
	const std::string m_name;
	const langbind::BuiltInFunction m_func;
};

class BuiltInFunctionBuilder :public SimpleBuilder
{
public:
	BuiltInFunctionBuilder( const char* name_, const langbind::BuiltInFunction& func_)
		:SimpleBuilder( name_)
		,m_func(func_){}

	virtual ~BuiltInFunctionBuilder(){}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return ObjectConstructorBase::FORM_FUNCTION_OBJECT;
	}
	virtual ObjectConstructorBase* constructor()
	{
		return new BuiltInFunctionConstructor( m_className, m_func);
	}
private:
	const langbind::BuiltInFunction m_func;
};

}}//namespace

#endif

