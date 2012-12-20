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
// Wolframe processor provider implementation
//

#include "processor/procProvider.hpp"
#include "procProviderImpl.hpp"
#include "types/doctype.hpp"
#include "config/valueParser.hpp"
#include "config/ConfigurationTree.hpp"
#include "logger-v1.hpp"
#include "moduleDirectory.hpp"
#include "utils/miscUtils.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <ostream>
#include <string>

namespace _Wolframe {
namespace proc {

//**** Processor Provider PIMPL Implementation ******************************
ProcessorProvider::ProcessorProvider_Impl::ProcessorProvider_Impl( const ProcProviderConfig* conf,
								   const module::ModulesDirectory* modules,
								   prgbind::ProgramLibrary* programs_)
	:m_programs(programs_)
{
	m_db = NULL;
	if ( !conf->m_dbLabel.empty())
		m_dbLabel = conf->m_dbLabel;
	m_programfiles = conf->programFiles();

	// Build the list of command handlers
	for ( std::list< config::NamedConfiguration* >::const_iterator it = conf->m_procConfig.begin();
									it != conf->m_procConfig.end(); it++ )	{
		module::ConfiguredBuilder* builder = modules->getBuilder((*it)->className());
		if ( builder )
		{
			if (builder->objectType() == ObjectConstructorBase::CMD_HANDLER_OBJECT)
			{
				cmdbind::CommandHandlerConstructor* cnstrctr =
					dynamic_cast<cmdbind::CommandHandlerConstructor*>( builder->constructor());

				if ( cnstrctr == NULL )	{
					LOG_ALERT << "Wolframe Processor Provider: '" << builder->objectClassName()
						  << "' is not a command handler";
					throw std::logic_error( "Object is not a commandHandler. See log." );
				}
				else	{
					m_cmd.push_back( cnstrctr );

					// register handler commands
					std::list<std::string> cmds = cnstrctr->commands( **it);
					for (std::list<std::string>::const_iterator cmdIt = cmds.begin(); cmdIt != cmds.end(); cmdIt++)
					{
						std::string opName = boost::algorithm::to_upper_copy( *cmdIt );
						CmdMap::const_iterator ci = m_cmdMap.find( opName);
						if (ci != m_cmdMap.end())
						{
							const char* c1 = ci->second.second->className();
							const char* c2 = (*it)->className();
							LOG_ERROR << "Duplicate definition of command '" << opName << "' (in '" << c1 << "' and in '" << c2 << "')";
							throw std::runtime_error( "duplicate command definition");
						}
						else
						{
							m_cmdMap[ opName ] = std::pair<cmdbind::CommandHandlerConstructor*, config::NamedConfiguration*>( cnstrctr, *it);
						}
						LOG_TRACE << "Command '" << opName << "' registered for '" << cnstrctr->objectClassName() << "' command handler";
					}
				}
			}
			else	{
				LOG_ALERT << "Wolframe Processor Provider: unknown processor type '" << (*it)->className() << "'";
				throw std::domain_error( "Unknown command handler type constructor. See log." );
			}
		}
		else	{
			LOG_ALERT << "Wolframe Processor Provider: processor provider configuration can not handle objects of this type '" << (*it)->className() << "'";
			throw std::domain_error( "Unknown configurable object for processor provider. See log." );
		}
	}

	// Build the lists of objects without configuration
	for ( module::ModulesDirectory::simpleBuilder_iterator it = modules->objectsBegin();
								it != modules->objectsEnd(); it++ )	{
		switch( it->objectType() )	{
			case ObjectConstructorBase::FILTER_OBJECT:	{	// object is a filter
				module::FilterConstructor* fltr = dynamic_cast< module::FilterConstructor* >((*it)->constructor());
				if ( fltr == NULL )	{
					LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
						  << "'' is not a filter";
					throw std::logic_error( "Object is not a filter. See log." );
				}
				else	{
					std::string name = boost::algorithm::to_upper_copy( fltr->name());
					std::string category = boost::algorithm::to_upper_copy( fltr->category());

					std::map < std::string, const module::FilterConstructor* >::const_iterator fltrItr = m_filterMap.find( name);
					if ( fltrItr != m_filterMap.end())
					{
						LOG_FATAL << "Duplicate filter name '" << name << "'";
						throw std::runtime_error( "Duplicate filter name" );
					}
					m_filter.push_back( fltr );
					m_filterMap[ name ] = fltr;
					if (!category.empty())
					{
						fltrItr = m_filterMap.find( category);
						if (fltrItr == m_filterMap.end())
						{
							m_filterMap[ category ] = fltr;
							LOG_TRACE << "'" << fltr->name() << "' as default '" << category << "'filter registered";
						}
					}
					LOG_TRACE << "'" << fltr->name() << "' (" << fltr->objectClassName() << ") filter registered";
				}
				break;
			}

			case ObjectConstructorBase::DDL_COMPILER_OBJECT:
			{	// object is a DDL compiler
				module::DDLCompilerConstructor* ffo = dynamic_cast< module::DDLCompilerConstructor* >((*it)->constructor());
				if ( ffo == NULL )	{
					LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
						  << "'' is not a DDL compiler";
					throw std::logic_error( "Object is not a form function. See log." );
				}
				else	{
					ddl::DDLCompilerR constructor( ffo->object());
					m_programs->defineFormDDL( constructor);
					m_formlibrary.addConstructor( constructor);
					delete ffo;
					LOG_TRACE << "'" << constructor->ddlname() << "' DDL compiler registered";
				}
				break;
			}

			case ObjectConstructorBase::FORM_FUNCTION_OBJECT:
			{	// object is a form function
				module::BuiltInFunctionConstructor* ffo = dynamic_cast< module::BuiltInFunctionConstructor* >((*it)->constructor());
				if ( ffo == NULL )	{
					LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
						  << "'' is not a form function";
					throw std::logic_error( "Object is not a form function. See log." );
				}
				else	{
					std::string name = ffo->objectClassName();
					boost::algorithm::to_upper( name);
					std::map< std::string, const module::BuiltInFunctionConstructor* >::const_iterator itr = m_formfunctionMap.find( name );
					if ( itr != m_formfunctionMap.end() )	{
						LOG_FATAL << "Duplicate form function name '" << name << "'";
						throw std::runtime_error( "Duplicate form function name" );
					}
					m_formfunction.push_back( ffo );
					m_formfunctionMap[ name ] = ffo;
					langbind::BuiltInFunction* func = ffo->object();
					m_programs->defineBuiltInFunction( name, *func);
					delete func;
					LOG_TRACE << "'" << name << "' form function registered";
				}
				break;
			}

			case ObjectConstructorBase::NORMALIZE_FUNCTION_OBJECT:
			{	// object is a normalize function constructor
				langbind::NormalizeFunctionConstructorR constructor( dynamic_cast< module::NormalizeFunctionConstructor* >((*it)->constructor()));
				if ( !constructor.get() )
				{
					LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
						  << "'' is not a normalize function constructor";
					throw std::logic_error( "Object is not a normalize function constructor. See log." );
				}
				else
				{
					m_normalizeFunctionConstructorMap.insert( std::string(constructor->domain()), constructor);
					m_programs->defineNormalizeFunctionConstructor( constructor);
					LOG_TRACE << "'" << constructor->objectClassName() << "' normalize function constructor for domain " << constructor->domain() << " registered";
				}
				break;
			}

			case ObjectConstructorBase::PRINT_FUNCTION_OBJECT:
			{	// object is a print function compiler
				langbind::PrintFunctionConstructorR constructor( dynamic_cast< module::PrintFunctionConstructor* >((*it)->constructor()));
				if (!constructor.get())
				{
					LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
						  << "'' is not a print layout description compiler";
					throw std::logic_error( "Object is not a print layout description compiler. See log." );
				}
				else
				{
					m_printprogram.addConstructor( constructor);
					m_programs->definePrintLayoutType( constructor);
					LOG_TRACE << "'" << constructor->programFileType() << "' print layout description compiler registered";
				}
				break;
			}

			case ObjectConstructorBase::AUDIT_OBJECT:
				LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
					  << "'' is marked as an AUDIT_OBJECT but has a simple object constructor";
				throw std::logic_error( "Object is not a valid simple object. See log." );
				break;
			case ObjectConstructorBase::AUTHENTICATION_OBJECT:
				LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
					  << "'' is marked as an AUTHENTICATION_OBJECT but has a simple object constructor";
				throw std::logic_error( "Object is not a valid simple object. See log." );
				break;
			case ObjectConstructorBase::AUTHORIZATION_OBJECT:
				LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
					  << "'' is marked as an AUTHORIZATION_OBJECT but has a simple object constructor";
				throw std::logic_error( "Object is not a valid simple object. See log." );
				break;
			case ObjectConstructorBase::DATABASE_OBJECT:
				LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
					  << "'' is marked as a DATABASE_OBJECT but has a simple object constructor";
				throw std::logic_error( "Object is not a valid simple object. See log." );
				break;
			case ObjectConstructorBase::CMD_HANDLER_OBJECT:
				LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
					  << "'' is marked as a CMD_HANDLER_OBJECT but has a simple object constructor";
				throw std::logic_error( "Object is not a valid simple object. See log." );
				break;
			case ObjectConstructorBase::LANGUAGE_EXTENSION_OBJECT:
				// ... language extension modules are not handled here
				break;
			case ObjectConstructorBase::TEST_OBJECT:
				LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
					  << "'' is marked as a TEST_OBJECT but has a simple object constructor";
				throw std::logic_error( "Object is not a valid simple object. See log." );
				break;
			default:
				LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
					  << "'' is of an unknown object type";
				throw std::logic_error( "Object is not a valid simple object. See log." );
		}
	}
}


ProcessorProvider::ProcessorProvider_Impl::~ProcessorProvider_Impl()
{
	for ( std::list< cmdbind::CommandHandlerConstructor* >::iterator it = m_cmd.begin();
							it != m_cmd.end(); it++ )
		delete *it;

	for ( std::list< module::FilterConstructor* >::iterator it = m_filter.begin();
							it != m_filter.end(); it++ )
		delete *it;

	for ( std::list< module::BuiltInFunctionConstructor* >::iterator it = m_formfunction.begin();
							it != m_formfunction.end(); it++ )
		delete *it;
}

class ProcessorProvider::ProcessorProvider_Impl::DDLTypeMap :public ddl::TypeMap
{
public:
	explicit DDLTypeMap( const ProcessorProvider::ProcessorProvider_Impl* pp)
		:m_provider( pp){}

