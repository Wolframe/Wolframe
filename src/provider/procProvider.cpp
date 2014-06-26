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
// Wolframe processor provider
//

#include "processor/procProvider.hpp"
#include "cmdbind/commandHandlerConstructor.hpp"
#include "appdevel/module/runtimeEnvironmentConstructor.hpp"
#include "appdevel/module/filterBuilder.hpp"
#include "appdevel/module/ddlcompilerBuilder.hpp"
#include "appdevel/module/programTypeBuilder.hpp"
#include "appdevel/module/cppFormFunctionBuilder.hpp"
#include "appdevel/module/normalizeFunctionBuilder.hpp"
#include "appdevel/module/customDataTypeBuilder.hpp"
#include "appdevel/module/doctypeDetectorBuilder.hpp"
#include "logger-v1.hpp"
#include "module/moduleDirectory.hpp"
#include "utils/fileUtils.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <ostream>
#include <string>
#include <vector>

using namespace _Wolframe;
using namespace _Wolframe::proc;

class CombinedDoctypeDetector
	:public cmdbind::DoctypeDetector
{
public:
	CombinedDoctypeDetector( const std::vector<cmdbind::DoctypeDetectorType>& dtlist)
		:m_doctypes(&dtlist),m_nof_finished(0)
	{
		std::vector<cmdbind::DoctypeDetectorType>::const_iterator di = dtlist.begin(), de = dtlist.end();
		for (; di != de; ++di)
		{
			m_detectors.push_back( cmdbind::DoctypeDetectorR( di->create()));
			m_finished.push_back( false);
		}
		if (dtlist.empty())
		{
			m_lastError = "no module for document type/format detection loaded";
		}
	}

	/// \brief Implement cmdbind::DoctypeDetector::putInput( const std::string&,std::size)
	virtual void putInput( const char* chunk, std::size_t chunksize)
	{
		std::vector<cmdbind::DoctypeDetectorR>::const_iterator di = m_detectors.begin(), de = m_detectors.end();
		std::size_t idx = 0;
		for (; di != de; ++di,++idx)
		{
			if (!m_finished.at(idx))
			{
				(*di)->putInput( chunk, chunksize);
			}
		}
	}

	/// \brief Implement cmdbind::DoctypeDetector::run()
	virtual bool run()
	{
		try
		{
			if (!m_lastError.empty()) return false;

			std::size_t idx = 0;
			std::vector<cmdbind::DoctypeDetectorR>::const_iterator di = m_detectors.begin(), de = m_detectors.end();
			for (; di != de; ++di,++idx)
			{
				if (!m_finished.at(idx))
				{
					if ((*di)->run())
					{
						if ((*di)->info().get())
						{
							//... we got a positive result
							LOG_DEBUG << "document type/format detection matches for '" << m_doctypes->at(idx).name() << "'";
							m_info = (*di)->info();
							return true;
						}
						else
						{
							//... we got a negative result
							LOG_DEBUG << "document type/format detection for '" << m_doctypes->at(idx).name() << "' returned negative result";
							++m_nof_finished;
							m_finished[ idx] = true;

							if (m_nof_finished == m_finished.size())
							{
								//... all results are negative
								m_lastError = "document type/format not recognized";
								return false;
							}
						}
					}
					else
					{
						const char* err = (*di)->lastError();
						if (err)
						{
							m_lastError.append( err);
							return false;
						}
						
					}
				}
			}
		}
		catch (const std::runtime_error& e)
		{
			m_lastError = std::string("exception while detecing document type: ") + e.what();
		}
		return false;
	}

	/// \brief Implement cmdbind::DoctypeDetector::lastError()
	virtual const char* lastError() const
	{
		return m_lastError.empty()?0:m_lastError.c_str();
	}

	/// \brief Implement cmdbind::DoctypeDetector::info()
	const types::DoctypeInfoR& info() const
	{
		return m_info;
	}

private:
	const std::vector<cmdbind::DoctypeDetectorType>* m_doctypes;	///< list of sub document type detectors
	std::vector<cmdbind::DoctypeDetectorR> m_detectors;		///< list of all detector instances
	std::string m_lastError;					///< last error occurred
	std::vector<bool> m_finished;					///< bit field marking document type recognition termination
	std::size_t m_nof_finished;					///< count of finished document type detection processes (number of elements in m_nof_finished set to 'true')
	types::DoctypeInfoR m_info;					///< info object of the first positive match
};


