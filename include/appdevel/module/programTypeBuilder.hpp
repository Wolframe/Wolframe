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
///\file appdevel/module/programTypeBuilder.hpp
///\brief Interface template for object builder of arbitrary program types implementing form functions
#ifndef _Wolframe_MODULE_PROGRAM_TYPE_BUILDER_TEMPLATE_HPP_INCLUDED
#define _Wolframe_MODULE_PROGRAM_TYPE_BUILDER_TEMPLATE_HPP_INCLUDED
#include "prgbind/program.hpp"
#include "module/moduleInterface.hpp"
#include "module/constructor.hpp"
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace module {

typedef prgbind::Program* (*CreateProgramType)();

class ProgramTypeConstructor :public SimpleObjectConstructor< prgbind::Program >
{
public:
	ProgramTypeConstructor( const char* classname_, const char* name_, CreateProgramType createFunc_ )
		: m_classname(classname_)
		, m_name(name_)
		, m_createFunc(createFunc_) {}

	virtual ~ProgramTypeConstructor(){}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return PROGRAM_TYPE_OBJECT;
	}
	virtual const char* objectClassName() const
	{
		return m_classname.c_str();
	}
	virtual const char* programFileType() const
	{
		return m_name.c_str();
	}
	virtual prgbind::Program* object() const
	{
		return m_createFunc();
	}
	const std::string& name() const
	{
		return m_name;
	}
private:
	std::string m_classname;
	std::string m_name;
	CreateProgramType m_createFunc;
};

typedef boost::shared_ptr<ProgramTypeConstructor> ProgramTypeConstructorR;


class ProgramTypeBuilder :public SimpleBuilder
{
public:
	ProgramTypeBuilder( const char* classname_, const char* name_, CreateProgramType createFunc_)
		:SimpleBuilder( classname_)
		,m_name( name_)
		,m_createFunc(createFunc_){}

	virtual ~ProgramTypeBuilder(){}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return ObjectConstructorBase::PROGRAM_TYPE_OBJECT;
	}
	virtual ObjectConstructorBase* constructor()
	{
		return new ProgramTypeConstructor( m_className, m_name, m_createFunc);
	}
	const char* name() const
	{
		return m_name;
	}

private:
	const char* m_name;
	CreateProgramType m_createFunc;
};

}}//namespace

#endif

