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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
/// \file directmapCommandHandlerConfig.hpp
/// \brief Interface directmap command handler configuration
#ifndef _Wolframe_DIRECTMAP_COMMAND_HANDLER_CONFIG_HPP_INCLUDED
#define _Wolframe_DIRECTMAP_COMMAND_HANDLER_CONFIG_HPP_INCLUDED
#include "processor/execContext.hpp"
#include "config/configurationTree.hpp"
#include "module/moduleInterface.hpp"
#include "module/constructor.hpp"
#include <string>
#include <map>

namespace _Wolframe {
namespace cmdbind {

/// \class DirectmapCommandHandlerConfig
/// \brief Standard command handler configuration definition
class DirectmapCommandHandlerConfig
	:public config::NamedConfiguration
{
public:
	DirectmapCommandHandlerConfig( const char* classname_, const char* sectionName_, const char* logParent_, const char* logName_)
		:config::NamedConfiguration( sectionName_, logParent_, logName_)
		,m_classname(classname_){}
	virtual ~DirectmapCommandHandlerConfig(){}

	/// \brief Parse the configuration
	/// \param[in] pt configuration tree
	/// \param[in] modules module directory
	virtual bool parse( const config::ConfigurationNode& pt, const std::string&, const module::ModulesDirectory* modules);

	/// \brief Set canonical path for files referenced as relative path in configuration
	/// \param[in] referencePath reference path
	virtual void setCanonicalPathes( const std::string& referencePath);

	virtual bool check() const;

	virtual bool checkReferences( const proc::ProcessorProviderInterface* provider) const;

	virtual void print( std::ostream& os, size_t indent) const;

	virtual const char* className() const
	{
		return m_classname;
	}

	const std::vector<std::string>& programfiles() const
	{
		return m_programfiles;
	}

	const types::keymap<std::string>& filtermap() const
	{
		return m_filtermap;
	}

private:
	std::vector<std::string> m_programfiles;
	types::keymap<std::string> m_filtermap;
	const char* m_classname;
};

}}//namespace

#endif

