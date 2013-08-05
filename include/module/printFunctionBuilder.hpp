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
///\file module/printFunctionBuilder.hpp
///\brief Interface template for object builder of print functions from a layout description
#ifndef _Wolframe_MODULE_PRINT_FUNCTION_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#define _Wolframe_MODULE_PRINT_FUNCTION_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#include "prnt/printFunction.hpp"
#include "processor/moduleInterface.hpp"
#include "module/constructor.hpp"

namespace _Wolframe {
namespace module {

class PrintFunctionConstructor :public SimpleObjectConstructor< prnt::PrintFunction >
{
public:
	PrintFunctionConstructor( const char* classname_, const char* name_, prnt::CreatePrintFunction createFunc_ )
		:m_classname(classname_)
		,m_name(name_)
		,m_createFunc(createFunc_) {}

	virtual ~PrintFunctionConstructor(){}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return PRINT_FUNCTION_OBJECT;
	}
	virtual const char* objectClassName() const
	{
		return m_classname.c_str();
	}
	virtual const char* programFileType() const
	{
		return m_name.c_str();
	}
	virtual prnt::PrintFunction* object( const std::string& layout_source) const
	{
		return m_createFunc( layout_source);
	}
	const std::string& name() const
	{
		return m_name;
	}

private:
	std::string m_classname;
	std::string m_name;
	prnt::CreatePrintFunction m_createFunc;
};

typedef boost::shared_ptr<module::PrintFunctionConstructor> PrintFunctionConstructorR;


class PrintFunctionBuilder :public SimpleBuilder
{
public:
	PrintFunctionBuilder( const char* classname_, const char* name_, prnt::CreatePrintFunction createFunc_)
		:SimpleBuilder( classname_)
		,m_name( name_)
		,m_createFunc(createFunc_){}

	virtual ~PrintFunctionBuilder(){}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return ObjectConstructorBase::PRINT_FUNCTION_OBJECT;
	}
	virtual ObjectConstructorBase* constructor()
	{
		return new PrintFunctionConstructor( m_className, m_name, m_createFunc);
	}
	const char* name() const
	{
		return m_name;
	}

private:
	const char* m_name;
	prnt::CreatePrintFunction m_createFunc;
};

}}//namespace

#endif

