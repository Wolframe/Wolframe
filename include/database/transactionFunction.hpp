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
	class ResultStructure;
	typedef types::CountedReference<ResultStructure> ResultStructureR;

	TransactionFunctionOutput( const ResultStructureR& resultstruct_, const db::TransactionOutput& data_);
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


///\class VariableValue
///\brief Variable value referencing a command result or a constant
class VariableValue
{
public:
	///\brief Default constructor
	VariableValue()
		:m_scope_functionidx(-1),m_column_idx(-1){}
	///\brief Copy constructor
	VariableValue( const VariableValue& o)
		:m_scope_functionidx(o.m_scope_functionidx),m_column_idx(o.m_column_idx),m_name(o.m_name){}
	///\brief Symbolic reference constructor
	VariableValue( const std::string& name_, int scope_functionidx_)
		:m_scope_functionidx(scope_functionidx_),m_column_idx(-1),m_name(name_){}
	///\brief Column Index reference constructor
	VariableValue( int column_idx_, int scope_functionidx_)
		:m_scope_functionidx(scope_functionidx_),m_column_idx(column_idx_){}

	///\brief Evaluate if the reference is by name
	///\return true if yes
	bool isSymbolic() const				{return m_column_idx < 0;}
	///\brief Evaluate if the reference is by column index
	///\return true if yes
	bool isNumeric() const				{return m_column_idx > 0;}
	///\brief Evaluate if the value is constant
	///\return true if yes
	bool isConstant() const				{return m_column_idx == 0;}

	///\brief Get the scope of the referenced result
	int scope_functionidx() const			{return m_scope_functionidx;}
	///\brief Get the column index of the referenced result (if isNumeric)
	int column_idx() const				{return m_column_idx;}
	///\brief Get the name of the referenced result (if isSymbolic)
	const std::string& name() const			{return m_name;}
	///\brief Get the value of a constant (if isConstant)
	const std::string& value() const		{return m_name;}

private:
	friend class ConstantValue;			//< constant value type
	int m_scope_functionidx;			//< index of the last function defined +1 at the moment of variable definition. Used to defined the scope of this reference (to identify the referenced result.
	int m_column_idx;				//< index of the result column referenced or -1, if reference is symbolic or 0, if reference is a constant (m_name)
	std::string m_name;				//< symbolic reference name (m_column_idx < 0) or constant value (m_column_idx == 0)
};

///\class ConstantValue
///\brief Constructor for a variable value representing a constant
class ConstantValue :public VariableValue
{
public:
	///\brief Constructor
	explicit ConstantValue( const std::string& value_)
		:VariableValue( value_, -1)		{m_scope_functionidx = 0;}
};

typedef types::keymap<VariableValue>	VariableTable;


///\class TransactionFunctionDescription
///\brief Description of a transaction function
class TransactionFunctionDescription
{
public:
	class ProcessingStep
	{
	public:
		class Argument
		{
		public:
			enum Type {Constant,Selector};

			///\brief Default constructor
			Argument(){}
			///\brief Copy constructor
			Argument( const Argument& o)
				:type(o.type),value(o.value){}
			///\brief Constructor
			Argument( Type t, const std::string& v)
				:type(t),value(v){}

			Type type;				//< type of the argument
			std::string value;			//< value of the argument
		};

		///\brief Default constructor
		ProcessingStep(){}
		///\brief Copy constructor
		ProcessingStep( const ProcessingStep& o)
			:selector_FOREACH(o.selector_FOREACH)
			,functionname(o.functionname)
			,args(o.args)
			,resultnodes(o.resultnodes){}

		std::vector<std::string> selector_FOREACH;	//< selector of the nodes to process (FOREACH argument)
		std::vector<std::string> functionname;		//< name of function to call
		std::vector<Argument> args;			//< function call arguments
		std::vector<std::string> resultnodes;		//< names of nodes in the processed input node
	};

	///\class OperationStep
	///\brief Description of one step (database operation) of a transaction function
	class OperationStep
	{
	public:
		///\brief Default constructor
		OperationStep()
			:nonempty(false)
			,unique(false){}

		///\brief Copy constructor
		OperationStep( const OperationStep& o)
			:selector_FOREACH(o.selector_FOREACH)
			,call(o.call)
			,path_INTO(o.path_INTO)
			,nonempty(o.nonempty)
			,unique(o.unique)
			,hints(o.hints){}

		///\brief Clear content (default constructor)
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
			///\brief Constructor
			Error( std::size_t elemidx_, const std::string& msg_)
				:elemidx(elemidx_)
				,msg(msg_){}

			std::size_t elemidx;
			std::string msg;
		};

