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

#include "processor/procProvider.hpp"
#include "procProviderImpl.hpp"

#include "config/valueParser.hpp"
#include "config/ConfigurationTree.hpp"
#include "logger-v1.hpp"
#include "moduleDirectory.hpp"
#include "utils/miscUtils.hpp"

#define BOOST_FILESYSTEM_VERSION 3
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
		else if ( boost::algorithm::iequals( "environment", L1it->first ))
		{
			for ( boost::property_tree::ptree::const_iterator eni = L1it->second.begin(); eni != L1it->second.end(); eni++ )
			{
				if ( boost::algorithm::iequals( "form", eni->first ))
				{
					langbind::DDLFormConfigStruct st;
					for ( boost::property_tree::ptree::const_iterator ai = eni->second.begin(); ai != eni->second.end(); ai++ )
					{
						if (boost::algorithm::iequals( "DDL", ai->first ))
						{
							st.DDL = ai->second.data();
						}
						else if (boost::algorithm::iequals( "file", ai->first ))
						{
							st.file = ai->second.data();
						}
						else
						{
							LOG_ERROR << "invalid form configuration element '" << ai->first << "'";
						}
					}
					m_environment.form.push_back( st);
				}
				else if ( boost::algorithm::iequals( "printlayout", eni->first ))
				{
					langbind::PrintLayoutConfigStruct st;
					for ( boost::property_tree::ptree::const_iterator ai = eni->second.begin(); ai != eni->second.end(); ai++ )
					{
						if (boost::algorithm::iequals( "name", ai->first ))
						{
							st.name = ai->second.data();
						}
						else if (boost::algorithm::iequals( "type", ai->first ))
						{
							st.type = ai->second.data();
						}
						else if (boost::algorithm::iequals( "file", ai->first ))
						{
							st.file = ai->second.data();
						}
						else
						{
							LOG_ERROR << "invalid printlayout configuration element '" << ai->first << "'";
						}
					}
					m_environment.printlayout.push_back( st);
				}
				else if ( boost::algorithm::iequals( "normalize", eni->first ))
				{
					langbind::NormalizeFunctionConfigStruct st;
					for ( boost::property_tree::ptree::const_iterator ai = eni->second.begin(); ai != eni->second.end(); ai++ )
					{
						if (boost::algorithm::iequals( "name", ai->first ))
						{
							st.name = ai->second.data();
						}
						else if (boost::algorithm::iequals( "type", ai->first ))
						{
							st.type = ai->second.data();
						}
						else if (boost::algorithm::iequals( "call", ai->first ))
						{
							st.call = ai->second.data();
						}
						else
						{
							LOG_ERROR << "invalid normalize configuration element '" << ai->first << "'";
						}
					}
					m_environment.normalize.push_back( st);
				}
				else if ( boost::algorithm::iequals( "transaction", eni->first ))
				{
					langbind::TransactionFunctionConfigStruct st;
					for ( boost::property_tree::ptree::const_iterator ai = eni->second.begin(); ai != eni->second.end(); ai++ )
					{
						if (boost::algorithm::iequals( "name", ai->first ))
						{
							st.name = ai->second.data();
						}
						else if (boost::algorithm::iequals( "type", ai->first ))
						{
							st.type = ai->second.data();
						}
						else if (boost::algorithm::iequals( "call", ai->first ))
						{
							st.call = ai->second.data();
						}
						else
						{
							LOG_ERROR << "invalid transaction configuration element '" << ai->first << "'";
						}
					}
					m_environment.transaction.push_back( st);
				}
			}
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
						break;
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

	std::string indstr( '\t', indent+1);
	os << "Environment:" << std::endl;
	for (std::vector<langbind::DDLFormConfigStruct>::const_iterator ii=m_environment.form.begin(), ee=m_environment.form.end(); ii != ee; ++ii)
	{
		os << indstr << "DDL " << ii->DDL << " " << ii->file;
	}
	for (std::vector<langbind::PrintLayoutConfigStruct>::const_iterator ii=m_environment.printlayout.begin(), ee=m_environment.printlayout.end(); ii != ee; ++ii)
	{
		os << indstr << "printlayout " << ii->name << " (" << ii->type << ") " << ii->file;
	}
	for (std::vector<langbind::TransactionFunctionConfigStruct>::const_iterator ii=m_environment.transaction.begin(), ee=m_environment.transaction.end(); ii != ee; ++ii)
	{
		os << indstr << "transaction " << ii->name << " (" << ii->type << ") " << ii->call;
	}
	for (std::vector<langbind::NormalizeFunctionConfigStruct>::const_iterator ii=m_environment.normalize.begin(), ee=m_environment.normalize.end(); ii != ee; ++ii)
	{
		os << indstr << "normalize " << ii->name << " (" << ii->type << ") " << ii->call;
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
	for (std::vector<langbind::DDLFormConfigStruct>::const_iterator ii=m_environment.form.begin(), ee=m_environment.form.end(); ii != ee; ++ii)
	{
		if (!utils::fileExists( ii->file)) return false;
	}
	for (std::vector<langbind::PrintLayoutConfigStruct>::const_iterator ii=m_environment.printlayout.begin(), ee=m_environment.printlayout.end(); ii != ee; ++ii)
	{
		if (!utils::fileExists( ii->file)) return false;
	}
	return correct;
}

void ProcProviderConfig::setCanonicalPathes( const std::string& refPath )
{
	for ( std::list< config::NamedConfiguration* >::const_iterator it = m_procConfig.begin();
								it != m_procConfig.end(); it++ )	{
		(*it)->setCanonicalPathes( refPath );
	}
	for (std::vector<langbind::DDLFormConfigStruct>::iterator ii=m_environment.form.begin(), ee=m_environment.form.end(); ii != ee; ++ii)
	{
		ii->file = utils::getCanonicalPath( ii->file, refPath);
	}
	for (std::vector<langbind::PrintLayoutConfigStruct>::iterator ii=m_environment.printlayout.begin(), ee=m_environment.printlayout.end(); ii != ee; ++ii)
	{
		ii->file = utils::getCanonicalPath( ii->file, refPath);
	}
}


//**** Processor Provider PIMPL *********************************************
ProcessorProvider::ProcessorProvider( const ProcProviderConfig* conf,
				      const module::ModulesDirectory* modules )
	: m_impl(0)
{
	m_impl = new ProcessorProvider_Impl( conf, modules );
}

ProcessorProvider::~ProcessorProvider()
{
	delete m_impl;
}

bool ProcessorProvider::resolveDB( const db::DatabaseProvider& db )
{
	return m_impl->resolveDB( db );
}

langbind::Filter* ProcessorProvider::filter( const std::string& name, const std::string& arg ) const
{
	return m_impl->filter( name, arg );
}

langbind::FormFunction* ProcessorProvider::formfunction( const std::string& name) const
{
	return m_impl->formfunction( name );
}

cmdbind::CommandHandler* ProcessorProvider::cmdhandler( const std::string& name )
{
	cmdbind::CommandHandler* rt = m_impl->cmdhandler( name );
	if (rt) rt->setProcProvider( this);
	return rt;
}

cmdbind::IOFilterCommandHandler* ProcessorProvider::iofilterhandler( const std::string& name )
{
	cmdbind::IOFilterCommandHandler* rt = m_impl->iofilterhandler( name );
	if (rt) rt->setProcProvider( this);
	return rt;
}

const ddl::Form* ProcessorProvider::form( const std::string& name) const
{
	return m_impl->form( name);
}

const prnt::PrintFunction* ProcessorProvider::printFunction( const std::string& name) const
{
	return m_impl->printFunction( name);
}

const langbind::NormalizeFunction* ProcessorProvider::normalizeFunction( const std::string& name) const
{
	return m_impl->normalizeFunction( name );
}

const langbind::TransactionFunction* ProcessorProvider::transactionFunction( const std::string& name) const
{
	return m_impl->transactionFunction( name);
}

db::Database* ProcessorProvider::transactionDatabase() const
{
	return m_impl->transactionDatabase();
}

db::Transaction* ProcessorProvider::transaction( const std::string& name ) const
{
	return m_impl->transaction( name );
}

}} // namespace _Wolframe::proc
