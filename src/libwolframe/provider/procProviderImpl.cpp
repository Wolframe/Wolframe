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
// Wolframe processor provider implementation
//

#include "processor/procProvider.hpp"
#include "procProviderImpl.hpp"
#include "module/ddlcompilerBuilder.hpp"
#include "module/cppFormFunctionBuilder.hpp"
#include "module/normalizeFunctionBuilder.hpp"
#include "module/printFunctionBuilder.hpp"
#include "module/programTypeBuilder.hpp"
#include "prgbind/runtimeEnvironmentConstructor.hpp"
#include "types/doctype.hpp"
#include "config/valueParser.hpp"
#include "config/ConfigurationTree.hpp"
#include "logger-v1.hpp"
#include "processor/moduleDirectory.hpp"
#include "utils/fileUtils.hpp"
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
					m_cmd.push_back( CommandHandlerDef( cnstrctr, *it));

					// register handler commands
					std::list<std::string> cmds = cnstrctr->commands( **it);
					for (std::list<std::string>::const_iterator cmdIt = cmds.begin(); cmdIt != cmds.end(); cmdIt++)
					{
						std::string opName = boost::algorithm::to_upper_copy( *cmdIt );
						CmdMap::const_iterator ci = m_cmdMap.find( opName);
						if (ci != m_cmdMap.end())
						{
							const char* c1 = m_cmd.at(ci->second).configuration->className();
							const char* c2 = (*it)->className();
							LOG_ERROR << "duplicate definition of command '" << opName << "' (in '" << c1 << "' and in '" << c2 << "')";
							throw std::runtime_error( "duplicate command definition");
						}
						else
						{
							m_cmdMap[ opName ] = m_cmd.size()-1;
						}
						LOG_TRACE << "Command '" << opName << "' registered for '" << cnstrctr->objectClassName() << "' command handler";
					}
				}
			}
			else if (builder->objectType() == ObjectConstructorBase::RUNTIME_ENVIRONMENT_OBJECT)
			{
				prgbind::RuntimeEnvironmentConstructor* constructor =
					dynamic_cast<prgbind::RuntimeEnvironmentConstructor*>( builder->constructor());

				if (!constructor)	{
					LOG_ALERT << "Wolframe Processor Provider: '" << builder->objectClassName()
						  << "' is not a runtime environment constructor";
					throw std::logic_error( "Object is not a runtime environment constructor. See log." );
				}
				else
				{
					prgbind::RuntimeEnvironmentConstructorR cref( constructor);
					m_runtime_environment_defs.push_back( prgbind::RuntimeEnvironmentDef( cref, *it));
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
				module::FilterConstructorR fltr( dynamic_cast< module::FilterConstructor* >((*it)->constructor()));
				if ( fltr.get() == NULL )	{
					LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
						  << "'' is not a filter";
					throw std::logic_error( "Object is not a filter. See log." );
				}
				else	{
					try
					{
						m_programs->defineFilterConstructor( fltr);
						LOG_TRACE << "registered filter '" << fltr->name() << "' (" << fltr->objectClassName() << ")";
					}
					catch (const std::runtime_error& e)
					{
						LOG_ERROR << "error loading filter object module: " << e.what();
					}
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
				else {
					try
					{
						langbind::DDLCompilerR constructor( ffo->object());
						m_programs->defineFormDDL( constructor);
						LOG_TRACE << "registered '" << constructor->ddlname() << "' DDL compiler";
					}
					catch (const std::runtime_error& e)
					{
						LOG_FATAL << "Error loading DDL compiler '" << ffo->name() << "':" << e.what();
					}
					delete ffo;
				}
				break;
			}

			case ObjectConstructorBase::PROGRAM_TYPE_OBJECT:
			{	// object is a form function program type
				module::ProgramTypeConstructor* ffo = dynamic_cast< module::ProgramTypeConstructor* >((*it)->constructor());
				if ( ffo == NULL )	{
					LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
						  << "'' is not a DDL compiler";
					throw std::logic_error( "Object is not a form function. See log." );
				}
				else {
					try
					{
						prgbind::ProgramR prgtype( ffo->object());
						m_programs->defineProgramType( prgtype);
						LOG_TRACE << "registered '" << ffo->name() << "' form function program type";
					}
					catch (const std::runtime_error& e)
					{
						LOG_FATAL << "Error loading form function program type '" << ffo->name() << "':" << e.what();
					}
					delete ffo;
				}
				break;
			}

			case ObjectConstructorBase::FORM_FUNCTION_OBJECT:
			{	// object is a form function
				module::CppFormFunctionConstructor* ffo = dynamic_cast< module::CppFormFunctionConstructor* >((*it)->constructor());
				if ( ffo == NULL )	{
					LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
						  << "'' is not a form function";
					throw std::logic_error( "Object is not a form function. See log." );
				}
				else	{
					try
					{
						std::string name = ffo->objectClassName();
						langbind::CppFormFunctionR func( ffo->object());
						m_programs->defineCppFormFunction( name, *func);
						LOG_TRACE << "registered '" << name << "' built-in form function ";
					}
					catch (const std::runtime_error& e)
					{
						LOG_FATAL << "Error loading form function object '" << ffo->objectClassName() << "':" << e.what();
					}
					delete ffo;
				}
				break;
			}

			case ObjectConstructorBase::NORMALIZE_FUNCTION_OBJECT:
			{	// object is a normalize function constructor
				module::NormalizeFunctionConstructorR constructor( dynamic_cast< module::NormalizeFunctionConstructor* >((*it)->constructor()));
				if ( !constructor.get() )
				{
					LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
						  << "'' is not a normalize function constructor";
					throw std::logic_error( "Object is not a normalize function constructor. See log." );
				}
				else
				{
					try {
						m_programs->defineNormalizeFunctionConstructor( constructor);
						LOG_TRACE << "registered '" << constructor->objectClassName() << "' normalize function constructor for domain '" << constructor->domain() << "'";
					}
					catch (const std::runtime_error& e)
					{
						LOG_FATAL << "Error loading normalize function object '" << constructor->domain() << "':" << e.what();
					}
				}
				break;
			}

			case ObjectConstructorBase::PRINT_FUNCTION_OBJECT:
			{	// object is a print function compiler
				module::PrintFunctionConstructorR constructor( dynamic_cast< module::PrintFunctionConstructor* >((*it)->constructor()));
				if (!constructor.get())
				{
					LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
						  << "'' is not a print layout description compiler";
					throw std::logic_error( "Object is not a print layout description compiler. See log." );
				}
				else
				{
					try {
						m_programs->definePrintLayoutType( constructor);
						LOG_TRACE << "'" << constructor->programFileType() << "' print layout description compiler registered";
					}
					catch (const std::runtime_error& e)
					{
						LOG_FATAL << "Error loading normalize function object '" << constructor->name() << "':" << e.what();
					}
				}
				break;
			}
			case ObjectConstructorBase::LANGUAGE_EXTENSION_OBJECT:
				// ... language extension modules are not handled here
				break;

			case ObjectConstructorBase::AUDIT_OBJECT:
			case ObjectConstructorBase::AUTHENTICATION_OBJECT:
			case ObjectConstructorBase::AUTHORIZATION_OBJECT:
			case ObjectConstructorBase::JOB_SCHEDULE_OBJECT:
			case ObjectConstructorBase::DATABASE_OBJECT:
			case ObjectConstructorBase::CMD_HANDLER_OBJECT:
			case ObjectConstructorBase::RUNTIME_ENVIRONMENT_OBJECT:
			case ObjectConstructorBase::TEST_OBJECT:
				LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
					  << "'' is marked as '" << ObjectConstructorBase::objectTypeName( it->objectType())
					  << "' object but has a simple object constructor";
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
{}

bool ProcessorProvider::ProcessorProvider_Impl::loadPrograms()
{
	try
	{
		// load all locally defined programs of the database:
		if (m_db) m_db->loadAllPrograms();

		// load all globally defined programs:
		m_programs->loadPrograms( transactionDatabase( true), m_programfiles);
		return true;

		// load functions based on a configured runtime environment:
		std::vector<prgbind::RuntimeEnvironmentDef>::const_iterator ri = m_runtime_environment_defs.begin(), re = m_runtime_environment_defs.end();
		for (; ri != re; ++ri)
		{
			prgbind::RuntimeEnvironmentR env( ri->constructor->object( *ri->configuration));
			m_runtime_environments.push_back( env);

			std::vector<std::string> functionlist = env->functions();
			std::vector<std::string>::const_iterator fi = functionlist.begin(), fe = functionlist.end();
			for (; fi != fe; ++fi)
			{
				langbind::FormFunctionR func( new prgbind::RuntimeEnvironmentFormFunction( *fi, env.get()));
				m_programs->defineFormFunction( *fi, func);
				LOG_TRACE << "Function '" << *fi << "' registered as '" << ri->constructor->objectClassName() << "' function";
			}
		}
	}
	catch (const std::runtime_error& e)
	{
		LOG_ERROR << "failed to load programs: " << e.what();
		return false;
	}
}

bool ProcessorProvider::ProcessorProvider_Impl::checkReferences( const ProcessorProvider* provider) const
{
	bool rt = true;
	std::vector<CommandHandlerDef>::const_iterator ci = m_cmd.begin(), ce = m_cmd.end();
	for (; ci != ce; ++ci)
	{
		if (!ci->constructor->checkReferences( *ci->configuration, provider))
		{
			rt = false;
			LOG_ERROR << "invalid references found in configuration of '" << ci->configuration->sectionName() << "' '" << ci->configuration->className() << "'";
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
	}
	return rt;
}

const types::NormalizeFunction* ProcessorProvider::ProcessorProvider_Impl::normalizeFunction( const std::string& name) const
{
	return m_programs->getNormalizeFunction( name);
}

const langbind::FormFunction* ProcessorProvider::ProcessorProvider_Impl::formFunction( const std::string& name) const
{
	LOG_TRACE << "[provider] get function '" << name << "'";
	return m_programs->getFormFunction( name);
}

const types::FormDescription* ProcessorProvider::ProcessorProvider_Impl::formDescription( const std::string& name) const
{
	LOG_TRACE << "[provider] get form description '" << name << "'";
	return m_programs->getFormDescription( name);
}

static std::string filterargAsString( const std::vector<langbind::FilterArgument>& arg)
{
	std::ostringstream out;
	std::vector<langbind::FilterArgument>::const_iterator ai = arg.begin(), ae = arg.end();
	for (; ai != ae; ++ai)
	{
		if (ai != arg.begin()) out << ", ";
		out << ai->first << "='" << ai->second << "'";
	}
	return out.str();
}

langbind::Filter* ProcessorProvider::ProcessorProvider_Impl::filter( const std::string& name, const std::vector<langbind::FilterArgument>& arg) const
{
	LOG_TRACE << "[provider] get filter '" << name << "(" << filterargAsString(arg) << ")'";
	return m_programs->createFilter( name, arg);
}

cmdbind::CommandHandler* ProcessorProvider::ProcessorProvider_Impl::cmdhandler( const std::string& command) const
{
	std::string cmdName = boost::algorithm::to_upper_copy( command );
	std::map<std::string, std::size_t>::const_iterator cmd = m_cmdMap.find( cmdName );
	if ( cmd == m_cmdMap.end() )
	{
		return NULL;
	}
	cmdbind::CommandHandlerConstructor* constructor = m_cmd.at( cmd->second).constructor.get();
	const config::NamedConfiguration* cfg = m_cmd.at( cmd->second).configuration;
	return constructor->object( *cfg);
}

cmdbind::IOFilterCommandHandler* ProcessorProvider::ProcessorProvider_Impl::iofilterhandler( const std::string& command ) const
{
	LOG_TRACE << "[provider] get iofilter command handler '" << command << "'";
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
	{
		LOG_TRACE << "[provider] get transaction '" << name << "'";
		return m_db->transaction( name );
	} else	{
		LOG_ALERT << "No database defined for the processor provider";
		return NULL;
	}
}

std::string ProcessorProvider::ProcessorProvider_Impl::xmlDoctypeString( const std::string& formname, const std::string& ddlname, const std::string& xmlroot) const
{
	std::ostringstream rt;
	rt << xmlroot << " SYSTEM \"" << formname << "." << ddlname << "\"";
	return rt.str();
}

}} // namespace _Wolframe::proc

