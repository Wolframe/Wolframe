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
//
// Wolframe processor provider
//

#ifndef _WOLFRAME_PROCESSOR_PROVIDER_HPP_INCLUDED
#define _WOLFRAME_PROCESSOR_PROVIDER_HPP_INCLUDED

#include "processor/procProvider.hpp"
#include "database/database.hpp"
#include "database/DBprovider.hpp"
#include "cmdbind/commandHandlerConstructor.hpp"
#include "prgbind/programLibrary.hpp"
#include <list>
#include <map>

namespace _Wolframe {
namespace proc {

class ProcessorProvider::ProcessorProvider_Impl
{
public:
	ProcessorProvider_Impl( const ProcProviderConfig* conf,
				const module::ModulesDirectory* modules,
				prgbind::ProgramLibrary* programs_);
	~ProcessorProvider_Impl();

	bool resolveDB( const db::DatabaseProvider& db );

	cmdbind::CommandHandler* cmdhandler( const std::string& command) const;
	cmdbind::IOFilterCommandHandler* iofilterhandler( const std::string& command ) const;

	std::string xmlDoctypeString( const std::string& formname, const std::string& ddlname, const std::string& xmlroot) const;

	const UI::UserInterfaceLibrary* UIlibrary() const;

	db::Database* transactionDatabase( bool suppressAlert=false) const;
	db::Transaction* transaction( const std::string& name ) const;

	const types::NormalizeFunction* typeNormalizer( const std::string& name) const;
	const langbind::FormFunction* formFunction( const std::string& name) const;
	const types::FormDescription* formDescription( const std::string& name) const;
	langbind::Filter* filter( const std::string& name, const std::vector<langbind::FilterArgument>& arg) const;
	const types::CustomDataType* customDataType( const std::string& name) const;

	bool loadPrograms();
	bool checkReferences( const ProcessorProvider* provider) const;

private:
	std::string					m_dbLabel;
	db::Database*					m_db;

	class CommandHandlerDef
	{
	public:
		CommandHandlerDef()
			:configuration(0){}
		CommandHandlerDef( const CommandHandlerDef& o)
			:constructor(o.constructor),configuration(o.configuration){}
		CommandHandlerDef( cmdbind::CommandHandlerConstructor* constructor_, const config::NamedConfiguration* configuration_)
			:constructor(constructor_),configuration(configuration_){}
		~CommandHandlerDef(){}
	public:
		cmdbind::CommandHandlerConstructorR constructor;
		const config::NamedConfiguration* configuration;
	};
	std::vector<CommandHandlerDef> m_cmd;
	typedef std::map<std::string,std::size_t> CmdMap;
	CmdMap m_cmdMap;

	std::list<std::string> m_programfiles;
	prgbind::ProgramLibrary* m_programs;
};

}} // namespace _Wolframe::proc

#endif // _WOLFRAME_PROCESSOR_PROVIDER_HPP_INCLUDED