		///\class Call
		///\brief Database command call
		struct Call
		{
			///\class Param
			///\brief Database command call parameter
			struct Param
			{
				///\class Type
				///\brief Parameter type
				enum Type
				{
					VariableReference,		//< Parameter is a variable reference
					NumericResultReference,		//< Parameter is a result reference by index
					SymbolicResultReference,	//< Parameter is a result reference by column name
					Constant,			//< Parameter is a contant value
					InputSelectorPath		//< Parameter refers to a set of input values
				};
				///\brief Parameter type name
				static const char* typeName( Type i)
				{
					static const char* ar[] = {"VariableReference","NumericResultReference","SymbolicResultReference","Constant","InputSelectorPath"};
					return ar[(std::size_t)i];
				}
				Type type;		//< type of the parameter
				std::string value;	//< parsed value of the parameter

				///\brief Default constructor
				Param(){}
				///\brief Copy constructor
				Param( const Param& o)
					:type(o.type),value(o.value){}
				///\brief Constructor
				Param( Type type_, const std::string& value_)
					:type(type_),value(value_){}
			};

			std::string funcname;			//< function name
			std::vector<Param> paramlist;		//< list of arguments

			///\brief Default constructor
			Call(){}
			///\brief Copy constructor
			Call( const Call& o)
				:funcname(o.funcname),paramlist(o.paramlist){}
			///\brief Constructor
			Call( const std::string& funcname_, const std::vector<Param>& paramlist_)
				:funcname(funcname_),paramlist(paramlist_){}

			///\brief Reset call
			void clear()		{funcname.clear(); paramlist.clear();}
		};

		std::string selector_FOREACH;		//< parsed argument of foreach
		Call call;				//< called database function
		std::vector<std::string> path_INTO;	//< parsed argument of INTO (splitted by '/')
		bool nonempty;				//< true, if NONEMPTY is set
		bool unique;				//< true, if UNIQUE is set
		types::keymap<std::string> hints;	//< error messages to add to database errors depending on the error class
	};
	///\class Block
	///\brief Result block declaration
	struct Block
	{
		std::vector<std::string> path_INTO;	//< parsed argument of INTO (splitted by '/')
		std::size_t startidx;			//< start of block declaration
		std::size_t size;			//< number of command calls in block declaration

		///\brief Constructor
		Block( const std::vector<std::string>& p, std::size_t i, std::size_t n)
			:path_INTO(p),startidx(i),size(n){}
		///\brief Copy constructor
		Block( const Block& o)
			:path_INTO(o.path_INTO),startidx(o.startidx),size(o.size){}
		///\brief Default constructor
		Block()
			:startidx(0),size(0){}
	};
	std::vector<ProcessingStep> preprocs;	//< preprocessing steps on input
	std::vector<OperationStep> steps;	//< list of database commands or operations
	std::vector<Block> blocks;		//< substructures of the output
	langbind::Authorization auth;		//< authorization definition structure for this function
	VariableTable variablemap;		//< variable definitions with LET a = ...;
	bool casesensitive;			//< true, is the database is case sensitive

	///\brief Copy constructor
	TransactionFunctionDescription( const TransactionFunctionDescription& o)
		:steps(o.steps),blocks(o.blocks),auth(o.auth),variablemap(o.variablemap),casesensitive(o.casesensitive){}
	///\brief Default constructor
	TransactionFunctionDescription()
		:casesensitive(false){}
};

///\class TransactionFunctionR
///\brief Reference to transaction function
typedef types::CountedReference<TransactionFunction> TransactionFunctionR;

///\class TransactionFunction
///\brief Transaction function definition
class TransactionFunction
{
public:
	///\brief Copy constructor
	TransactionFunction( const TransactionFunction& o);
	///\brief Constructor
	TransactionFunction( const std::string& name_, const TransactionFunctionDescription& description, const types::keymap<TransactionFunctionR>& functionmap);
	///\brief Destructor
	virtual ~TransactionFunction();

	///\brief Build the function input
	virtual TransactionFunctionInput* getInput() const;
	///\brief Build the function output
	virtual TransactionFunctionOutput* getOutput( const db::TransactionOutput& o) const;

	///\brief Get the name of the function
	const std::string& name() const			{return m_name;}
	///\brief Set the name of the function
	void name( const std::string& name_)		{m_name = name_;}

	///\brief Implementation structure
	struct Impl;
	const Impl& impl() const
	{
		return *m_impl;
	}

	///\brief Get the authorization structure
	const langbind::Authorization& authorization() const
	{
		return m_authorization;
	}

	///\brief Get a hint for a given error class
	const char* getErrorHint( const std::string& errorclass, int functionidx) const;

private:
	std::string m_name;				//< transaction function name
	langbind::Authorization m_authorization;	//< transaction function authorization structure
	Impl* m_impl;					//< PIMPL reference
};

///\brief Creates a database transaction function from its description source
///\param[in] name name of  the transaction
///\param[in] description description of the function
///\param[in] functionmap map op operations in the module context that can be referenced
TransactionFunction* createTransactionFunction( const std::string& name, const TransactionFunctionDescription& description, const types::keymap<TransactionFunctionR>& functionmap);

}}//namespace
#endif

