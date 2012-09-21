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
//
// Wolframe processor provider (group)
//

#ifndef _WOLFRAME_PROCESSOR_PROVIDER_HPP_INCLUDED
#define _WOLFRAME_PROCESSOR_PROVIDER_HPP_INCLUDED

#include "processor/procProvider.hpp"
#include "database/database.hpp"
#include "database/DBprovider.hpp"
#include "module/filterBuilder.hpp"
#include "module/ddlcompilerBuilder.hpp"
#include "module/formfunctionBuilder.hpp"
#include "module/ddlcompilerBuilder.hpp"
#include "module/printFunctionBuilder.hpp"
#include "module/transactionFunctionBuilder.hpp"
#include "cmdbind/commandHandlerUnit.hpp"

#include <list>
#include <map>

namespace _Wolframe {
namespace proc {

class ProcessorProvider::ProcessorProvider_Impl
{
public:
	ProcessorProvider_Impl( const ProcProviderConfig* conf,
				const ProcessorProvider* this_,
				const module::ModulesDirectory* modules);
	~ProcessorProvider_Impl();

	bool resolveDB( const db::DatabaseProvider& db );

	cmdbind::CommandHandler* cmdhandler( const std::string& command );
	cmdbind::IOFilterCommandHandler* iofilterhandler( const std::string& command );

	langbind::Filter* filter( const std::string& name, const std::string& arg ) const;
	langbind::FormFunction* formfunction( const std::string& name ) const;
	const ddl::StructType* form( const std::string& name ) const;
	const prnt::PrintFunction* printFunction( const std::string& name) const;
	const langbind::TransactionFunction* transactionFunction( const std::string& name) const;

	const db::Database* transactionDatabase() const;

private:
	bool loadForm( const std::string& ddlname, const std::string& dataDefinitionFilename);
	bool loadPrintFunction( const std::string& name, const std::string& type, const std::string& layoutFilename);
	bool declareTransactionFunction( const std::string& name, const std::string& type, const std::string& command);
	bool declareFunctionName( const std::string& name, const char* typestr);

private:
	std::string					m_dbLabel;
	const db::Database*				m_db;
	std::list< cmdbind::CommandHandlerConstructor* >	m_cmd;
	std::map< std::string, std::pair<cmdbind::CommandHandlerConstructor*, config::NamedConfiguration*> >	m_cmdMap;

	std::list< module::FilterConstructor* >	m_filter;
	std::map< std::string, const module::FilterConstructor* >	m_filterMap;

	std::list< module::FormFunctionConstructor* >	m_formfunction;
	std::map< std::string, const module::FormFunctionConstructor* >	m_formfunctionMap;

	std::list< module::DDLCompilerConstructor* >	m_ddlcompiler;
	std::map< std::string, ddl::DDLCompilerR >	m_ddlcompilerMap;
	std::map< std::string, ddl::StructTypeR>	m_formMap;

	std::list< module::TransactionFunctionConstructor* >	m_transactionFunctionCompiler;
	std::map< std::string, const module::TransactionFunctionConstructor* >	m_transactionFunctionCompilerMap;
	std::map< std::string, langbind::TransactionFunctionR>	m_transactionFunctionMap;

	std::list< module::PrintFunctionConstructor* >	m_printFunctionCompiler;
	std::map< std::string, const module::PrintFunctionConstructor* >	m_printFunctionCompilerMap;
	std::map< std::string, prnt::PrintFunctionR>	m_printFunctionMap;

	std::map< std::string, const char*>	m_langfunctionIdentifierMap;
};

}} // namespace _Wolframe::proc

#endif // _WOLFRAME_PROCESSOR_PROVIDER_HPP_INCLUDED