ProcessorProvider::ProcessorProvider( const ProcProviderConfig* conf,
					const module::ModulesDirectory* modules,
					prgbind::ProgramLibrary* programs_)
	:m_programs(programs_),m_referencePath(conf->referencePath())
{
	m_db = NULL;
	if ( !conf->m_dbLabel.empty())
	{
		m_dbLabel = conf->m_dbLabel;
	}
	m_programfiles.insert( m_programfiles.end(), conf->programFiles().begin(), conf->programFiles().end());

	// Build the list of command handlers and runtime environments (configured objects)
	for ( std::list< config::NamedConfiguration* >::const_iterator it = conf->m_procConfig.begin();
									it != conf->m_procConfig.end(); it++ )	{
		module::ConfiguredBuilder* builder = modules->getBuilder((*it)->className());
		if ( builder )
		{
			if (builder->objectType() == ObjectConstructorBase::CMD_HANDLER_OBJECT)
			{
				cmdbind::CommandHandlerConstructorR constructor( dynamic_cast<cmdbind::CommandHandlerConstructor*>( builder->constructor()));

				if (!constructor.get())	{
					LOG_ALERT << "Wolframe Processor Provider: '" << builder->objectClassName()
						  << "' is not a command handler";
					throw std::logic_error( "Object is not a commandHandler. See log." );
				}
				else	{
					m_cmd.push_back( CommandHandlerDef( constructor->object( **it), *it));
				}
			}
			else if (builder->objectType() == ObjectConstructorBase::RUNTIME_ENVIRONMENT_OBJECT)
			{
				module::RuntimeEnvironmentConstructorR constructor( dynamic_cast<module::RuntimeEnvironmentConstructor*>( builder->constructor()));
				if (!constructor.get())
				{
					LOG_ALERT << "Wolframe Processor Provider: '" << builder->objectClassName()
						  << "' is not a runtime environment constructor";
					throw std::logic_error( "Object is not a runtime environment constructor. See log." );
				}
				else
				{
					langbind::RuntimeEnvironmentR env( constructor->object( **it));
					m_programs->defineRuntimeEnvironment( env);

					LOG_TRACE << "Registered runtime environment '" << env->name() << "'";
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
			case ObjectConstructorBase::DOCTYPE_DETECTOR_OBJECT:
			{
				// object defines a document type/format detector
				const module::DoctypeDetectorBuilder* bld = dynamic_cast<const module::DoctypeDetectorBuilder*>( *it);
				if (!bld)
				{
					LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
						  << "' is not a document type/format detector";
					throw std::logic_error( "Object is not a document type/format detector. See log." );
				}
				module::DoctypeDetectorConstructorR dtcr( dynamic_cast<module::DoctypeDetectorConstructor*>((*it)->constructor()));
				if (!dtcr.get())
				{
					LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
						  << "' is not a document type/format detector";
					throw std::logic_error( "Object is not a document type/format detector. See log." );
				}
				else
				{
					try
					{
						boost::shared_ptr<cmdbind::DoctypeDetectorType> type( dtcr->object());
						m_doctypes.push_back( *type);
						LOG_TRACE << "registered document type/format detection for '" << bld->name() << "' (" << (*it)->objectClassName() << ")";
					}
					catch (const std::runtime_error& e)
					{
						LOG_ERROR << "error loading document type/format detection object: " << e.what();
					}
				}
				break;
			}
			case ObjectConstructorBase::FILTER_OBJECT:	{	// object is a filter
				module::FilterConstructorR fltr( dynamic_cast< module::FilterConstructor* >((*it)->constructor()));
				if (!fltr.get())	{
					LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
						  << "' is not a filter";
					throw std::logic_error( "Object is not a filter. See log." );
				}
				else	{
					try
					{
						langbind::FilterTypeR filtertype( fltr->object());
						m_programs->defineFilterType( fltr->name(), filtertype);
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
				module::DDLCompilerConstructorR ffo( dynamic_cast< module::DDLCompilerConstructor* >((*it)->constructor()));
				if (!ffo.get())	{
					LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
						  << "' is not a DDL compiler";
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
				}
				break;
			}

			case ObjectConstructorBase::PROGRAM_TYPE_OBJECT:
			{	// object is a form function program type
				module::ProgramTypeConstructorR ffo( dynamic_cast< module::ProgramTypeConstructor* >((*it)->constructor()));
				if (!ffo.get())	{
					LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
						  << "' is not a program type";
					throw std::logic_error( "Object is not a program type. See log." );
				}
				else {
					try
					{
						prgbind::ProgramR prgtype( ffo->object());
						m_programs->defineProgramType( prgtype);
						LOG_TRACE << "registered '" << ffo->name() << "' program type";
					}
					catch (const std::runtime_error& e)
					{
						LOG_FATAL << "Error loading program type '" << ffo->name() << "':" << e.what();
					}
				}
				break;
			}

			case ObjectConstructorBase::FORM_FUNCTION_OBJECT:
			{	// object is a form function
				module::CppFormFunctionConstructorR ffo( dynamic_cast< module::CppFormFunctionConstructor* >((*it)->constructor()));
				if (!ffo.get())	{
					LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
						  << "' is not a form function";
					throw std::logic_error( "Object is not a form function. See log." );
				}
				else
				{
					try
					{
						m_programs->defineCppFormFunction( ffo->identifier(), ffo->function());
						LOG_TRACE << "registered C++ form function '" << ffo->identifier() << "'";
					}
					catch (const std::runtime_error& e)
					{
						LOG_FATAL << "Error loading form function object '" << ffo->objectClassName() << "':" << e.what();
					}
				}
				break;
			}

			case ObjectConstructorBase::NORMALIZE_FUNCTION_OBJECT:
			{	// object is a normalize function constructor
				module::NormalizeFunctionConstructorR constructor( dynamic_cast< module::NormalizeFunctionConstructor* >((*it)->constructor()));
				if ( !constructor.get() )
				{
					LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
						  << "' is not a normalize function constructor";
					throw std::logic_error( "Object is not a normalize function constructor. See log." );
				}
				else
				{
					try {
						m_programs->defineNormalizeFunctionType( constructor->identifier(), constructor->function());
						LOG_TRACE << "registered '" << constructor->objectClassName() << "' normalize function '" << constructor->identifier() << "'";
					}
					catch (const std::runtime_error& e)
					{
						LOG_FATAL << "Error loading normalize function object '" << constructor->objectClassName() << "':" << e.what();
					}
				}
				break;
			}

			case ObjectConstructorBase::CUSTOM_DATA_TYPE_OBJECT:
			{
				module::CustomDataTypeConstructorR constructor( dynamic_cast< module::CustomDataTypeConstructor* >((*it)->constructor()));
				if ( !constructor.get() )
				{
					LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
						  << "' is not a custom data type constructor";
					throw std::logic_error( "Object is not a custom data type constructor. See log." );
				}
				else
				{
					try {
						m_programs->defineCustomDataType( constructor->identifier(), constructor->object());
						LOG_TRACE << "registered '" << constructor->objectClassName() << "' custom data type '" << constructor->identifier() << "'";
					}
					catch (const std::runtime_error& e)
					{
						LOG_FATAL << "Error loading custom data type '" << constructor->objectClassName() << "':" << e.what();
					}
				}
				break;
			}

			case ObjectConstructorBase::AUDIT_OBJECT:
			case ObjectConstructorBase::AUTHENTICATION_OBJECT:
			case ObjectConstructorBase::AUTHORIZATION_OBJECT:
			case ObjectConstructorBase::JOB_SCHEDULE_OBJECT:
			case ObjectConstructorBase::DATABASE_OBJECT:
			case ObjectConstructorBase::CMD_HANDLER_OBJECT:
			case ObjectConstructorBase::RUNTIME_ENVIRONMENT_OBJECT:
			case ObjectConstructorBase::TEST_OBJECT:
				LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
					  << "' is marked as '" << ObjectConstructorBase::objectTypeName( it->objectType())
					  << "' object but has a simple object constructor";
				throw std::logic_error( "Object is not a valid simple object. See log." );
				break;
			default:
				LOG_ALERT << "Wolframe Processor Provider: '" << (*it)->objectClassName()
					  << "' is of an unknown object type";
				throw std::logic_error( "Object is not a valid simple object. See log." );
		}
	}
}


ProcessorProvider::~ProcessorProvider()
{}

bool ProcessorProvider::loadPrograms()
{
	try
	{
		// load all globally defined programs:
		m_programs->loadPrograms( m_db, m_programfiles);

		// load command handler programs and register commands 
		std::vector<CommandHandlerDef>::const_iterator di = m_cmd.begin(), de = m_cmd.end();
		for (; di != de; ++di)
		{
			if (!di->unit->loadPrograms( this))
			{
				LOG_ERROR << "failed to load command handler programs";
			}
			const std::vector<std::string>& cmds = di->unit->commands();
			for (std::vector<std::string>::const_iterator cmdIt = cmds.begin(); cmdIt != cmds.end(); cmdIt++)
			{
				types::keymap<std::size_t>::const_iterator ci = m_cmdMap.find( *cmdIt);
				if (ci != m_cmdMap.end())
				{
					const char* c1 = m_cmd.at(ci->second).configuration->className();
					const char* c2 = di->configuration->className();
					LOG_ERROR << "duplicate definition of command '" << *cmdIt << "' (in '" << c1 << "' and in '" << c2 << "')";
					throw std::runtime_error( "duplicate command definition");
				}
				else
				{
					m_cmdMap[ *cmdIt] = m_cmd.size()-1;
				}
				LOG_TRACE << "Command '" << *cmdIt << "' registered for '" << di->configuration->className() << "' command handler";
			}
		}
		return true;
	}
	catch (const std::runtime_error& e)
	{
		LOG_ERROR << "failed to load programs: " << e.what();
		return false;
	}
}

bool ProcessorProvider::resolveDB( const db::DatabaseProvider& db )
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

const types::AuthorizationFunction* ProcessorProvider::authorizationFunction( const std::string& name) const
{
	return m_programs->getAuthorizationFunction( name);
}

const types::NormalizeFunction* ProcessorProvider::normalizeFunction( const std::string& name) const
{
	return m_programs->getNormalizeFunction( name);
}

const types::NormalizeFunctionType* ProcessorProvider::normalizeFunctionType( const std::string& name) const
{
	return m_programs->getNormalizeFunctionType( name);
}

const langbind::FormFunction* ProcessorProvider::formFunction( const std::string& name) const
{
	LOG_TRACE << "[provider] get function '" << name << "'";
	return m_programs->getFormFunction( name);
}

const types::FormDescription* ProcessorProvider::formDescription( const std::string& name) const
{
	LOG_TRACE << "[provider] get form description '" << name << "'";
	return m_programs->getFormDescription( name);
}

const langbind::FilterType* ProcessorProvider::filterType( const std::string& name) const
{
	return m_programs->getFilterType( name);
}

const types::CustomDataType* ProcessorProvider::customDataType( const std::string& name) const
{
	LOG_TRACE << "[provider] get custom data type '" << name << "'";
	return m_programs->getCustomDataType( name);
}

cmdbind::DoctypeDetector* ProcessorProvider::doctypeDetector() const
{
	return new CombinedDoctypeDetector( m_doctypes);
}

cmdbind::CommandHandler* ProcessorProvider::cmdhandler( const std::string& command, const std::string& docformat) const
{
	types::keymap<std::size_t>::const_iterator ci = m_cmdMap.find( command);
	if (ci == m_cmdMap.end())
	{
		return 0;
	}
	cmdbind::CommandHandlerUnit* unit = m_cmd.at( ci->second).unit.get();
	return unit->createCommandHandler( command, docformat);
}

bool ProcessorProvider::existcmd( const std::string& command) const
{
	return m_cmdMap.find( command) != m_cmdMap.end();
}

db::Database* ProcessorProvider::transactionDatabase() const
{
	if ( ! m_db )	{
		LOG_ALERT << "No database defined for the processor provider";
	}
	return m_db;
}

db::Transaction* ProcessorProvider::transaction( const std::string& name ) const
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

const std::string& ProcessorProvider::referencePath() const
{
	return m_referencePath;
}

