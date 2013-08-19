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
#include "types/variant.hpp"
#include "filter/typedfilter.hpp"
#include "database/transactionInput.hpp"
#include "database/transactionOutput.hpp"
#include "langbind/authorization.hpp"
#include <string>
#include <vector>
#include <map>
#include <limits>
#include <stdexcept>

namespace _Wolframe {
namespace db {

///\class TransactionFunction
///\brief Forward Declaration
class TransactionFunction;

///\class TransactionFunctionOutput
///\brief Output structure of transaction for language bindings
class TransactionFunctionOutput
	:public langbind::TypedInputFilter
{
public:
	class ResultStruct;
	typedef types::CountedReference<ResultStruct> ResultStructR;

	TransactionFunctionOutput( const ResultStructR& resultstruct_, const db::TransactionOutput& data_);
	TransactionFunctionOutput( const TransactionFunctionOutput& o);
	virtual ~TransactionFunctionOutput();

	///\brief Get a self copy
	///\return allocated pointer to copy of this
	virtual TypedInputFilter* copy() const		{return new TransactionFunctionOutput(*this);}

	virtual bool getNext( ElementType& type, types::VariantConst& element);
	virtual void resetIterator();

private:
	struct Impl;
	Impl* m_impl;
};

///\class TransactionFunctionInput
///\brief Input structure of transaction for language bindings
class TransactionFunctionInput
	:public langbind::TypedOutputFilter
{
public:
	class Structure;
	typedef types::CountedReference<Structure> StructureR;

	explicit TransactionFunctionInput( const TransactionFunction* func_);
	TransactionFunctionInput( const TransactionFunctionInput& o);
	virtual ~TransactionFunctionInput();

	///\brief Get a self copy
	///\return allocated pointer to copy of this
	virtual TypedOutputFilter* copy() const		{return new TransactionFunctionInput(*this);}

	virtual bool print( ElementType type, const types::VariantConst& element);
	virtual TransactionInput get() const;

	const Structure& structure() const
	{
		return *m_structure.get();
	}
	const TransactionFunction* func() const
	{
		return m_func;
	}

private:
	StructureR m_structure;
	const TransactionFunction* m_func;
	langbind::TypedInputFilter::ElementType m_lasttype;
};


class VariableValue
{
public:
	VariableValue()
		:m_scope_functionidx(-1),m_column_idx(-1){}
	VariableValue( const VariableValue& o)
		:m_scope_functionidx(o.m_scope_functionidx),m_column_idx(o.m_column_idx),m_name(o.m_name){}
	VariableValue( const std::string& name_, int scope_functionidx_)
		:m_scope_functionidx(scope_functionidx_),m_column_idx(-1),m_name(name_){}
	VariableValue( int column_idx_, int scope_functionidx_)
		:m_scope_functionidx(scope_functionidx_),m_column_idx(column_idx_){}

	int scope_functionidx() const			{return m_scope_functionidx;}
	int column_idx() const				{return m_column_idx;}
	const std::string& name() const			{return m_name;}

	bool isConstant() const				{return m_column_idx == 0;}
	const std::string& value() const		{return m_name;}

private:
	friend class ConstantValue;
	int m_scope_functionidx;
	int m_column_idx;
	std::string m_name;
};

struct ConstantValue :public VariableValue
{
	ConstantValue( const std::string& value_)
		:VariableValue( value_, -1)		{m_scope_functionidx = 0;}
};

typedef types::keymap<VariableValue>	VariableTable;


class TransactionFunctionDescription
{
public:
	class OperationStep
	{
	public:
		OperationStep()
			:nonempty(false)
			,unique(false){}

		OperationStep( const OperationStep& o)
			:selector_FOREACH(o.selector_FOREACH)
			,call(o.call)
			,path_INTO(o.path_INTO)
			,nonempty(o.nonempty)
			,unique(o.unique)
			,hints(o.hints){}