	virtual const ddl::NormalizeFunction* getType( const std::string& name) const
	{
		return m_provider->normalizeFunction( name);
	}

private:
	const ProcessorProvider::ProcessorProvider_Impl* m_provider;
};

bool ProcessorProvider::ProcessorProvider_Impl::loadPrograms()
{
	bool rt = true;
	try
	{
		m_programs->loadPrograms( transactionDatabase( true), m_programfiles);
	}
	catch (const std::runtime_error& e)
	{
		LOG_ERROR << "failed to load programs: " << e.what();
		return false;
	}

	std::list< std::string >::const_iterator pi = m_programfiles.begin(), pe = m_programfiles.end();
	for (; pi != pe; ++pi)
	{
		if (m_normprogram.is_mine( *pi)) m_normprogram.loadfile( *pi, m_normalizeFunctionConstructorMap);
	}
	m_formtypemap.reset( new DDLTypeMap( this));
	m_formlibrary.setTypeMap( m_formtypemap);

	for(pi = m_programfiles.begin(); pi != pe; ++pi)
	{
		if (m_formlibrary.is_mine( *pi)) m_formlibrary.loadProgram( *pi);
	}
	for(pi = m_programfiles.begin(); pi != pe; ++pi)
	{
		if (m_printprogram.is_mine( *pi)) m_printprogram.loadProgram( *pi);
	}
	return rt;
}


bool ProcessorProvider::ProcessorProvider_Impl::resolveDB( const db::DatabaseProvider& db )
{
	bool rt = true;
	if ( m_db == NULL && ! m_dbLabel.empty() )	{
		m_db = db.database( m_dbLabel );
		if ( m_db )	{
			LOG_DEBUG << "Processor database: database reference '" << m_dbLabel << "' resolved";
		}
		else	{
			LOG_ALERT << "Processor database: database labeled '" << m_dbLabel << "' not found !";
			return false;
		}
		types::keymap<std::string> embeddedStatementMap;

		// load all locally defined programs
		m_db->loadAllPrograms();
		// load database programs:
		m_dbprogram.defineEmbeddedLanguageDescription( m_db->getLanguageDescription());
		std::list< std::string >::const_iterator pi = m_programfiles.begin(), pe = m_programfiles.end();
		for (; pi != pe; ++pi)
		{
			if (m_dbprogram.is_mine( *pi))
			{
				try
				{
					std::string dbsrc;
					m_dbprogram.loadfile( *pi, dbsrc, embeddedStatementMap);
					m_db->addProgram( dbsrc);
				}
				catch (const std::runtime_error& err)
				{
					LOG_ERROR << "failed to load transaction program '" << *pi << "': " << err.what();
					rt = false;
				}
			}
		}
		m_db->addStatements( embeddedStatementMap);
	}
	return rt;
}

langbind::Filter* ProcessorProvider::ProcessorProvider_Impl::filter( const std::string& name, const std::string& arg ) const
{
	std::string filterName = boost::algorithm::to_upper_copy( name);
	std::map< std::string, const module::FilterConstructor* >::const_iterator fltr = m_filterMap.find( filterName );
	if ( fltr == m_filterMap.end() )
		return NULL;
	else
		return fltr->second->object( arg);
}

langbind::BuiltInFunction* ProcessorProvider::ProcessorProvider_Impl::formfunction( const std::string& name ) const
{
	std::string formfunctionName = boost::algorithm::to_upper_copy( name);
	std::map< std::string, const module::BuiltInFunctionConstructor* >::const_iterator ffo = m_formfunctionMap.find( formfunctionName );
	if ( ffo == m_formfunctionMap.end() )
		return NULL;
	else
		return ffo->second->object();
}

const ddl::Form* ProcessorProvider::ProcessorProvider_Impl::form( const std::string& name ) const
{
	return m_formlibrary.get( name);
}

const prnt::PrintFunction* ProcessorProvider::ProcessorProvider_Impl::printFunction( const std::string& name ) const
{
	return m_printprogram.get( name);
}

const langbind::NormalizeFunction* ProcessorProvider::ProcessorProvider_Impl::normalizeFunction( const std::string& name ) const
{
	return m_programs->getNormalizeFunction( name);
}

cmdbind::CommandHandler* ProcessorProvider::ProcessorProvider_Impl::cmdhandler( const std::string& command) const
{
	std::string cmdName = boost::algorithm::to_upper_copy( command );
	std::map< std::string, std::pair<cmdbind::CommandHandlerConstructor*, config::NamedConfiguration*> >::const_iterator cmd = m_cmdMap.find( cmdName );
	if ( cmd == m_cmdMap.end() )
	{
		return NULL;
	}
	cmdbind::CommandHandlerConstructor* constructor = cmd->second.first;
	config::NamedConfiguration* cfg = cmd->second.second;
	return constructor->object( *cfg);
}

cmdbind::IOFilterCommandHandler* ProcessorProvider::ProcessorProvider_Impl::iofilterhandler( const std::string& command ) const
{
	cmdbind::CommandHandler* hnd = cmdhandler( command);
	if (!hnd) return NULL;
	return dynamic_cast<cmdbind::IOFilterCommandHandler*>( hnd);
}


const UI::UserInterfaceLibrary* ProcessorProvider::ProcessorProvider_Impl::UIlibrary() const
{
	if ( m_db )
		return m_db->UIlibrary();
	else	{
		LOG_ALERT << "No database defined for the processor provider";
		return NULL;
	}
}

db::Database* ProcessorProvider::ProcessorProvider_Impl::transactionDatabase( bool suppressAlert) const
{
	if ( ! m_db && !suppressAlert )	{
		LOG_ALERT << "No database defined for the processor provider";
	}
	return m_db;
}

db::Transaction* ProcessorProvider::ProcessorProvider_Impl::transaction( const std::string& name ) const
{
	if ( m_db )
		return m_db->transaction( name );
	else	{
		LOG_ALERT << "No database defined for the processor provider";
		return NULL;
	}
}

const db::TransactionFunction* ProcessorProvider::ProcessorProvider_Impl::transactionFunction( const std::string& name ) const
{
	return m_dbprogram.function( name );
}

std::string ProcessorProvider::ProcessorProvider_Impl::xmlDoctypeString( const std::string& formname, const std::string& ddlname, const std::string& xmlroot) const
{
	std::ostringstream rt;
	rt << xmlroot << " SYSTEM \"" << formname << "." << ddlname << "\"";
	return rt.str();
}

}} // namespace _Wolframe::proc
