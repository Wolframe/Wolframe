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
#include "utils/doctype.hpp"

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

//**** Processor Provider PIMPL Implementation ******************************
ProcessorProvider::ProcessorProvider_Impl::ProcessorProvider_Impl( const ProcProviderConfig* conf,
								   const module::ModulesDirectory* modules )
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
					std::string name = ffo->programFileType();
					boost::algorithm::to_upper( name);
					std::map< std::string, ddl::DDLCompilerR >::const_iterator itr = m_ddlcompilerMap.find( name );
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
					std::map< std::string, const module::FormFunctionConstructor* >::const_iterator itr = m_formfunctionMap.find( name );
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

			case ObjectConstructorBase::NORMALIZE_FUNCTION_OBJECT:
			{	// object is a normalize function compiler
				module::NormalizeFunctionConstructor* fc = dynamic_cast< module::NormalizeFunctionConstructor* >((*it)->constructor());
				if ( fc == NULL )
				{
					LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
						  << "'' is not a normalize function compiler";
					throw std::logic_error( "Object is not a normalize function compiler. See log." );
				}
				else	{
					std::string name = fc->objectClassName();
					boost::algorithm::to_upper( name);
					std::map< std::string, const module::NormalizeFunctionConstructor* >::const_iterator itr = m_normalizeFunctionCompilerMap.find( name );
					if ( itr != m_normalizeFunctionCompilerMap.end() )	{
						LOG_FATAL << "Duplicate normalize function compiler name '" << name << "'";
						throw std::runtime_error( "Duplicate normalize function compiler name" );
					}
					m_normalizeFunctionCompiler.push_back( fc );
					m_normalizeFunctionCompilerMap[ name ] = fc;

					LOG_TRACE << "'" << name << "' normalize function compiler registered";
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
					std::string name = ffo->programFileType();
					std::string key = boost::algorithm::to_upper_copy( name);
					std::map< std::string, const module::PrintFunctionConstructor* >::const_iterator itr = m_printFunctionCompilerMap.find( key );
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
	// Load the programs except database programs:
	bool success = true;
	for (std::vector<langbind::NormalizeFunctionConfigStruct>::const_iterator ii=conf->m_environment.normalize.begin(), ee=conf->m_environment.normalize.end(); ii != ee; ++ii)
	{
		success &= declareNormalizeFunction( ii->name, ii->type, ii->call);
	}
	success &= loadPrograms();

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

	for ( std::list< module::NormalizeFunctionConstructor* >::iterator it = m_normalizeFunctionCompiler.begin();
							it != m_normalizeFunctionCompiler.end(); ++it )
		delete *it;

	for ( std::list< module::PrintFunctionConstructor* >::iterator it = m_printFunctionCompiler.begin();
							it != m_printFunctionCompiler.end(); ++it )
		delete *it;
}

bool ProcessorProvider::ProcessorProvider_Impl::loadPrograms()
{
	bool rt = true;
	std::list< std::string >::const_iterator pi = m_programfiles.begin(), pe = m_programfiles.end();
	for (; pi != pe; ++pi)
	{
		std::string ext = utils::getFileExtension( *pi);
		if (ext.empty())
		{
			LOG_ERROR << "Program file has no extension. Cannot load '" << *pi << "'";
			rt = false;
		}
		else if (!boost::iequals( ext, ".tdl"))
		{
			std::string key = boost::algorithm::to_upper_copy( std::string( ext.c_str() + 1));
			std::map< std::string, ddl::DDLCompilerR >::const_iterator ci = m_ddlcompilerMap.find( key);
			if (ci != m_ddlcompilerMap.end())
			{
				rt &= loadForm( ci->second.get(), *pi);
				continue;
			}
			std::map< std::string, const module::PrintFunctionConstructor*>::const_iterator ri = m_printFunctionCompilerMap.find( key);
			if (ri != m_printFunctionCompilerMap.end())
			{
				rt &= loadPrintProgram( ri->second, *pi);
			}
		}
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
		// load database programs:
		std::list< std::string >::const_iterator pi = m_programfiles.begin(), pe = m_programfiles.end();
		for (; pi != pe; ++pi)
		{
			std::string ext = utils::getFileExtension( *pi);
			if (boost::iequals( ext, ".tdl"))
			{
				try
				{
					std::string dbsrc;
					m_program.loadfile( *pi, dbsrc);
					m_db->addProgram( dbsrc);
				}
				catch (const std::runtime_error& err)
				{
					LOG_ERROR << "failed to load transaction program '" << *pi << "': " << err.what();
					rt = false;
				}
			}
		}
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

langbind::FormFunction* ProcessorProvider::ProcessorProvider_Impl::formfunction( const std::string& name ) const
{
	std::string formfunctionName = boost::algorithm::to_upper_copy( name);
	std::map< std::string, const module::FormFunctionConstructor* >::const_iterator ffo = m_formfunctionMap.find( formfunctionName );
	if ( ffo == m_formfunctionMap.end() )
		return NULL;
	else
		return ffo->second->object();
}

bool ProcessorProvider::ProcessorProvider_Impl::declareFunctionName( const std::string& name, const char* typestr)
{
	std::string key = boost::algorithm::to_upper_copy( name);
	std::map< std::string, const char*>::const_iterator idt = m_langfunctionIdentifierMap.find( key);
	if (idt != m_langfunctionIdentifierMap.end())
	{
		LOG_ERROR << "Duplicate function identifier for "<< typestr << " with name '" << name << "' already used for a " << idt->second << "";
		return false;
	}
	m_langfunctionIdentifierMap[ key] = typestr;
	return true;
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

bool ProcessorProvider::ProcessorProvider_Impl::loadForm( const ddl::DDLCompiler* dc, const std::string& dataDefinitionFilename)
{
	try
	{
		DDLTypeMap typemap( this);
		std::pair< std::string, ddl::FormR> def;
		def.second.reset( new ddl::Form());
		try
		{
			*def.second = dc->compile( utils::readSourceFileContent( dataDefinitionFilename), &typemap);
		}
		catch (const std::exception& e)
		{
			std::ostringstream msg;
			msg << "could not compile data description file '" << dataDefinitionFilename << "': " << e.what() << std::endl;
			throw std::runtime_error( msg.str());
		}
		if (def.second->doctype())
		{
			def.first = utils::getIdFromDoctype( def.second->doctype());
		}
		else
		{
			def.first = utils::getFileStem( dataDefinitionFilename);
		}
		std::string formkey = boost::algorithm::to_upper_copy( def.first);
		m_formMap[ formkey] = def.second;

		LOG_TRACE << "Form '" << def.first << "' in '" << utils::getFileStem( dataDefinitionFilename) << "' loaded";
		return true;
	}
	catch (std::exception& e)
	{
		LOG_ERROR << e.what();
		return false;
	}
}

const ddl::Form* ProcessorProvider::ProcessorProvider_Impl::form( const std::string& name ) const
{
	std::string key = boost::algorithm::to_upper_copy( name);
	std::map <std::string, ddl::FormR>::const_iterator itr = m_formMap.find( key);
	if ( itr == m_formMap.end() )
		return NULL;
	else
		return itr->second.get();
}

bool ProcessorProvider::ProcessorProvider_Impl::loadPrintProgram( const module::PrintFunctionConstructor* pc, const std::string& layoutFilename)
{
	try
	{
		prnt::PrintFunctionR funcp( pc->object( utils::readSourceFileContent( layoutFilename)));
		std::string funcname( funcp->name());
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
		LOG_TRACE << "Print layout '" << funcname << "' loaded";
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
	std::map< std::string, prnt::PrintFunctionR>::const_iterator itr = m_printFunctionMap.find( key);
	if ( itr == m_printFunctionMap.end() )
		return NULL;
	else
		return itr->second.get();
}

bool ProcessorProvider::ProcessorProvider_Impl::declareNormalizeFunction( const std::string& name, const std::string& type, const std::string& command)
{
	try
	{
		std::string key = boost::algorithm::to_upper_copy( type);
		std::map< std::string, const module::NormalizeFunctionConstructor*>::const_iterator itr = m_normalizeFunctionCompilerMap.find( key);
		if (itr == m_normalizeFunctionCompilerMap.end())
		{
			LOG_ERROR << "Cannot declare normalize function '" << name << "'. Normalize function type '" << type << "' is not defined";
			return false;
		}
		langbind::NormalizeFunctionR funcp( itr->second->object( command));
		std::string funckey( boost::algorithm::to_upper_copy( name));

		std::map< std::string, langbind::NormalizeFunctionR>::const_iterator ip = m_normalizeFunctionMap.find( funckey);
		if (ip != m_normalizeFunctionMap.end())
		{
			LOG_ERROR << "Duplicate definition of normalize function with name '" << name << "'";
			return false;
		}
		m_normalizeFunctionMap[ funckey] = funcp;
		LOG_TRACE << "Normalize function '" << name << "' (" << type << ") declared";
		return true;
	}
	catch (std::exception& e)
	{
		LOG_ERROR << "Cannot declare '" << type << "' normalize function '" << name << "': " <<  e.what();
		return false;
	}
}

const langbind::NormalizeFunction* ProcessorProvider::ProcessorProvider_Impl::normalizeFunction( const std::string& name ) const
{
	std::string key = boost::algorithm::to_upper_copy( name);
	std::map< std::string, langbind::NormalizeFunctionR>::const_iterator itr = m_normalizeFunctionMap.find( key);
	if ( itr == m_normalizeFunctionMap.end() )
		return NULL;
	else
		return itr->second.get();
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

db::Database* ProcessorProvider::ProcessorProvider_Impl::transactionDatabase() const
{
	if ( ! m_db )
		LOG_ALERT << "No database defined for the processor provider";
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
	return m_program.function( name );
}

}} // namespace _Wolframe::proc