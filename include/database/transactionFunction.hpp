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


struct OperationStepDescription
{
	OperationStepDescription()
		:nonempty(false)
		,unique(false){}

	OperationStepDescription( const OperationStepDescription& o)
		:selector_FOREACH(o.selector_FOREACH)
		,call(o.call)
		,path_INTO(o.path_INTO)
		,nonempty(o.nonempty)
		,unique(o.unique)
		,hints(o.hints){}

	void clear()
	{
		selector_FOREACH.clear();
		call.first.clear();
		call.second.clear();
		path_INTO.clear();
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

	std::string selector_FOREACH;
	typedef std::vector<std::string> ParamList;
	std::pair<std::string,ParamList> call;
	std::vector<std::string> path_INTO;
	bool nonempty;
	bool unique;
	types::keymap<std::string> hints;
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
	TransactionFunction( const std::string& name_, const std::vector<OperationStepDescription>& description, const std::string& resultname, const types::keymap<TransactionFunctionR>& functionmap, const langbind::Authorization& authorization_);
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
///\param[in] description description of the operation steps (Result of TDL parser)
///\param[in] resultname name of the result (RESULT INTO definition)
///\param[in] functionmap map op operations in the module context that can be referenced
///\param[in] auth authorization definition structure for this function
TransactionFunction* createTransactionFunction( const std::string& name, const std::vector<OperationStepDescription>& description, const std::string& resultname, const types::keymap<TransactionFunctionR>& functionmap, const langbind::Authorization& auth);

}}//namespace
#endif

