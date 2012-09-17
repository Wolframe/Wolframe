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
///\file module/transactionFunctionBuilder.hpp
///\brief Interface template for object builder of transaction functions from a description
#ifndef _Wolframe_MODULE_TRANSACTION_FUNCTION_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#define _Wolframe_MODULE_TRANSACTION_FUNCTION_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#include "langbind/transactionFunction.hpp"
#include "moduleInterface.hpp"
#include "constructor.hpp"

namespace _Wolframe {
namespace module {

class TransactionFunctionConstructor :public SimpleObjectConstructor< langbind::TransactionFunction >
{
public:
	TransactionFunctionConstructor( const char* name_, langbind::CreateTransactionFunction createFunc_ )
		:m_name(name_)
		,m_provider(0)
		,m_createFunc(createFunc_) {}

	virtual ~TransactionFunctionConstructor(){}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return TRANSACTION_FUNCTION_OBJECT;
	}

	virtual const char* identifier() const
	{
		return m_name.c_str();
	}

	virtual langbind::TransactionFunction* object( const std::string& description) const
	{
		return m_createFunc( m_provider, description);
	}

	void setDatabaseProvider( db::DatabaseProvider* provider_)
	{
		m_provider = provider_;
	}

private:
	const std::string m_name;
	db::DatabaseProvider* m_provider;
	const langbind::CreateTransactionFunction m_createFunc;
};

class TransactionFunctionBuilder :public SimpleBuilder
{
public:
	TransactionFunctionBuilder( const char* name_, langbind::CreateTransactionFunction createFunc_)
		:SimpleBuilder( name_)
		,m_createFunc(createFunc_){}

	virtual ~TransactionFunctionBuilder(){}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return ObjectConstructorBase::TRANSACTION_FUNCTION_OBJECT;
	}
	virtual ObjectConstructorBase* constructor()
	{
		return new TransactionFunctionConstructor( m_builderName, m_createFunc);
	}
private:
	const langbind::CreateTransactionFunction m_createFunc;
};

}}//namespace

#endif

