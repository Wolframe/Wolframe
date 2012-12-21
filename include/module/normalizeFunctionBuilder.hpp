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
///\file module/normalizeFunctionBuilder.hpp
///\brief Interface template for object builder of normalize functions from a description
#ifndef _Wolframe_MODULE_NORMALIZE_FUNCTION_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#define _Wolframe_MODULE_NORMALIZE_FUNCTION_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#include "langbind/normalizeFunction.hpp"
#include "moduleInterface.hpp"
#include "types/countedReference.hpp"
#include "constructor.hpp"

namespace _Wolframe {
namespace module {

class NormalizeFunctionConstructor :public SimpleObjectConstructor< langbind::NormalizeFunction >
{
public:
	NormalizeFunctionConstructor( const char* classname_, const char* name_, langbind::GetNormalizeFunctions getNormalizeFunctions_, langbind::ResourceHandle* rh, langbind::CreateNormalizeFunction createFunc_)
		:m_classname(classname_)
		,m_name(name_)
		,m_resource(rh)
		,m_createFunc(createFunc_)
		,m_getNormalizeFunctions(getNormalizeFunctions_) {}

	virtual ~NormalizeFunctionConstructor(){}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return NORMALIZE_FUNCTION_OBJECT;
	}

	virtual const char* objectClassName() const
	{
		return m_classname;
	}

	virtual langbind::NormalizeFunction* object( const std::string& name_, const std::string& arg_) const
	{
		return m_createFunc( *m_resource, name_, arg_);
	}

	const std::vector<std::string>& functions() const
	{
		return m_getNormalizeFunctions();
	}

	const char* domain() const
	{
		return m_name;
	}

private:
	const char* m_classname;
	const char* m_name;
	langbind::ResourceHandle* m_resource;
	langbind::CreateNormalizeFunction m_createFunc;
	langbind::GetNormalizeFunctions m_getNormalizeFunctions;
};

typedef types::CountedReference<NormalizeFunctionConstructor> NormalizeFunctionConstructorR;


class NormalizeFunctionBuilder :public SimpleBuilder
{
public:
	NormalizeFunctionBuilder( const char* classname_, const char* name_, langbind::GetNormalizeFunctions getNormalizeFunctions_, langbind::CreateNormalizeFunction createFunc_, langbind::ResourceHandle* r=0)
		:SimpleBuilder( classname_)
		,m_name(name_)
		,m_getNormalizeFunctions(getNormalizeFunctions_)
		,m_createFunc(createFunc_)
		,m_resource(r){}

	virtual ~NormalizeFunctionBuilder()
	{}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return ObjectConstructorBase::NORMALIZE_FUNCTION_OBJECT;
	}

	virtual ObjectConstructorBase* constructor()
	{
		return new NormalizeFunctionConstructor( m_className, m_name, m_getNormalizeFunctions, m_resource, m_createFunc);
	}

private:
	const char* m_name;
	langbind::GetNormalizeFunctions m_getNormalizeFunctions;
	langbind::CreateNormalizeFunction m_createFunc;
	langbind::ResourceHandle* m_resource;
};

}}//namespace

#endif

