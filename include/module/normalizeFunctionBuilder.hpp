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
///\file module/normalizeFunctionBuilder.hpp
///\brief Interface template for object builder of normalize functions from a description
#ifndef _Wolframe_MODULE_NORMALIZE_FUNCTION_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#define _Wolframe_MODULE_NORMALIZE_FUNCTION_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#include "types/normalizeFunction.hpp"
#include "processor/moduleInterface.hpp"
#include "types/countedReference.hpp"
#include "types/keymap.hpp"
#include "module/constructor.hpp"

namespace _Wolframe {
namespace module {

class NormalizeFunctionConstructor :public SimpleObjectConstructor< types::NormalizeFunction >
{
public:
	NormalizeFunctionConstructor( const char* classname_, const char* name_, const types::keymap<types::CreateNormalizeFunction>& functionmap_, types::CreateNormalizeResourceHandleFunction createResourceHandle)
		:m_classname(classname_)
		,m_name(name_)
		,m_resource(createResourceHandle?createResourceHandle():0)
		,m_functionmap(functionmap_)
		{}

	virtual ~NormalizeFunctionConstructor()
	{
		if (m_resource) delete m_resource;
	}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return NORMALIZE_FUNCTION_OBJECT;
	}

	virtual const char* objectClassName() const
	{
		return m_classname;
	}

	virtual types::NormalizeFunction* object( const std::string& name_, const std::string& arg_) const
	{
		FunctionMap::const_iterator fi = m_functionmap.find( name_);
		if (fi == m_functionmap.end()) return 0;
		return fi->second( m_resource, arg_);
	}

	std::vector<std::string> functions() const
	{
		return m_functionmap.getkeys<std::vector<std::string> >();
	}

	const char* domain() const
	{
		return m_name;
	}

private:
	const char* m_classname;
	const char* m_name;
	types::NormalizeResourceHandle* m_resource;
	typedef types::keymap<types::CreateNormalizeFunction> FunctionMap;
	FunctionMap m_functionmap;
};


typedef types::CountedReference<NormalizeFunctionConstructor> NormalizeFunctionConstructorR;

struct NormalizeFunctionDef
{
	const char* name;
	types::CreateNormalizeFunction createFunc;
};

class NormalizeFunctionBuilder :public SimpleBuilder
{
public:
	//\brief Constructor
	//\param[in] functions {0,0} terminated array of function definitions
	NormalizeFunctionBuilder( const char* classname_, const char* name_, const NormalizeFunctionDef* functions, types::CreateNormalizeResourceHandleFunction createResourceHandle_=0)
		:SimpleBuilder( classname_)
		,m_name(name_)
		,m_createResourceHandle(createResourceHandle_)
	{
		std::size_t fi = 0;
		for (; functions[fi].name && functions[fi].createFunc; ++fi)
		{
			m_functionmap.insert( std::string(functions[fi].name), functions[fi].createFunc);
		}
	}

	virtual ~NormalizeFunctionBuilder()
	{}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return ObjectConstructorBase::NORMALIZE_FUNCTION_OBJECT;
	}

	virtual ObjectConstructorBase* constructor()
	{
		return new NormalizeFunctionConstructor( m_className, m_name, m_functionmap, m_createResourceHandle);
	}

private:
	const char* m_name;
	types::keymap<types::CreateNormalizeFunction> m_functionmap;
	types::CreateNormalizeResourceHandleFunction m_createResourceHandle;
};

}}//namespace

#endif

