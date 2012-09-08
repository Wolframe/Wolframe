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
// Wolframe processor group
//

#include "processor/procProvider.hpp"
#include "procProviderImpl.hpp"

#include "config/valueParser.hpp"
#include "config/ConfigurationTree.hpp"
#include "logger-v1.hpp"
#include "moduleInterface.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "utils/miscUtils.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <ostream>
#include <string>

namespace _Wolframe {
namespace proc {

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
		else	{
			if ( modules )	{
				module::ConfiguredBuilder* builder = modules->getBuilder( "processor", L1it->first );
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
			else
				LOG_WARNING << logPrefix() << "unknown configuration option: '"
					    << L1it->first << "'";
		}
	}
	return retVal;
}

} // namespace config


//***  Processor Group Configuration  ************************************
namespace proc {

ProcProviderConfig::~ProcProviderConfig()
{
	for ( std::list< config::NamedConfiguration* >::const_iterator it = m_procConfig.begin();
								it != m_procConfig.end(); it++ )
		delete *it;
}

void ProcProviderConfig::print( std::ostream& os, size_t /* indent */ ) const
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
}


/// Check if the database configuration makes sense
bool ProcProviderConfig::check() const
{
	bool correct = true;
	if ( m_dbLabel.empty() )	{
		LOG_ERROR << logPrefix() << "referenced database ID cannot be empty";
		correct = false;
	}
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
}


//**** Processor Provider PIMPL *********************************************
ProcessorProvider::ProcessorProvider( const ProcProviderConfig* conf,
				      const module::ModulesDirectory* modules )
	: m_impl( new ProcessorProvider_Impl( conf, modules ))	{}

ProcessorProvider::~ProcessorProvider()
{
	delete m_impl;
}

bool ProcessorProvider::resolveDB( const db::DatabaseProvider& db )
{
	return m_impl->resolveDB( db );
}

const langbind::Filter* ProcessorProvider::filter( const std::string& name ) const
{
	return m_impl->filter( name );
}

cmdbind::CommandHandler* ProcessorProvider::handler( const std::string& name ) const
{
	return m_impl->handler( name );
}