		void clear()
		{
			selector_FOREACH.clear();
			path_INTO.clear();
			call.clear();
			nonempty = false;
			unique = false;
			hints.clear();
		}

		///\class Error
		///\brief Error thrown by createTransactionFunction( const proc::ProcessorProvider*,const std::vector<>&);
		struct Error
		{
			Error( std::size_t elemidx_, const std::string& msg_)
				:elemidx(elemidx_)
				,msg(msg_){}

			std::size_t elemidx;
			std::string msg;
		};

		struct Call
		{
			struct Param
			{
				enum Type
				{
					VariableReference,
					NumericResultReference,
					SymbolicResultReference,
					Constant,
					InputSelectorPath
				};
				static const char* typeName( Type i)
				{
					static const char* ar[] = {"VariableReference","NumericResultReference","SymbolicResultReference","Constant","InputSelectorPath"};
					return ar[(std::size_t)i];
				}
				Type type;
				std::string value;

				Param(){}
				Param( const Param& o)
					:type(o.type),value(o.value){}
				Param( Type type_, const std::string& value_)
					:type(type_),value(value_){}
			};

			std::string funcname;
			std::vector<Param> paramlist;

			Call(){}
			Call( const Call& o)
				:funcname(o.funcname),paramlist(o.paramlist){}
			Call( const std::string& funcname_, const std::vector<Param>& paramlist_)
				:funcname(funcname_),paramlist(paramlist_){}

			void clear()		{funcname.clear(); paramlist.clear();}
		};

		std::string selector_FOREACH;
		Call call;
		std::vector<std::string> path_INTO;
		bool nonempty;
		bool unique;
		types::keymap<std::string> hints;
	};
	struct Block
	{
		std::vector<std::string> path_INTO;
		std::size_t startidx;
		std::size_t size;

		Block( const std::vector<std::string>& p, std::size_t i, std::size_t n)
			:path_INTO(p),startidx(i),size(n){}
		Block( const Block& o)
			:path_INTO(o.path_INTO),startidx(o.startidx),size(o.size){}
		Block()
			:startidx(0),size(0){}
	};
	std::vector<OperationStep> steps;	//< list of database commands or operations
	std::vector<Block> blocks;		//< substructures of the output
	langbind::Authorization auth;		//< authorization definition structure for this function
	VariableTable variablemap;		//< variable definitions with LET a = ...;

	TransactionFunctionDescription( const TransactionFunctionDescription& o)
		:steps(o.steps),blocks(o.blocks),auth(o.auth){}
	TransactionFunctionDescription(){}
};

///\class TransactionFunctionR
///\brief Reference to transaction function
typedef types::CountedReference<TransactionFunction> TransactionFunctionR;

///\class TransactionFunction
///\brief Transaction function definition
class TransactionFunction
{
public:
	TransactionFunction( const TransactionFunction& o);
	TransactionFunction( const std::string& name_, const TransactionFunctionDescription& description, const types::keymap<TransactionFunctionR>& functionmap);
	virtual ~TransactionFunction();

	virtual TransactionFunctionInput* getInput() const;
	virtual TransactionFunctionOutput* getOutput( const db::TransactionOutput& o) const;

	const std::string& name() const			{return m_name;}
	void name( const std::string& name_)		{m_name = name_;}

	struct Impl;
	const Impl& impl() const
	{
		return *m_impl;
	}

	const langbind::Authorization& authorization() const
	{
		return m_authorization;
	}

	const char* getErrorHint( const std::string& errorclass, int functionidx) const;

private:
	std::string m_name;
	langbind::Authorization m_authorization;
	Impl* m_impl;
};

///\brief Creates a database transaction function from its description source
///\param[in] name name of  the transaction
///\param[in] description description of the function
///\param[in] functionmap map op operations in the module context that can be referenced
TransactionFunction* createTransactionFunction( const std::string& name, const TransactionFunctionDescription& description, const types::keymap<TransactionFunctionR>& functionmap);

}}//namespace
#endif

