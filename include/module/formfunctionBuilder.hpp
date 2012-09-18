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
///\file module/formfunctionBuilder.hpp
///\brief Interface template for object builder of form functions
#ifndef _Wolframe_MODULE_FORMFUNCTION_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#define _Wolframe_MODULE_FORMFUNCTION_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#include "langbind/appFormFunction.hpp"
#include "moduleInterface.hpp"
#include "constructor.hpp"

namespace _Wolframe {
namespace module {

class FormFunctionConstructor :public SimpleObjectConstructor< langbind::FormFunction >
{
public:
	FormFunctionConstructor( const char* name_, const langbind::FormFunction& func_)
		: m_name(name_)
		, m_func(func_) {}

	virtual ~FormFunctionConstructor(){}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return FORM_FUNCTION_OBJECT;
	}
	virtual const char* objectClassName() const
	{
		return m_name.c_str();
	}
	virtual langbind::FormFunction* object() const
	{
		return new langbind::FormFunction( m_func);
	}

private:
	const std::string m_name;
	const langbind::FormFunction m_func;
};

class FormFunctionBuilder :public SimpleBuilder
{
public:
	FormFunctionBuilder( const char* name_, const langbind::FormFunction& func_)
		:SimpleBuilder( name_)
		,m_func(func_){}

	virtual ~FormFunctionBuilder(){}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return ObjectConstructorBase::FORM_FUNCTION_OBJECT;
	}
	virtual ObjectConstructorBase* constructor()
	{
		return new FormFunctionConstructor( m_className, m_func);
	}
private:
	const langbind::FormFunction m_func;
};

}}//namespace

#endif

