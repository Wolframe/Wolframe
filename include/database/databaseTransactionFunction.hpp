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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\brief Interface to transactions in the database for language bindings
///\file database/databaseTransactionFunction.hpp
#ifndef _LANGBIND_DATABASE_TRANSACTION_FUNCTION_HPP_INCLUDED
#define _LANGBIND_DATABASE_TRANSACTION_FUNCTION_HPP_INCLUDED
#include "types/allocators.hpp"
#include "langbind/transactionFunction.hpp"
#include "filter/typedfilter.hpp"
#include "database/transactionInput.hpp"
#include "database/transactionOutput.hpp"
#include <string>
#include <vector>
#include <map>
#include <limits>
#include <stdexcept>

namespace _Wolframe {
namespace db {

class DatabaseTransactionFunction;

class TransactionFunctionOutput
	:public langbind::TypedInputFilter
{
public:
	TransactionFunctionOutput( const std::string& rootname_, const std::vector<std::string>& resname_, const db::TransactionOutput& data_);
	virtual ~TransactionFunctionOutput();

	virtual bool getNext( ElementType& type, TypedFilterBase::Element& element);

private:
	struct Impl;
	Impl* m_impl;
};


class TransactionFunctionInput
	:public langbind::TypedOutputFilter
{
public:
	class Structure;

	explicit TransactionFunctionInput( const DatabaseTransactionFunction* func_);
	TransactionFunctionInput( const TransactionFunctionInput& o);
	virtual ~TransactionFunctionInput();

	virtual bool print( ElementType type, const Element& element);
	virtual TransactionInput get() const;

	const Structure& structure() const
	{
		return *m_structure;
	}

private:
	Structure* m_structure;
	const DatabaseTransactionFunction* m_func;
	langbind::TypedInputFilter::ElementType m_lasttype;
};


struct TransactionDescription
{
	TransactionDescription(){}
	TransactionDescription( const TransactionDescription& o)
		:selector(o.selector)
		,call(o.call)
		,output(o.output){}

	void clear()
	{
		selector.clear();
		call.clear();
		output.clear();
	}

	enum ElementName
	{
		Selector,Call,Output
	};

	///\class Error
	///\brief Error thrown by createDatabaseTransactionFunction( const proc::ProcessorProvider*,const std::vector<>&);
	struct Error
	{
		Error( ElementName elemname_, std::size_t elemidx_, const std::string& msg_)
			:elemname(elemname_)
			,elemidx(elemidx_)
			,msg(msg_){}

		ElementName elemname;
		std::size_t elemidx;
		std::string msg;
	};

	std::string name;
	std::string selector;
	std::string call;
	std::string output;
};

class DatabaseTransactionFunction
	:public langbind::TransactionFunction
{
public:
	DatabaseTransactionFunction( const DatabaseTransactionFunction& o);
	DatabaseTransactionFunction( const std::vector<TransactionDescription>& description);
	virtual ~DatabaseTransactionFunction();

	virtual langbind::TypedOutputFilter* getInput() const;
	virtual langbind::TypedInputFilter* getOutput( const db::TransactionOutput& o) const;

	class TagTable;
	const TagTable* tagmap() const;

	struct Impl;
	const Impl& impl() const
	{
		return *m_impl;
	}

private:
	Impl* m_impl;
};

///\brief Creates a database transaction function from its description source
///\param[in] description transaction description source
langbind::TransactionFunction* createDatabaseTransactionFunction( const std::vector<TransactionDescription>& description);

}}//namespace
#endif

