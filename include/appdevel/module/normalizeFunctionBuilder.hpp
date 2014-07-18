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
/// \file appdevel/module/normalizeFunctionBuilder.hpp
/// \brief Interface template for object builder of normalize functions from a description
#ifndef _Wolframe_MODULE_NORMALIZE_FUNCTION_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#define _Wolframe_MODULE_NORMALIZE_FUNCTION_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#include "types/normalizeFunction.hpp"
#include "types/keymap.hpp"
#include "module/moduleInterface.hpp"
#include "module/constructor.hpp"
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace module {

/// \class NormalizeFunctionConstructor
/// \brief Constructor of a normalization function type
class NormalizeFunctionConstructor
	:public SimpleObjectConstructor< types::NormalizeFunction >
{
public:
	NormalizeFunctionConstructor( const char* classname_, const std::string& identifier_, const types::NormalizeFunctionType& function_)
		:m_classname(classname_)
		,m_identifier(identifier_)
		,m_function(function_)
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

	const std::string& identifier() const
	{
		return m_identifier;
	}

	const types::NormalizeFunctionType& function() const
	{
		return m_function;
	}

private:
	const char* m_classname;
	std::string m_identifier;
	types::NormalizeFunctionType m_function;
};

typedef boost::shared_ptr<NormalizeFunctionConstructor> NormalizeFunctionConstructorR;


/// \class NormalizeFunctionBuilder
/// \brief Builder of a normalization function type constructor
class NormalizeFunctionBuilder :public SimpleBuilder
{
public:
	/// \brief Constructor
	/// \param[in] classname_ unique object name in the system (C++ name base)
	/// \param[in] identifier_ object name to reference the function
	/// \param[in] createFunc_ function creating the normalization function object
	/// \param[in] resource_ resource object (module singleton) shared by all instances using this resource
	NormalizeFunctionBuilder( const char* classname_, const char* identifier_, types::CreateNormalizeFunction createFunc_, const types::NormalizeResourceHandleR& resource_)
		:SimpleBuilder( classname_),m_identifier(identifier_),m_function(types::NormalizeFunctionType(createFunc_,resource_))
	{}
	/// \brief Constructor
	/// \param[in] classname_ unique object name in the system (C++ name base)
	/// \param[in] identifier_ object name to reference the function
	/// \param[in] createFunc_ function creating the normalization function object
	NormalizeFunctionBuilder( const char* classname_, const char* identifier_, types::CreateNormalizeFunction createFunc_)
		:SimpleBuilder( classname_),m_identifier(identifier_),m_function(types::NormalizeFunctionType(createFunc_))
	{}

	virtual ~NormalizeFunctionBuilder()
	{}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return ObjectConstructorBase::NORMALIZE_FUNCTION_OBJECT;
	}

	virtual ObjectConstructorBase* constructor()
	{
		return new NormalizeFunctionConstructor( m_className, m_identifier, m_function);
	}

private:
	std::string m_identifier;
	types::NormalizeFunctionType m_function;
};

}}//namespace

#endif

