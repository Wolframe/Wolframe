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
//
// Processor Provider
//

#ifndef _PROCESSOR_PROVIDER_HPP_INCLUDED
#define _PROCESSOR_PROVIDER_HPP_INCLUDED

#include <boost/noncopyable.hpp>
#include "database/DBprovider.hpp"
#include "cmdbind/commandHandler.hpp"
#include "cmdbind/ioFilterCommandHandler.hpp"
#include "cmdbind/lineCommandHandler.hpp"
#include "cmdbind/authCommandHandler.hpp"
#include "prgbind/programLibrary.hpp"
#include "types/customDataType.hpp"

namespace _Wolframe {
namespace proc {

/// Base class for processor configuration
class ProcProviderConfig : public config::ConfigurationBase
{
	friend class ProcessorProvider;
public:
	/// constructor & destructor
	ProcProviderConfig()
		: ConfigurationBase( "Processor(s)", NULL, "Processor configuration" )	{}
	~ProcProviderConfig();

	/// methods
	bool parse( const config::ConfigurationTree& pt, const std::string& node,
		    const module::ModulesDirectory* modules );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );
	const std::list<std::string>& programFiles() const
	{
		return m_programFiles;
	}

private:
	std::string					m_dbLabel;
	std::list< config::NamedConfiguration* >	m_procConfig;
	std::list< std::string >			m_programFiles;
};


/// Processor provider
class ProcessorProvider : private boost::noncopyable
{
public:
	ProcessorProvider( const ProcProviderConfig* conf,
			   const module::ModulesDirectory* modules,
			   prgbind::ProgramLibrary* programs_);
	~ProcessorProvider();

	bool resolveDB( const db::DatabaseProvider& db );
	bool loadPrograms();

	cmdbind::CommandHandler* cmdhandler( const std::string& command) const;
	cmdbind::IOFilterCommandHandler* iofilterhandler( const std::string& command) const;

	std::string xmlDoctypeString( const std::string& formname, const std::string& ddlname, const std::string& xmlroot) const;

	db::Database* transactionDatabase() const;

	///\brief Just and interface at the moment
	const UI::UserInterfaceLibrary* UIlibrary() const;

	///\brief return a database transaction object for the given name
	db::Transaction* transaction( const std::string& name ) const;

	///\brief Get the list of UI-forms
	///\return map name -> uiform xml without header
	std::map<std::string,std::string> uiforms( const std::string& /*auth_ticket*/, int /*min_version*/, int& /*version*/) const
	{
		/// make it just compile
		return std::map<std::string,std::string>();
	}

	const types::NormalizeFunction* normalizeFunction( const std::string& name) const;
	const types::NormalizeFunctionType* normalizeFunctionType( const std::string& name) const;
	const langbind::FormFunction* formFunction( const std::string& name) const;
	const types::FormDescription* formDescription( const std::string& name) const;
	langbind::Filter* filter( const std::string& name, const std::vector<langbind::FilterArgument>& arg=std::vector<langbind::FilterArgument>()) const;
	const types::CustomDataType* customDataType( const std::string& name) const;

private:
	class ProcessorProvider_Impl;
	ProcessorProvider_Impl *m_impl;
};

}} // namespace _Wolframe::proc

#endif // _PROCESSOR_PROVIDER_HPP_INCLUDED
