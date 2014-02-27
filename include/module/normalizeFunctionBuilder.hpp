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
///\file module/normalizeFunctionBuilder.hpp
///\brief Interface template for object builder of normalize functions from a description
#ifndef _Wolframe_MODULE_NORMALIZE_FUNCTION_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#define _Wolframe_MODULE_NORMALIZE_FUNCTION_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#include "types/normalizeFunction.hpp"
#include "types/keymap.hpp"
#include "module/moduleInterface.hpp"
#include "module/constructor.hpp"
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace module {

class NormalizeFunctionConstructor :public SimpleObjectConstructor< types::NormalizeFunction >
{
public:
	NormalizeFunctionConstructor( const char* classname_, const types::keymap<types::NormalizeFunctionType>& functionmap_)
		:m_classname(classname_)
		,m_functionmap(functionmap_)
	{}

	virtual ~NormalizeFunctionConstructor()
	{}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return NORMALIZE_FUNCTION_OBJECT;
	}

	virtual const char* objectClassName() const
	{
		return m_classname;
	}

	std::vector<std::string> functions() const
	{
		return m_functionmap.getkeys<std::vector<std::string> >();
	}

	typedef types::keymap<types::NormalizeFunctionType> FunctionTypeMap;
	const FunctionTypeMap& functionmap() const
	{
		return m_functionmap;
	}

private:
	const char* m_classname;
	FunctionTypeMap m_functionmap;
};


typedef boost::shared_ptr<NormalizeFunctionConstructor> NormalizeFunctionConstructorR;

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
	NormalizeFunctionBuilder( const char* classname_, const NormalizeFunctionDef* functions, types::CreateNormalizeResourceHandle createResourceHandle=0)
		:SimpleBuilder( classname_)
	{
		if (createResourceHandle)
		{
			m_resource.reset( createResourceHandle());
		}
		std::size_t fi = 0;
		for (; functions[fi].name && functions[fi].createFunc; ++fi)
		{
			types::NormalizeFunctionType functype( functions[fi].createFunc, m_resource);
			m_functionmap.insert( std::string(functions[fi].name), functype);
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
		return new NormalizeFunctionConstructor( m_className, m_functionmap);
	}

private:
	types::keymap<types::NormalizeFunctionType> m_functionmap;
	types::NormalizeResourceHandleR m_resource;
};

}}//namespace

#endif

