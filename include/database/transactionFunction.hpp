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
#include "processor/procProvider.hpp"
#include <string>
#include <vector>
#include <map>

namespace _Wolframe {
namespace db {

///\brief Forward declaration
class TransactionFunctionDescription;

///\class TransactionFunction
///\brief Forward Declaration
class TransactionFunction;

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
	void finalize( const proc::ProcessorProvider* provider);
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
	virtual langbind::TypedInputFilterR getOutput( const proc::ProcessorProvider* provider, const db::TransactionOutputR& o) const;

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

	std::string tostring() const;

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

