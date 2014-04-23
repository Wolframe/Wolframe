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
///\file appdevel/module/cppFormFunctionFunctionBuilder.hpp
///\brief Interface template for object builder of built-in functions
#ifndef _Wolframe_MODULE_CPP_FORM_FUNCTION_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#define _Wolframe_MODULE_CPP_FORM_FUNCTION_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#include "serialize/cppFormFunction.hpp"
#include "module/moduleInterface.hpp"
#include "module/constructor.hpp"
#include "types/keymap.hpp"
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace module {


struct CppFormFunctionDef
{
	const char* name;
	serialize::CppFormFunction func;
};

class CppFormFunctionConstructor :public SimpleObjectConstructor< serialize::CppFormFunction >
{
public:
	CppFormFunctionConstructor( const std::string& classname_, const std::string& name_, const serialize::CppFormFunction& function_)
		:m_classname(classname_),m_name(name_),m_function(function_)
	{}

	virtual ~CppFormFunctionConstructor(){}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return FORM_FUNCTION_OBJECT;
	}

	virtual const char* objectClassName() const
	{
		return m_classname.c_str();
	}

	const std::string& identifier() const
	{
		return m_name;
	}

	const serialize::CppFormFunction function() const
	{
		return m_function;
	}

private:
	std::string m_classname;
	std::string m_name;
	serialize::CppFormFunction m_function;
};

typedef boost::shared_ptr<CppFormFunctionConstructor> CppFormFunctionConstructorR;


class CppFormFunctionBuilder :public SimpleBuilder
{
public:
	CppFormFunctionBuilder( const char* className_, const char* name_, const serialize::CppFormFunction& function_)
		:SimpleBuilder( className_)
		,m_name(name_)
		,m_function(function_)
		  
	{}

	virtual ~CppFormFunctionBuilder(){}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return ObjectConstructorBase::FORM_FUNCTION_OBJECT;
	}
	virtual ObjectConstructorBase* constructor()
	{
		return new CppFormFunctionConstructor( m_className, m_name, m_function);
	}
private:
	std::string m_name;
	serialize::CppFormFunction m_function;
};

}}//namespace

#endif

