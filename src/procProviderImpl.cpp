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
#include "moduleDirectory.hpp"

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
		if ( boost::algorithm::iequals( "environment", L1it->first ))
		{
			try
			{
				m_environment.initFromPropertyTree( L1it->second);
			}
			catch (const std::exception& e)
			{
				LOG_ERROR << e.what();
				retVal = false;
			}
		}
		else	{
			if ( modules )	{
				module::ConfiguredBuilder* builder;
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


//***  Processor Group Configuration  ************************************
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

	m_environment.print( os, indent);
}


/// Check if the configuration makes sense
bool ProcProviderConfig::check() const
{
	bool correct = true;
#if PF_DISABLED
//...Configuration without database can be possible in wolfilter
	if ( m_dbLabel.empty() )	{
		LOG_ERROR << logPrefix() << "referenced database ID can not be empty";
		correct = false;
	}
#endif
	for ( std::list< config::NamedConfiguration* >::const_iterator it = m_procConfig.begin();
								it != m_procConfig.end(); it++ )	{
		if ( !(*it)->check() )
			correct = false;
	}
	if (!m_environment.check())
	{
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
	m_environment.setCanonicalPathes( refPath );
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

const ddl::StructType* ProcessorProvider::form( const std::string& name) const
{
	return m_impl->form( name);
}

const prnt::PrintFunction* ProcessorProvider::printFunction( const std::string& name) const
{
	return m_impl->printFunction( name);
}

const langbind::TransactionFunction* ProcessorProvider::transactionFunction( const std::string& name) const
{
	return m_impl->transactionFunction( name);
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
						m_cmdMap[ opName ] = std::pair<cmdbind::CommandHandlerConstructor*, config::NamedConfiguration*>( cnstrctr, *it);

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
					std::string name = fltr->objectClassName();
					boost::algorithm::to_upper( name );
					std::map <const std::string, const module::FilterConstructor* >::const_iterator fltrItr = m_filterMap.find( name );
					if ( fltrItr != m_filterMap.end() )	{
						LOG_FATAL << "Duplicate filter name '" << name << "'";
						throw std::runtime_error( "Duplicate filter name" );
					}
					m_filter.push_back( fltr );
					m_filterMap[ name ] = fltr;
					const char* cc = strchr( name.c_str(), ':');
					if (cc)
					{
						//filter names with ':' separated segments
						//in the name have the first segment as name
						//(default filter for a category like 'xml')
						//for the first loaded filter:
						std::string category( name.c_str(), cc - name.c_str());
						fltrItr = m_filterMap.find( category);
						if (fltrItr == m_filterMap.end())
						{
							m_filterMap[ category ] = fltr;
							LOG_TRACE << "'" << name << "' as default '" << category << "'filter registered";
						}
					}
					LOG_TRACE << "'" << name << "' filter registered";
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
					std::string name = ffo->objectClassName();
					boost::algorithm::to_upper( name);
					std::map <const std::string, ddl::DDLCompilerR >::const_iterator itr = m_ddlcompilerMap.find( name );
					if ( itr != m_ddlcompilerMap.end() )	{
						LOG_FATAL << "Duplicate DDL compiler name '" << name << "'";
						throw std::runtime_error( "Duplicate DDL compiler name" );
					}
					m_ddlcompiler.push_back( ffo );
					const ddl::DDLCompilerR cc( ffo->object());
					m_ddlcompilerMap[ name ] = cc;

					LOG_TRACE << "'" << name << "' DDL compiler registered";
				}
				break;
			}

			case ObjectConstructorBase::FORM_FUNCTION_OBJECT:
			{	// object is a form function
				module::FormFunctionConstructor* ffo = dynamic_cast< module::FormFunctionConstructor* >((*it)->constructor());
				if ( ffo == NULL )	{
					LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
						  << "'' is not a form function";
					throw std::logic_error( "Object is not a form function. See log." );
				}
				else	{
					std::string name = ffo->objectClassName();
					boost::algorithm::to_upper( name);
					std::map <const std::string, const module::FormFunctionConstructor* >::const_iterator itr = m_formfunctionMap.find( name );
					if ( itr != m_formfunctionMap.end() )	{
						LOG_FATAL << "Duplicate form function name '" << name << "'";
						throw std::runtime_error( "Duplicate form function name" );
					}
					if (!declareFunctionName( name, "form function"))
					{
						throw std::runtime_error( "Duplicate function identifier used for form function");
					}
					m_formfunction.push_back( ffo );
					m_formfunctionMap[ name ] = ffo;

					LOG_TRACE << "'" << name << "' form function registered";
				}
				break;
			}

			case ObjectConstructorBase::TRANSACTION_FUNCTION_OBJECT:
			{	// object is a transaction function compiler
				module::TransactionFunctionConstructor* ffo = dynamic_cast< module::TransactionFunctionConstructor* >((*it)->constructor());
				if ( ffo == NULL )
				{
					LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
						  << "'' is not a transaction function compiler";
					throw std::logic_error( "Object is not a transaction function compiler. See log." );
				}
				else	{
					std::string name = ffo->objectClassName();
					boost::algorithm::to_upper( name);
					std::map <const std::string, const module::TransactionFunctionConstructor* >::const_iterator itr = m_transactionFunctionCompilerMap.find( name );
					if ( itr != m_transactionFunctionCompilerMap.end() )	{
						LOG_FATAL << "Duplicate transaction function compiler name '" << name << "'";
						throw std::runtime_error( "Duplicate transaction function compiler name" );
					}
					m_transactionFunctionCompiler.push_back( ffo );
					m_transactionFunctionCompilerMap[ name ] = ffo;

					LOG_TRACE << "'" << name << "' transaction function compiler registered";
				}
				break;
			}

			case ObjectConstructorBase::PRINT_FUNCTION_OBJECT:
			{	// object is a print function compiler
				module::PrintFunctionConstructor* ffo = dynamic_cast< module::PrintFunctionConstructor* >((*it)->constructor());
				if ( ffo == NULL )
				{
					LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
						  << "'' is not a print function compiler";
					throw std::logic_error( "Object is not a print function compiler. See log." );
				}
				else
				{
					std::string name = ffo->objectClassName();
					std::string key = boost::algorithm::to_upper_copy( name);
					std::map <const std::string, const module::PrintFunctionConstructor* >::const_iterator itr = m_printFunctionCompilerMap.find( key );
					if ( itr != m_printFunctionCompilerMap.end() )	{
						LOG_FATAL << "Duplicate print function compiler name '" << name << "'";
						throw std::runtime_error( "Duplicate print function compiler name" );
					}
					m_printFunctionCompiler.push_back( ffo );
					m_printFunctionCompilerMap[ key ] = ffo;

					LOG_TRACE << "'" << name << "' print function compiler registered";
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

	// Build the list of configured objects in the processor environment:
	bool success = true;
	for (std::vector<langbind::DDLFormConfigStruct>::const_iterator ii=conf->m_environment.form.begin(), ee=conf->m_environment.form.end(); ii != ee; ++ii)
	{
		success &= loadForm( ii->DDL, ii->file);
	}
	for (std::vector<langbind::PrintLayoutConfigStruct>::const_iterator ii=conf->m_environment.printlayout.begin(), ee=conf->m_environment.printlayout.end(); ii != ee; ++ii)
	{
		success &= loadPrintFunction( ii->name, ii->type, ii->file);
	}
	for (std::vector<langbind::TransactionFunctionConfigStruct>::const_iterator ii=conf->m_environment.transaction.begin(), ee=conf->m_environment.transaction.end(); ii != ee; ++ii)
	{
		success &= declareTransactionFunction( ii->name, ii->type, ii->call);
	}
	if (!success)
	{
		throw std::logic_error( "Not all configured objects in the processor environment could be loaded. See log." );
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

	for ( std::list< module::FormFunctionConstructor* >::iterator it = m_formfunction.begin();
							it != m_formfunction.end(); it++ )
		delete *it;

	for ( std::list< module::DDLCompilerConstructor* >::iterator it = m_ddlcompiler.begin();
							it != m_ddlcompiler.end(); ++it )
		delete *it;

	for ( std::list< module::TransactionFunctionConstructor* >::iterator it = m_transactionFunctionCompiler.begin();
							it != m_transactionFunctionCompiler.end(); ++it )
		delete *it;

	for ( std::list< module::PrintFunctionConstructor* >::iterator it = m_printFunctionCompiler.begin();
							it != m_printFunctionCompiler.end(); ++it )
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


langbind::Filter* ProcessorProvider::ProcessorProvider_Impl::filter( const std::string& name, const std::string& arg ) const
{
	std::string filterName = boost::algorithm::to_upper_copy( name);
	std::map <const std::string, const module::FilterConstructor* >::const_iterator fltr = m_filterMap.find( filterName );
	if ( fltr == m_filterMap.end() )
		return NULL;
	else
		return fltr->second->object( arg);
}

langbind::FormFunction* ProcessorProvider::ProcessorProvider_Impl::formfunction( const std::string& name ) const
{
	std::string formfunctionName = boost::algorithm::to_upper_copy( name);
	std::map <const std::string, const module::FormFunctionConstructor* >::const_iterator ffo = m_formfunctionMap.find( formfunctionName );
	if ( ffo == m_formfunctionMap.end() )
		return NULL;
	else
		return ffo->second->object();
}

bool ProcessorProvider::ProcessorProvider_Impl::declareFunctionName( const std::string& name, const char* typestr)
{
	std::string key = boost::algorithm::to_upper_copy( name);
	std::map <const std::string, const char*>::const_iterator idt = m_langfunctionIdentifierMap.find( key);
	if (idt != m_langfunctionIdentifierMap.end())
	{
		LOG_ERROR << "Duplicate function identifier for "<< typestr << " with name '" << name << "' already used for a " << idt->second << "";
		return false;
	}
	m_langfunctionIdentifierMap[ key] = typestr;
	return true;
}

bool ProcessorProvider::ProcessorProvider_Impl::loadForm( const std::string& ddlname, const std::string& dataDefinitionFilename)
{
	try
	{
		std::string key = boost::algorithm::to_upper_copy( ddlname);
		std::map <const std::string, ddl::DDLCompilerR>::const_iterator itr = m_ddlcompilerMap.find( key);
		if (itr == m_ddlcompilerMap.end())
		{
			LOG_ERROR << "Failed to load form '" << utils::getFileStem( dataDefinitionFilename) << "'. Compiler for DDL '" << ddlname << "' is not defined";
			return false;
		}
		std::pair< std::string, ddl::StructTypeR> def = ddl::loadForm( *itr->second, dataDefinitionFilename);
		std::string formkey = boost::algorithm::to_upper_copy( def.first);
		m_formMap[ formkey] = def.second;

		LOG_TRACE << "Form '" << def.first << "' in '" << utils::getFileStem( dataDefinitionFilename) << "' for DDL '" << ddlname << "' loaded";
		return true;
	}
	catch (std::exception& e)
	{
		LOG_ERROR << e.what();
		return false;
	}
}

const ddl::StructType* ProcessorProvider::ProcessorProvider_Impl::form( const std::string& name ) const
{
	std::string key = boost::algorithm::to_upper_copy( name);
	std::map <std::string, ddl::StructTypeR>::const_iterator itr = m_formMap.find( key);
	if ( itr == m_formMap.end() )
		return NULL;
	else
		return itr->second.get();
}

bool ProcessorProvider::ProcessorProvider_Impl::loadPrintFunction( const std::string& name, const std::string& type, const std::string& layoutFilename)
{
	try
	{
		std::string typekey = boost::algorithm::to_upper_copy( type);
		std::map <std::string, const module::PrintFunctionConstructor*>::const_iterator itr = m_printFunctionCompilerMap.find( typekey);
		if (itr == m_printFunctionCompilerMap.end())
		{
			LOG_ERROR << "Failed to load print layout '" << utils::getFileStem( layoutFilename) << "'. Printer type '" << type << "' is not defined";
			return false;
		}
		prnt::PrintFunctionR funcp( itr->second->object( utils::readSourceFileContent( layoutFilename)));
		std::string funcname( name);
		if (funcname.empty())
		{
			funcname = utils::getFileStem( layoutFilename);
		}
		std::string funckey( boost::algorithm::to_upper_copy( funcname));

		std::map <std::string, prnt::PrintFunctionR>::const_iterator ip = m_printFunctionMap.find( funckey);
		if (ip != m_printFunctionMap.end())
		{
			LOG_ERROR << "Duplicate definition of print layout with name '" << funcname << "'";
			return false;
		}
		if (!declareFunctionName( funcname, "print function name"))
		{
			return false;
		}
		m_printFunctionMap[ funckey] = funcp;
		LOG_TRACE << "Print layout '" << funcname << "' for printer '" << name << "' loaded";
		return true;
	}
	catch (std::exception& e)
	{
		LOG_ERROR << "Cannot load print layout from file '" << utils::getFileStem( layoutFilename) << "': " <<  e.what();
		return false;
	}
}

const prnt::PrintFunction* ProcessorProvider::ProcessorProvider_Impl::printFunction( const std::string& name ) const
{
	std::string key = boost::algorithm::to_upper_copy( name);
	std::map <std::string, prnt::PrintFunctionR>::const_iterator itr = m_printFunctionMap.find( key);
	if ( itr == m_printFunctionMap.end() )
		return NULL;
	else
		return itr->second.get();
}

bool ProcessorProvider::ProcessorProvider_Impl::declareTransactionFunction( const std::string& name, const std::string& type, const std::string& command)
{
	try
	{
		std::string key = boost::algorithm::to_upper_copy( type);
		std::map <std::string, const module::TransactionFunctionConstructor*>::const_iterator itr = m_transactionFunctionCompilerMap.find( key);
		if (itr == m_transactionFunctionCompilerMap.end())
		{
			LOG_ERROR << "Cannot declare transaction function '" << name << "'. Transaction function type '" << type << "' is not defined";
			return false;
		}
		langbind::TransactionFunctionR funcp( itr->second->object( command));
		std::string funckey( boost::algorithm::to_upper_copy( name));

		std::map <std::string, langbind::TransactionFunctionR>::const_iterator ip = m_transactionFunctionMap.find( funckey);
		if (ip != m_transactionFunctionMap.end())
		{
			LOG_ERROR << "Duplicate definition of transaction function with name '" << name << "'";
			return false;
		}
		if (!declareFunctionName( name, "transaction function name"))
		{
			return false;
		}
		m_transactionFunctionMap[ funckey] = funcp;
		LOG_TRACE << "Transaction function '" << name << "' (" << type << ") declared";
		return true;
	}
	catch (std::exception& e)
	{
		LOG_ERROR << "Cannot declare '" << type << "' transaction function '" << name << "': " <<  e.what();
		return false;
	}
}

const langbind::TransactionFunction* ProcessorProvider::ProcessorProvider_Impl::transactionFunction( const std::string& name ) const
{
	std::string key = boost::algorithm::to_upper_copy( name);
	std::map <std::string, langbind::TransactionFunctionR>::const_iterator itr = m_transactionFunctionMap.find( key);
	if ( itr == m_transactionFunctionMap.end() )
		return NULL;
	else
		return itr->second.get();
}

cmdbind::CommandHandler* ProcessorProvider::ProcessorProvider_Impl::cmdhandler( const std::string& command )
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

cmdbind::IOFilterCommandHandler* ProcessorProvider::ProcessorProvider_Impl::iofilterhandler( const std::string& command )
{
	cmdbind::CommandHandler* hnd = cmdhandler( command);
	if (!hnd) return NULL;
	return dynamic_cast<cmdbind::IOFilterCommandHandler*>( hnd);
}

}} // namespace _Wolframe::proc
