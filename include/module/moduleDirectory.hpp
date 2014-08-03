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
/// \file module/moduleDirectory.hpp
/// \brief Module objects directory

#ifndef _MODULE_DIRECTORY_HPP_INCLUDED
#define _MODULE_DIRECTORY_HPP_INCLUDED

#include <string>
#include <list>
#include "module/moduleInterface.hpp"

namespace _Wolframe {
namespace module {

/// \class ModulesDirectory
/// \brief The modules directory used by the constructors of the providers to build themselves.
class ModulesDirectory
{
public:
	/// \brief Constructor
	/// \param[in] confDir_ Configuration directory that specifies the relative path where to load the modules from if not specified by the system or explicitely
	explicit ModulesDirectory( const std::string& confDir_)
		:m_confDir(confDir_){}
	~ModulesDirectory();

	/// \brief Add a configured builder to the directory.
	bool addBuilder( ConfiguredBuilder* builder );

	/// \brief Add a simple builder (builder for objects without configuration) to the directory.
	bool addBuilder( SimpleBuilder* builder );

	/// \brief Get the builder for the configuration section, keyword pair.
	/// \param[in] section	the section (parent) of the configuration
	/// \param[in] keyword	the keyword in the section
	ConfiguredBuilder* getBuilder( const std::string& section, const std::string& keyword ) const;

	/// \brief Get the builder for the specified object class name
	ConfiguredBuilder* getBuilder( const std::string& objectClassName ) const;

	typedef std::list<SimpleBuilder*>::const_iterator simpleBuilder_iterator;
	typedef std::list<ConfiguredBuilder*>::const_iterator configuredBuilder_iterator;

	simpleBuilder_iterator simpleBuilderObjectsBegin() const		{ return m_simpleBuilder.begin(); }
	simpleBuilder_iterator simpleBuilderObjectsEnd() const			{ return m_simpleBuilder.end(); }
	configuredBuilder_iterator configuredBuilderObjectsBegin() const	{ return m_cfgdBuilder.begin(); }
	configuredBuilder_iterator configuredBuilderObjectsEnd() const		{ return m_cfgdBuilder.end(); }

	/// \brief Get a selected list of configurable objects loaded as tuple (section,keyword)
	/// \remark This function solves a henn and egg problem for the wolfilter program.
	/// \param[in] objtype type of the configurable objects to select
	/// \return The list of section,keyword tuples of configurable objects with the selected type
	std::vector<std::pair<std::string,std::string> > getConfigurableSectionKeywords( ObjectConstructorBase::ObjectType objtype ) const;

	/// \brief Get the absolute path of a module
	/// \brief param[in] moduleName name of the module to resolve
	/// \brief param[in] configuredDirectory not expanded name of directory configured as "directory" in the "LoadModules" section of the configuration
	/// \brief param[in] useDefaultModuleDir wheter to load modules from the default system module dir (only test programs may not do so)
	std::string getAbsoluteModulePath( const std::string& moduleName, const std::string& configuredDirectory, bool useDefaultModuleDir=true) const;

	/// \brief Load the list of modules specified by their absolute path
	/// \brief param[in] modFiles list of modules to load into this structure
	bool loadModules( const std::list<std::string>& modFiles);

private:
	std::string m_confDir;					///< configuration directory needed as base for calculating the absolute path of a module
	std::list< ConfiguredBuilder* >	m_cfgdBuilder;		///< list of configurable builders
	std::list< SimpleBuilder* >	m_simpleBuilder;	///< list of simple builders
};

}} // namespace _Wolframe::module

#endif // _MODULE_DIRECTORY_HPP_INCLUDED
