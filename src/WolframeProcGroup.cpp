/************************************************************************

 Copyright (C) 2011 Project Wolframe.
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
// Wolframe processor group
//

#include "WolframeProcGroup.hpp"

#include "config/valueParser.hpp"
#include "config/configurationParser.hpp"
#include "logger.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "miscUtils.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <ostream>
#include <string>

/****  Impersonating the module loader  ******************************************************/
#include "echoProcessor.hpp"

using namespace _Wolframe;

static const size_t noProcModules = 1;
static module::ModuleConfigurationDescription< WolframeProcConfig >
procConfigs[ noProcModules ] = { module::ModuleConfigurationDescription< WolframeProcConfig >( "echoProcessor", "Echo Processor", "echoProcessor",
				 EchoProcConfig::create,
				 &config::ConfigurationParser::parseBase< EchoProcConfig > ) };

static module::ModuleContainerDescription< WolframeProcContainer, WolframeProcConfig >
procModules[ noProcModules ] = { module::ModuleContainerDescription< WolframeProcContainer, WolframeProcConfig >( "EchoProcessor", &EchoProcContainer::create ) };
/****  End impersonating the module loader  **************************************************/

namespace _Wolframe {
namespace config {

template<>
bool ConfigurationParser::parse( proc::ProcessorGroupConfig& cfg,
				 const boost::property_tree::ptree& pt, const std::string& /*node*/ )
{
	using namespace _Wolframe::config;
	bool retVal = true;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( "database", L1it->first ))	{
			bool isDefined = ( ! cfg.m_dbLabel.empty());
			if ( ! Parser::getValue( cfg.logPrefix().c_str(), *L1it, cfg.m_dbLabel, &isDefined ))
				retVal = false;
		}
		else	{
			size_t i;
			for ( i = 0; i < noProcModules; i++ )	{
				if ( boost::algorithm::iequals( procConfigs[i].typeName, L1it->first ))	{
					WolframeProcConfig* conf = procConfigs[i].createFunc( procConfigs[i].sectionTitle,
											      cfg.logPrefix().c_str(),
											      procConfigs[i].sectionName );
					if ( procConfigs[i].parseFunc( *conf, L1it->second, L1it->first ))
						cfg.m_procConfig.push_back( conf );
					else	{
						delete conf;
						retVal = false;
					}
					break;
				}
			}
			if ( i >= noProcModules )
				LOG_WARNING << cfg.logPrefix() << "unknown configuration option: '"
					    << L1it->first << "'";
		}
	}
	return retVal;
}

} // namespace config


//***  Processor Group Configuration  ************************************
namespace proc {

ProcessorGroupConfig::~ProcessorGroupConfig()
{
	for ( std::list<WolframeProcConfig*>::const_iterator it = m_procConfig.begin();
								it != m_procConfig.end(); it++ )
		delete *it;
}

void ProcessorGroupConfig::print( std::ostream& os, size_t /* indent */ ) const
{
	os << sectionName() << std::endl;
	os << "   Database: " << (m_dbLabel.empty() ? "(none)" : m_dbLabel) << std::endl;
	if ( m_procConfig.size() > 0 )	{
		for ( std::list<WolframeProcConfig*>::const_iterator it = m_procConfig.begin();
								it != m_procConfig.end(); it++ )	{
			(*it)->print( os, 3 );
		}
	}
	else
		os << "   None configured" << std::endl;
}


/// Check if the database configuration makes sense
bool ProcessorGroupConfig::check() const
{
	bool correct = true;
//	if ( m_dbLabel.empty() )	{
//		LOG_ERROR << logPrefix() << "referenced database ID cannot be empty";
//		correct = false;
//	}
	for ( std::list<WolframeProcConfig*>::const_iterator it = m_procConfig.begin();
								it != m_procConfig.end(); it++ )	{
		if ( !(*it)->check() )
			correct = false;
	}
	return correct;
}

void ProcessorGroupConfig::setCanonicalPathes( const std::string& refPath )
{
	for ( std::list<WolframeProcConfig*>::const_iterator it = m_procConfig.begin();
								it != m_procConfig.end(); it++ )	{
		(*it)->setCanonicalPathes( refPath );
	}
}

//**** Processor Group **************************************************
ProcessorGroup::ProcessorGroup( const ProcessorGroupConfig& conf )
{
	m_db = NULL;
	if ( !conf.m_dbLabel.empty())
		m_dbLabel = conf.m_dbLabel;
	for ( std::list<WolframeProcConfig*>::const_iterator it = conf.m_procConfig.begin();
								it != conf.m_procConfig.end(); it++ )	{
		const char* procType = (*it)->typeName();
		size_t i;
		for ( i = 0; i < noProcModules; i++ )	{
			if ( boost::algorithm::iequals( procModules[i].name, procType ))	{
				WolframeProcContainer* container = procModules[i].createFunc( **it );
				m_proc.push_back( container );
				break;
			}
		}
		if ( i >= noProcModules )	{
			LOG_ALERT << "Wolframe Processor Group: unknown processor type '" << procType << "'";
			throw std::domain_error( "Unknown processor type in Processor Group constructor. See log" );
		}
	}
}

ProcessorGroup::~ProcessorGroup()
{
	for ( std::list< WolframeProcContainer* >::const_iterator it = m_proc.begin();
							it != m_proc.end(); it++ )
		delete *it;
}

bool ProcessorGroup::resolveDB( db::DatabaseProvider& db )
{
	if ( m_db == NULL && ! m_dbLabel.empty() )	{
		m_db = db.database( m_dbLabel );
		if ( m_db )	{
			LOG_NOTICE << "Processor database: database reference '" << m_dbLabel << "' resolved";
			return true;
		}
		else	{
			LOG_ERROR << "Processor database: database labeled '" << m_dbLabel << "' not found !";
			return false;
		}
	}
	return true;
}

} // namespace proc
} // namespace _Wolframe
