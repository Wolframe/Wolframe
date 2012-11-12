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
///\file database/transactionFunction.hpp
#ifndef _DATABASE_TRANSACTION_FUNCTION_HPP_INCLUDED
#define _DATABASE_TRANSACTION_FUNCTION_HPP_INCLUDED
#include "types/countedReference.hpp"
#include "types/keymap.hpp"
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

class TransactionFunction;

class TransactionFunctionOutput
	:public langbind::TypedInputFilter
{
public:
	TransactionFunctionOutput( const std::string& rootname_, const std::vector<std::string>& resname_, const std::vector<bool>& resunique_, const db::TransactionOutput& data_);
	virtual ~TransactionFunctionOutput();

	virtual bool getNext( ElementType& type, TypedFilterBase::Element& element);
	virtual void resetIterator();

private:
	struct Impl;
	Impl* m_impl;
};


class TransactionFunctionInput
	:public langbind::TypedOutputFilter
{
public:
	class Structure;

	explicit TransactionFunctionInput( const TransactionFunction* func_);
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
	const TransactionFunction* m_func;
	langbind::TypedInputFilter::ElementType m_lasttype;
};


struct TransactionDescription
{
	TransactionDescription()
		:nonempty(false)
		,unique(false){}

	TransactionDescription( const TransactionDescription& o)
		:selector(o.selector)
		,call(o.call)
		,output(o.output)
		,nonempty(o.nonempty)
		,unique(o.unique){}

	void clear()
	{
		selector.clear();
		call.clear();
		output.clear();
		nonempty = false;
		unique = false;
	}

	enum ElementName
	{
		Selector,Call,Output
	};

	///\class Error
	///\brief Error thrown by createTransactionFunction( const proc::ProcessorProvider*,const std::vector<>&);
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

	std::string selector;
	std::string call;
	std::string output;
	bool nonempty;
	bool unique;
};

class TransactionFunction;
typedef types::CountedReference<TransactionFunction> TransactionFunctionR;

class TransactionFunction
{
public:
	TransactionFunction( const TransactionFunction& o);
	TransactionFunction( const std::string& name_, const std::vector<TransactionDescription>& description, const types::keymap<TransactionFunctionR>& functionmap);
	virtual ~TransactionFunction();

	virtual TransactionFunctionInput* getInput() const;
	virtual TransactionFunctionOutput* getOutput( const db::TransactionOutput& o) const;

	const std::string& name() const			{return m_name;}
	void name( const std::string& name_)		{m_name = name_;}

	class TagTable;
	const TagTable* tagmap() const;

	struct Impl;
	const Impl& impl() const
	{
		return *m_impl;
	}

private:
	std::string m_name;
	Impl* m_impl;
};

///\brief Creates a database transaction function from its description source
///\param[in] name name of  the transaction
///\param[in] description transaction description source
TransactionFunction* createTransactionFunction( const std::string& name, const std::vector<TransactionDescription>& description, const types::keymap<TransactionFunctionR>& functionmap);

}}//namespace
#endif

