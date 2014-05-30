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
// Processor Provider
//

#ifndef _PROCESSOR_PROVIDER_HPP_INCLUDED
#define _PROCESSOR_PROVIDER_HPP_INCLUDED

#include "processor/procProviderInterface.hpp"
#include "processor/procProviderConfig.hpp"
#include "database/database.hpp"
#include "cmdbind/commandHandlerUnit.hpp"
#include "cmdbind/doctypeDetector.hpp"
#include "prgbind/programLibrary.hpp"
#include "types/keymap.hpp"
#include <list>
#include <map>
#include <boost/noncopyable.hpp>

namespace _Wolframe {
namespace proc {

/// \class ProcessorProvider
/// \brief Processor provider, the class that provides access to configured global objects to processors
class ProcessorProvider
	:public ProcessorProviderInterface
	,private boost::noncopyable
{
public:
	/// \brief Constructor
	ProcessorProvider( const ProcProviderConfig* conf,
			   const module::ModulesDirectory* modules,
			   prgbind::ProgramLibrary* programs_);
	/// \brief Destructor
	virtual ~ProcessorProvider();

	/// \brief Pass the references to the built database interfaces and let the provider find its transaction database
	bool resolveDB( const db::DatabaseProvider& db );
	/// \brief Load all configured programs
	bool loadPrograms();

	/// \brief Create a new command handler for a command and for a document format (e.g. XML,JSON,...)
	virtual cmdbind::CommandHandler* cmdhandler( const std::string& command, const std::string& docformat) const;
	/// \brief Find out if a command with a specific name exists without creating a new command handler instance
	virtual bool existcmd( const std::string& command) const;

	/// \brief Get a reference to the transaction database
	virtual db::Database* transactionDatabase() const;

	/// \brief Return a database transaction object for a transaction identified by name
	virtual db::Transaction* transaction( const std::string& name ) const;

	/// \brief Get a reference to a normalization function identified by name
	virtual const types::NormalizeFunction* normalizeFunction( const std::string& name) const;

	/// \brief Get a reference to a normalization function type identified by name
	virtual const types::NormalizeFunctionType* normalizeFunctionType( const std::string& name) const;

	/// \brief Get a reference to a form function type identified by name
	virtual const langbind::FormFunction* formFunction( const std::string& name) const;

	/// \brief Get a reference to a form description identified by name
	virtual const types::FormDescription* formDescription( const std::string& name) const;

	/// \brief Get a reference to a filter type identified by name
	virtual const langbind::FilterType* filterType( const std::string& name) const;

	/// \brief Get a reference to a custom data type identified by name
	virtual const types::CustomDataType* customDataType( const std::string& name) const;

	/// \brief PF:HACK: Function to guess the document format of a content defined as string
	///	This method is here because document format recognition may be dependent on modules loaded
	virtual bool guessDocumentFormat( std::string& result, const char* content, std::size_t contentsize) const;

	/// \brief Create a new document type and format detector (defined in modules)
	/// \return a document type and format detector reference allocated (owned and deleted by the caller)
	virtual cmdbind::DoctypeDetector* doctypeDetector() const;

	/// \brief Get the application reference path for local path expansion
	virtual const std::string& referencePath() const;

private:
	std::string			m_dbLabel;	///< idenfifier of the transaction database
	db::Database*			m_db;		///< reference to the transaction database

	/// \class CommandHandlerDef
	/// \brief Definition of a command handler with its configuration
	class CommandHandlerDef
	{
	public:
		/// \brief Default constructor
		CommandHandlerDef()
			:configuration(0){}
		/// \brief Copy constructor
		CommandHandlerDef( const CommandHandlerDef& o)
			:unit(o.unit),configuration(o.configuration){}
		/// \brief Constructor
		CommandHandlerDef( cmdbind::CommandHandlerUnit* unit_, const config::NamedConfiguration* configuration_)
			:unit(unit_),configuration(configuration_){}
		/// \brief Destructor
		~CommandHandlerDef(){}

	public:
		cmdbind::CommandHandlerUnitR unit;			///< command handler unit to instantiate new command handlers
		const config::NamedConfiguration* configuration;	///< command handler configuration
	};
	std::vector<CommandHandlerDef> m_cmd;				///< list of defined command handlers
	types::keymap<std::size_t> m_cmdMap;				///< map of command names to indices in 'm_cmd'

	std::vector<cmdbind::DoctypeDetectorType> m_doctypes;		///< list of document type detectors loaded from modules
	std::vector<std::string> m_programfiles;			///< list of all programs to load
	prgbind::ProgramLibrary* m_programs;				///< program library
	std::string m_referencePath;					///< application reference path
};

}} // namespace _Wolframe::proc

#endif // _PROCESSOR_PROVIDER_HPP_INCLUDED
