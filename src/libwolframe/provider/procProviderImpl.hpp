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
// Wolframe processor provider
//

#ifndef _WOLFRAME_PROCESSOR_PROVIDER_HPP_INCLUDED
#define _WOLFRAME_PROCESSOR_PROVIDER_HPP_INCLUDED

#include "processor/procProvider.hpp"
#include "database/database.hpp"
#include "database/DBprovider.hpp"
#include "module/filterBuilder.hpp"
#include "module/ddlcompilerBuilder.hpp"
#include "module/builtInFunctionBuilder.hpp"
#include "module/ddlcompilerBuilder.hpp"
#include "module/printFunctionBuilder.hpp"
#include "module/normalizeFunctionBuilder.hpp"
#include "cmdbind/commandHandlerUnit.hpp"
#include "langbind/normalizeProgram.hpp"
#include "database/transactionProgram.hpp"
#include "langbind/formFunction.hpp"
#include "langbind/formLibrary.hpp"
#include "langbind/printProgram.hpp"
#include <list>
#include <map>

namespace _Wolframe {
namespace proc {

class ProcessorProvider::ProcessorProvider_Impl
{
public:
	ProcessorProvider_Impl( const ProcProviderConfig* conf,
				const module::ModulesDirectory* modules,
				const std::vector<prgbind::ProgramR>& programTypes_);
	~ProcessorProvider_Impl();

	bool resolveDB( const db::DatabaseProvider& db );

	cmdbind::CommandHandler* cmdhandler( const std::string& command) const;
	cmdbind::IOFilterCommandHandler* iofilterhandler( const std::string& command ) const;

	langbind::Filter* filter( const std::string& name, const std::string& arg ) const;
	langbind::BuiltInFunction* formfunction( const std::string& name ) const;
	const ddl::Form* form( const std::string& name ) const;
	const prnt::PrintFunction* printFunction( const std::string& name) const;
	const langbind::NormalizeFunction* normalizeFunction( const std::string& name) const;
	std::string xmlDoctypeString( const std::string& formname, const std::string& ddlname, const std::string& xmlroot) const;

	const UI::UserInterfaceLibrary* UIlibrary() const;

	db::Database* transactionDatabase() const;
	db::Transaction* transaction( const std::string& name ) const;
	const db::TransactionFunction* transactionFunction( const std::string& name ) const;

	void defineFunction( const std::string& name, langbind::FormFunctionR func)
	{
		m_formFunctionLibrary.insert( name, func);
	}

private:
	class DDLTypeMap;
	bool loadPrograms();

private:
	std::string					m_dbLabel;
	db::Database*					m_db;

	std::list< cmdbind::CommandHandlerConstructor* >	m_cmd;
	typedef std::map< std::string, std::pair<cmdbind::CommandHandlerConstructor*, config::NamedConfiguration*> > CmdMap;
	CmdMap	m_cmdMap;

	std::list< module::FilterConstructor* >	m_filter;
	std::map< std::string, const module::FilterConstructor* >	m_filterMap;

	std::list< module::BuiltInFunctionConstructor* >	m_formfunction;
	std::map< std::string, const module::BuiltInFunctionConstructor* >	m_formfunctionMap;

	std::list< std::string >	m_programfiles;
	db::TransactionProgram	m_dbprogram;
	langbind::NormalizeProgram	m_normprogram;
	ddl::TypeMapR	m_formtypemap;
	langbind::FormLibrary	m_formlibrary;
	langbind::PrintProgram	m_printprogram;

	types::keymap<langbind::FormFunctionR> m_formFunctionLibrary;
	std::vector<prgbind::ProgramR> m_programTypes;
};

}} // namespace _Wolframe::proc

#endif // _WOLFRAME_PROCESSOR_PROVIDER_HPP_INCLUDED
