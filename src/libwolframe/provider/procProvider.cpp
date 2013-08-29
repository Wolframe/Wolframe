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

#include "processor/procProvider.hpp"
#include "procProviderImpl.hpp"

#include "config/valueParser.hpp"
#include "config/ConfigurationTree.hpp"
#include "logger-v1.hpp"
#include "processor/moduleDirectory.hpp"
#include "utils/fileUtils.hpp"

#include <boost/filesystem.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <ostream>
#include <string>

namespace _Wolframe {
namespace proc {

//***  Processor Provider Configuration  ************************************
bool ProcProviderConfig::parse( const config::ConfigurationTree& pt, const std::string& /*node*/,
				const module::ModulesDirectory* modules )
{
	using namespace _Wolframe::config;
	bool retVal = true;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( "database", L1it->first ))	{
			bool isDefined = ( ! m_dbLabel.empty());
			if ( ! Parser::getValue( logPrefix().c_str(), *L1it, m_dbLabel, &isDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( "program", L1it->first ) )	{
			std::string programFile;
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, programFile ))
				retVal = false;
			else
				m_programFiles.push_back( programFile );
		}
		else	{
			if ( modules )	{
				module::ConfiguredBuilder* builder = 0;
				boost::property_tree::ptree::const_iterator kwi=L1it->second.begin(),kwe=L1it->second.end();
				for (; kwi != kwe; ++kwi)
				{
					const char* section = L1it->first.c_str();
					const char* keyword = kwi->first.c_str();
					builder = modules->getBuilder( section, keyword);
					if (builder)
					{
						config::NamedConfiguration* conf = builder->configuration( logPrefix().c_str());
						if (conf->parse( kwi->second, kwi->first, modules))
						{
							m_procConfig.push_back( conf);
						}
						else
						{
							delete conf;
							retVal = false;
						}
					}
				}
				if (!builder)
				{
					builder = modules->getBuilder( "processor", L1it->first );
					if ( builder )	{
						config::NamedConfiguration* conf = builder->configuration( logPrefix().c_str());
						if ( conf->parse( L1it->second, L1it->first, modules ))
							m_procConfig.push_back( conf );
						else	{
							delete conf;
							retVal = false;
						}
					}
					else
						LOG_WARNING << logPrefix() << "unknown configuration option: '"
							    << L1it->first << "'";
				}
			}
			else
				LOG_WARNING << logPrefix() << "unknown configuration option: '"
					    << L1it->first << "'";
		}
	}
	return retVal;
}

} // namespace config


namespace proc {

ProcProviderConfig::~ProcProviderConfig()
{
	for ( std::list< config::NamedConfiguration* >::const_iterator it = m_procConfig.begin();
								it != m_procConfig.end(); it++ )
		delete *it;
}

void ProcProviderConfig::print( std::ostream& os, size_t indent ) const
{
	os << sectionName() << std::endl;
	os << "   Database: " << (m_dbLabel.empty() ? "(none)" : m_dbLabel) << std::endl;
	if ( m_procConfig.size() > 0 )	{
		for ( std::list< config::NamedConfiguration* >::const_iterator it = m_procConfig.begin();
								it != m_procConfig.end(); it++ )	{
			(*it)->print( os, 3 );
		}
	}
	else
		os << "   None configured" << std::endl;

	std::string indStr( indent + 1, '\t');
	if ( m_programFiles.size() == 0 )
		os << "   Program file: none" << std::endl;
	else if ( m_programFiles.size() == 1 )
		os << "   Program file: " << m_programFiles.front() << std::endl;
	else	{
		std::list< std::string >::const_iterator it = m_programFiles.begin();
		os << "   Program files: " << *it++ << std::endl;
		while ( it != m_programFiles.end() )
			os << "                  " << *it++ << std::endl;
	}
}


/// Check if the configuration makes sense
bool ProcProviderConfig::check() const
{
	bool correct = true;

	for ( std::list< config::NamedConfiguration* >::const_iterator it = m_procConfig.begin();
								it != m_procConfig.end(); it++ )	{
		if ( !(*it)->check() )
			correct = false;
	}
	return correct;
}

void ProcProviderConfig::setCanonicalPathes( const std::string& refPath )
{
	for ( std::list< config::NamedConfiguration* >::const_iterator it = m_procConfig.begin();
								it != m_procConfig.end(); it++ )	{
		(*it)->setCanonicalPathes( refPath );
	}
	for ( std::list< std::string >::iterator it = m_programFiles.begin();
						it != m_programFiles.end(); it++ )	{
		std::string oldPath = *it;
		*it = utils::getCanonicalPath( *it, refPath );
		if ( oldPath != *it )	{
/*MBa ?!?*/		LOG_NOTICE << logPrefix() << "Using program absolute filename '" << *it
				   << "' instead of '" << oldPath << "'";
		}
	}
}


//**** Processor Provider PIMPL *********************************************
ProcessorProvider::ProcessorProvider( const ProcProviderConfig* conf,
				      const module::ModulesDirectory* modules,
				      prgbind::ProgramLibrary* programs_)
	: m_impl(0)
{
	m_impl = new ProcessorProvider_Impl( conf, modules, programs_);
}

ProcessorProvider::~ProcessorProvider()
{
	delete m_impl;
}

bool ProcessorProvider::resolveDB( const db::DatabaseProvider& db )
{
	return m_impl->resolveDB( db );
}

bool ProcessorProvider::loadPrograms()
{
	return m_impl->loadPrograms();
}

cmdbind::CommandHandler* ProcessorProvider::cmdhandler( const std::string& name ) const
{
	cmdbind::CommandHandler* rt = m_impl->cmdhandler( name );
	if (rt) rt->setProcProvider( this);
	return rt;
}

cmdbind::IOFilterCommandHandler* ProcessorProvider::iofilterhandler( const std::string& name ) const
{
	cmdbind::IOFilterCommandHandler* rt = m_impl->iofilterhandler( name );
	if (rt) rt->setProcProvider( this);
	return rt;
}

std::string ProcessorProvider::xmlDoctypeString( const std::string& formname, const std::string& ddlname, const std::string& xmlroot) const
{
	return m_impl->xmlDoctypeString( formname, ddlname, xmlroot);
}

const UI::UserInterfaceLibrary* ProcessorProvider::UIlibrary() const
{
	return m_impl->UIlibrary();
}

db::Database* ProcessorProvider::transactionDatabase() const
{
	return m_impl->transactionDatabase();
}

db::Transaction* ProcessorProvider::transaction( const std::string& name ) const
{
	return m_impl->transaction( name );
}

const types::NormalizeFunction* ProcessorProvider::normalizeFunction( const std::string& name) const
{
	return m_impl->normalizeFunction( name);
}

const langbind::FormFunction* ProcessorProvider::formFunction( const std::string& name) const
{
	return m_impl->formFunction( name);
}

const types::FormDescription* ProcessorProvider::formDescription( const std::string& name) const
{
	return m_impl->formDescription( name);
}

langbind::Filter* ProcessorProvider::filter( const std::string& name, const std::string& arg) const
{
	return m_impl->filter( name, arg);
}

}} // namespace _Wolframe::proc
