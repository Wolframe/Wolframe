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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file processor/procProviderInterface.hpp
///\brief Interface to processor provider for language bindings and database
#ifndef _PROCESSOR_PROVIDER_INTERFACE_HPP_INCLUDED
#define _PROCESSOR_PROVIDER_INTERFACE_HPP_INCLUDED
#include "filter/filter.hpp"
#include "langbind/formFunction.hpp"
#include <string>
#include <vector>
namespace _Wolframe {
namespace db
{
///\brief Forward declaration
class Transaction;
///\brief Forward declaration
class Database;
}
namespace types
{
///\brief Forward declaration
class Form;
///\brief Forward declaration
class FormDescription;
///\brief Forward declaration
class NormalizeFunction;
///\brief Forward declaration
class NormalizeFunctionType;
}
namespace cmdbind
{
///\brief Forward declaration
class CommandHandler;
}

namespace proc {

///\class ProcessorProviderInterface
///\brief Abstract class as processor provider interface
class ProcessorProviderInterface
{
public:
	///\brief Destructor
	virtual ~ProcessorProviderInterface(){};
	///\brief Create a command handler for a specific command and doc format
	///\param[in] command name of the command
	///\param[in] docformat document format, e.g. "XML","JSON"
	///\return the constructed command handler (owned now by the caller)
	virtual cmdbind::CommandHandler* cmdhandler( const std::string& command, const std::string& docformat) const=0;
	///\brief Find out if there exists a command handler for a specific command without creating it
	///\param[in] command name of the command
	///\return true, if yes
	virtual bool existcmd( const std::string& command) const=0;
	///\brief Get the database for transactions
	///\return reference to database
	virtual db::Database* transactionDatabase() const=0;
	///\brief Get a database transaction object for the given name
	///\param[in] name name of the transaction
	///\return allocated transaction object now owned by the caller and to destroy by the caller with delete
	virtual db::Transaction* transaction( const std::string& name) const=0;
	///\brief Get a normalization function
	///\param[in] name name of the function
	///\return reference to normalization function
	virtual const types::NormalizeFunction* normalizeFunction( const std::string& name) const=0;
	///\brief Get a normalization function type
	///\param[in] name name of the function type
	///\return reference to normalization function type
	virtual const types::NormalizeFunctionType* normalizeFunctionType( const std::string& name) const=0;
	///\brief Get a form function
	///\param[in] name name of the function
	///\return reference to the function
	virtual const langbind::FormFunction* formFunction( const std::string& name) const=0;
	///\brief Get a form description
	///\param[in] name name of the form
	///\return reference to the form description
	virtual const types::FormDescription* formDescription( const std::string& name) const=0;
	///\brief Get a filter type
	///\param[in] name name of the filter
	///\return constant filter reference
	virtual const langbind::FilterType* filterType( const std::string& name) const=0;
	///\brief Get a custom data type
	///\param[in] name name of the type
	///\return reference to the custom data type
	virtual const types::CustomDataType* customDataType( const std::string& name) const=0;
	///\brief Guess document format ('XML','JSON', etc. based on guesser modules loaded)
	///\param[in] content pointer to document 
	///\param[in] contentsize size of content in bytes
	///\param[out] result document format as string
	///\return true, if decision was possible, false, if more data is needed
	virtual bool guessDocumentFormat( std::string& result, const char* content, std::size_t contentsize) const=0;
	///\brief Get the application configuration reference path
	///\return the reference path
	virtual const std::string& referencePath() const=0;
};

}}//namespace
#endif

