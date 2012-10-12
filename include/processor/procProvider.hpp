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
// Processor Provider
//

#ifndef _PROCESSOR_PROVIDER_HPP_INCLUDED
#define _PROCESSOR_PROVIDER_HPP_INCLUDED

#include <boost/noncopyable.hpp>
#include "database/DBprovider.hpp"
#include "database/transactionFunction.hpp"
#include "filter/filter.hpp"
#include "langbind/appFormFunction.hpp"
#include "cmdbind/commandHandler.hpp"
#include "cmdbind/ioFilterCommandHandler.hpp"
#include "ddl/compilerInterface.hpp"
#include "prnt/printFunction.hpp"
#include "langbind/normalizeFunction.hpp"
#include "langbind/appConfig_struct.hpp"

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

private:
	std::string					m_dbLabel;
	std::list< config::NamedConfiguration* >	m_procConfig;
	langbind::EnvironmentConfigStruct		m_environment;
	std::list< std::string >			m_programFiles;
};

/// Processor provider
class ProcessorProvider : private boost::noncopyable
{
public:
	ProcessorProvider( const ProcProviderConfig* conf,
			   const module::ModulesDirectory* modules );
	~ProcessorProvider();

	bool resolveDB( const db::DatabaseProvider& db );

	cmdbind::CommandHandler* cmdhandler( const std::string& command );
	cmdbind::IOFilterCommandHandler* iofilterhandler( const std::string& command);

	langbind::Filter* filter( const std::string& name, const std::string& arg ) const;
	langbind::FormFunction* formfunction( const std::string& name) const;

	const ddl::Form* form( const std::string& name ) const;
	const prnt::PrintFunction* printFunction( const std::string& name) const;
	const langbind::NormalizeFunction* normalizeFunction( const std::string& name) const;

	db::Database* transactionDatabase() const;

	/// return a database transaction object for the given name
	db::Transaction* transaction( const std::string& name ) const;

	const db::TransactionFunction* transactionFunction( const std::string& name) const;

private:
	class ProcessorProvider_Impl;
	ProcessorProvider_Impl *m_impl;
};

}} // namespace _Wolframe::proc

#endif // _PROCESSOR_PROVIDER_HPP_INCLUDED