//**** Processor Provider PIMPL Implementation ******************************
ProcessorProvider::ProcessorProvider_Impl::ProcessorProvider_Impl( const ProcProviderConfig* conf,
								   const module::ModulesDirectory* modules )
{
	m_db = NULL;
	if ( !conf->m_dbLabel.empty())
		m_dbLabel = conf->m_dbLabel;

	// Build the list of command handlers
	for ( std::list< config::NamedConfiguration* >::const_iterator it = conf->m_procConfig.begin();
									it != conf->m_procConfig.end(); it++ )	{
		module::ConfiguredBuilder* builder = modules->getBuilder((*it)->className());
		if ( builder )	{
			ConfiguredObjectConstructor< cmdbind::CommandHandlerUnit >* cnstrctr =
					dynamic_cast< ConfiguredObjectConstructor< cmdbind::CommandHandlerUnit >* >( builder->constructor());
			if ( cnstrctr == NULL )	{
				LOG_ALERT << "Wolframe Processor Provider: '" << builder->identifier()
					  << "'' is not a command handler";
				throw std::logic_error( "Object is not a commandHandler. See log." );
			}
			else	{
				cmdbind::CommandHandlerUnit* handlerUnit = cnstrctr->object( **it );
				m_handler.push_back( handlerUnit );
				std::string handlerName = cnstrctr->identifier();
				LOG_TRACE << "'" << handlerName << "' command handler registered";

				// register handler commands
				for ( std::list< std::string >::const_iterator cmdIt = handlerUnit->commands()->begin();
										cmdIt != handlerUnit->commands()->end(); cmdIt++ )	{
					std::string opName = boost::algorithm::to_upper_copy( *cmdIt );
					m_cmdMap[ opName ] = handlerUnit;
					LOG_TRACE << "'" << opName << "' registered for '" << handlerName << "' command handler";
				}
			}
		}
		else	{
			LOG_ALERT << "Wolframe Processor Provider: unknown processor type '" << (*it)->className() << "'";
			throw std::domain_error( "Unknown command handler type constructor. See log." );
		}
	}

	// Build the list of filters
	for ( module::ModulesDirectory::simpleBuilder_iterator it = modules->objectsBegin();
								it != modules->objectsEnd(); it++ )	{
		switch( it->constructor()->objectType() )	{
			case ObjectConstructorBase::FILTER_OBJECT:	{	// object is a filter
				module::FilterConstructor* fltr = dynamic_cast< module::FilterConstructor* >((*it)->constructor());
				if ( fltr == NULL )	{
					LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->identifier()
						  << "'' is not a filter";
					throw std::logic_error( "Object is not a filter. See log." );
				}
				else	{
					std::string name = fltr->identifier();
					boost::algorithm::to_upper( name );
					std::map <const std::string, const module::FilterConstructor* >::const_iterator fltrItr = m_filterMap.find( name );
					if ( fltrItr != m_filterMap.end() )	{
						LOG_FATAL << "Duplicate filter name '" << name << "'";
						throw std::runtime_error( "Duplicate filter name" );
					}
					m_filter.push_back( fltr );
					m_filterMap[ name ] = fltr;
					LOG_TRACE << "'" << name << "' filter registered";
				}
				break;
			}
			case ObjectConstructorBase::DDL_COMPILER_OBJECT:	// object is a DDL compiler
				LOG_WARNING << "DDL compiler interface is not yet available";
				break;

			case ObjectConstructorBase::FORM_FUNCTION_OBJECT:	{	// object is a DDL compiler
				module::FormFunctionContainer* ffo = dynamic_cast< module::FormFunctionContainer* >((*it)->constructor());
				if ( ffo == NULL )	{
					LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->identifier()
						  << "'' is not a form function";
					throw std::logic_error( "Object is not a form function. See log." );
				}
				else	{
					std::string name = ffo->identifier();
					boost::algorithm::to_upper( name );
					LOG_TRACE << "'" << name << "' form function registered";
				}
				break;
			}

			case ObjectConstructorBase::AUDIT_OBJECT:
				LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->identifier()
					  << "'' is marked as an AUDIT_OBJECT but has a simple object constructor";
				throw std::logic_error( "Object is not a valid simple object. See log." );
				break;
			case ObjectConstructorBase::AUTHENTICATION_OBJECT:
				LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->identifier()
					  << "'' is marked as an AUTHENTICATION_OBJECT but has a simple object constructor";
				throw std::logic_error( "Object is not a valid simple object. See log." );
				break;
			case ObjectConstructorBase::AUTHORIZATION_OBJECT:
				LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->identifier()
					  << "'' is marked as an AUTHORIZATION_OBJECT but has a simple object constructor";
				throw std::logic_error( "Object is not a valid simple object. See log." );
				break;
			case ObjectConstructorBase::DATABASE_OBJECT:
				LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->identifier()
					  << "'' is marked as an DATABASE_OBJECT but has a simple object constructor";
				throw std::logic_error( "Object is not a valid simple object. See log." );
				break;
			case ObjectConstructorBase::CMD_HANDLER_OBJECT:
				LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->identifier()
					  << "'' is marked as an CMD_HANDLER_OBJECT but has a simple object constructor";
				throw std::logic_error( "Object is not a valid simple object. See log." );
				break;
			case ObjectConstructorBase::TEST_OBJECT:
				LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->identifier()
					  << "'' is marked as an TEST_OBJECT but has a simple object constructor";
				throw std::logic_error( "Object is not a valid simple object. See log." );
				break;
			default:
				LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->identifier()
					  << "'' is of an unknown object type";
				throw std::logic_error( "Object is not a valid simple object. See log." );
		}
	}
}


ProcessorProvider::ProcessorProvider_Impl::~ProcessorProvider_Impl()
{
	for ( std::list< cmdbind::CommandHandlerUnit* >::iterator it = m_handler.begin();
							it != m_handler.end(); it++ )
		delete *it;
	for ( std::list< const module::FilterConstructor* >::iterator it = m_filter.begin();
							it != m_filter.end(); it++ )
		delete *it;
}

bool ProcessorProvider::ProcessorProvider_Impl::resolveDB( const db::DatabaseProvider& db )
{
	if ( m_db == NULL && ! m_dbLabel.empty() )	{
		m_db = db.database( m_dbLabel );
		if ( m_db )	{
			LOG_DEBUG << "Processor database: database reference '" << m_dbLabel << "' resolved";
			return true;
		}
		else	{
			LOG_ERROR << "Processor database: database labeled '" << m_dbLabel << "' not found !";
			return false;
		}
	}
	return true;
}


const langbind::Filter* ProcessorProvider::ProcessorProvider_Impl::filter( const std::string& name ) const
{
	std::string filterName = boost::algorithm::to_upper_copy( name );
	std::map <const std::string, const module::FilterConstructor* >::const_iterator fltr = m_filterMap.find( filterName );
	if ( fltr == m_filterMap.end() )
		return NULL;
	else
		return fltr->second->object();
}

cmdbind::CommandHandler* ProcessorProvider::ProcessorProvider_Impl::handler( const std::string& command ) const
{
	std::string cmdName = boost::algorithm::to_upper_copy( command );
	std::map< const std::string, cmdbind::CommandHandlerUnit* >::const_iterator cmd = m_cmdMap.find( cmdName );
	if ( cmd == m_cmdMap.end() )
		return NULL;
	else
		return cmd->second->handler( command );
}

}} // namespace _Wolframe::proc