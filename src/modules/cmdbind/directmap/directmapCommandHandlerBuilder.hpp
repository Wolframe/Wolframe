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
///\file directmapCommandHandlerBuilder.hpp
///\brief Interface directmap command handler builder
#ifndef _Wolframe_DIRECTMAP_COMMAND_HANDLER_BUILDER_HPP_INCLUDED
#define _Wolframe_DIRECTMAP_COMMAND_HANDLER_BUILDER_HPP_INCLUDED
#include "directmapCommandHandler.hpp"
#include "cmdbind/commandHandler.hpp"
#include "cmdbind/commandHandlerConstructor.hpp"
#include "cmdbind/ioFilterCommandHandler.hpp"
#include "config/ConfigurationTree.hpp"
#include "module/moduleInterface.hpp"
#include "module/constructor.hpp"
#include <boost/property_tree/ptree.hpp>
#include <string>
#include <map>

namespace _Wolframe {
namespace module {

///\brief Named configuration definition
class DirectmapCommandHandlerConfig
	:public config::NamedConfiguration
{
public:
	DirectmapCommandHandlerConfig( const char* classname_, const char* name, const char* logParent, const char* logName)
		:config::NamedConfiguration( name, logParent, logName)
		,m_classname(classname_){}
	virtual ~DirectmapCommandHandlerConfig(){}

	///\brief Parse the configuration
	///\param[in] pt configuration tree
	///\param[in] modules module directory
	virtual bool parse( const config::ConfigurationTree& pt, const std::string&, const ModulesDirectory* modules);

	///\brief Set canonical path for files referenced as relative path in configuration
	///\param[in] referencePath reference path
	virtual void setCanonicalPathes( const std::string& referencePath);

	virtual bool check() const;

	virtual bool checkReferences( const proc::ProcessorProvider* provider) const;

	virtual void print( std::ostream& os, size_t indent ) const;

	const cmdbind::DirectmapContext* context() const
	{
		return &m_context;
	}

	virtual const char* className() const
	{
		return m_classname;
	}

	std::list<std::string> commands() const
	{
		return m_context.commands();
	}

private:
	std::vector<std::string> m_programfiles;
	cmdbind::DirectmapContext m_context;
	const char* m_classname;
};


class DirectmapCommandHandlerConstructor
	:public cmdbind::CommandHandlerConstructor
{
public:
	DirectmapCommandHandlerConstructor(){}

	virtual ~DirectmapCommandHandlerConstructor(){}

	virtual cmdbind::DirectmapCommandHandler* object( const config::NamedConfiguration& cfgi)
	{
		const DirectmapCommandHandlerConfig* cfg = dynamic_cast<const DirectmapCommandHandlerConfig*>(&cfgi);
		if (!cfg) throw std::logic_error( "internal: wrong configuration interface passed to command handler constructor");
		cmdbind::DirectmapCommandHandler* rt = new cmdbind::DirectmapCommandHandler( cfg->context());
		return rt;
	}

	virtual const char* objectClassName() const
	{
		return "DirectmapCommandHandler";
	}

	virtual std::list<std::string> commands( const config::NamedConfiguration& cfgi) const
	{
		const DirectmapCommandHandlerConfig* cfg = dynamic_cast<const DirectmapCommandHandlerConfig*>(&cfgi);
		if (!cfg) throw std::logic_error( "internal: wrong configuration interface passed to command handler constructor");
		return cfg->commands();
	}

	virtual bool checkReferences( const config::NamedConfiguration& cfgi, const proc::ProcessorProvider* provider) const
	{
		const DirectmapCommandHandlerConfig* cfg = dynamic_cast<const DirectmapCommandHandlerConfig*>(&cfgi);
		if (!cfg) throw std::logic_error( "internal: wrong configuration interface passed to command handler constructor");
		return cfg->checkReferences( provider);
	}

};


class DirectmapCommandHandlerBuilder
	:public ConfiguredBuilder
{
public:
	DirectmapCommandHandlerBuilder( const char* classname_, const char* title, const char* section, const char* keyword, const char* id)
		:ConfiguredBuilder( title, section, keyword, id)
		,m_classname(classname_){}

	virtual ~DirectmapCommandHandlerBuilder(){}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return ObjectConstructorBase::CMD_HANDLER_OBJECT;
	}

	virtual config::NamedConfiguration* configuration( const char* logPrefix)
	{
		return new DirectmapCommandHandlerConfig( m_classname, m_title, logPrefix, m_keyword);
	}

	virtual ObjectConstructorBase* constructor()
	{
		return new DirectmapCommandHandlerConstructor();
	}

private:
	const char* m_classname;
};

}}//namespace

#endif

