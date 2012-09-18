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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file module/scriptCommandHandlerBuilder.hpp
///\brief Interface template for object builder for command handlers executing scripts with a configuration
#ifndef _Wolframe_MODULE_SCRIPT_COMMAND_HANDLER_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#define _Wolframe_MODULE_SCRIPT_COMMAND_HANDLER_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#include "cmdbind/commandHandler.hpp"
#include "cmdbind/commandHandlerUnit.hpp"
#include "cmdbind/ioFilterCommandHandler.hpp"
#include "processor/procProvider.hpp"
#include "langbind/scriptConfig_struct.hpp"
#include "logger-v1.hpp"
#include "moduleInterface.hpp"
#include "constructor.hpp"
#include <boost/type_traits.hpp>
#include <boost/concept_check.hpp>

namespace _Wolframe {
namespace module {

///\brief Named configuration definition based on a langbind::ScriptEnvironmentConfigStruct structure
///\tparam CommandHandlerType command handler of type cmdbind::IOFilterCommandHandler with a structure ContextStruct that describes the configured objects
template <class CommandHandlerType>
class ScriptCommandHandlerConfig
	:public config::NamedConfiguration
{
private:// Concept checks:

	///\brief Check for class CommandHandlerType::ContextStruct exists
	BOOST_STATIC_ASSERT((boost::is_class< typename CommandHandlerType::ContextStruct>::value));

	///\brief Check for method CommandHandlerType::ContextStruct::load with expected paraters
	template<typename T>
	struct struct_has_load_method
	{
		typedef char small_type;
		struct large_type {small_type dummy[2];};

		template<void (T::*)(const langbind::ScriptEnvironmentConfigStruct&, const module::ModulesDirectory*)> struct tester_member_signature;

		template<typename U>
		static small_type has_matching_member(tester_member_signature<&U::load>*);
		template<typename U>
		static large_type has_matching_member(...);

		static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
	};
	BOOST_STATIC_ASSERT(struct_has_load_method<typename CommandHandlerType::ContextStruct>::value);


public:
	typedef typename CommandHandlerType::ContextStruct ContextStruct;

public:
	ScriptCommandHandlerConfig( const char* classname_, const char* name, const char* logParent, const char* logName)
		:config::NamedConfiguration( name, logParent, logName)
		,m_modules(0)
		,m_classname(classname_){}
	virtual ~ScriptCommandHandlerConfig(){}

	virtual bool parse( const config::ConfigurationTree& pt, const std::string&, const module::ModulesDirectory* modules)
	{
		m_modules = modules;
		try
		{
			m_struct.initFromPropertyTree( (const boost::property_tree::ptree&)pt);
		}
		catch (std::runtime_error& e)
		{
			LOG_ERROR << e.what();
			return false;
		}
		return true;
	}

	virtual void setCanonicalPathes( const std::string& referencePath)
	{
		m_struct.setCanonicalPathes( referencePath);
		m_context.load( m_struct, m_modules);
	}

	virtual bool check() const
	{
		return m_struct.check();
	}

	virtual void print( std::ostream& os, size_t indent ) const
	{
		m_struct.print( os, indent);
	}

	const ContextStruct* context() const
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
	langbind::ScriptEnvironmentConfigStruct m_struct;
	const module::ModulesDirectory* m_modules;
	ContextStruct m_context;
	const char* m_classname;
};


template <class CommandHandlerType>
class ScriptCommandHandlerConstructor
	:public cmdbind::CommandHandlerConstructor
{
private:// Concept checks:

	///\brief Check CommandHandlerType :cmdbind::IOFilterCommandHandler
	BOOST_STATIC_ASSERT((boost::is_base_of< cmdbind::IOFilterCommandHandler, CommandHandlerType>::value));
	///\brief Check for class CommandHandlerType::ContextStruct exists with default constructor
	BOOST_STATIC_ASSERT((boost::is_class< typename CommandHandlerType::ContextStruct>::value));

public:
	typedef ScriptCommandHandlerConfig<CommandHandlerType> Config;

public:
	ScriptCommandHandlerConstructor(){}

	virtual ~ScriptCommandHandlerConstructor(){}

	virtual cmdbind::CommandHandler* object( const config::NamedConfiguration& cfgi)
	{
		const Config* cfg = dynamic_cast<const Config*>(&cfgi);
		if (!cfg) throw std::logic_error( "internal: wrong configuration interface passed to command handler constructor");
		CommandHandlerType* rt = new CommandHandlerType( cfg->context());
		return rt;
	}

	virtual const char* objectClassName() const
	{
		return CommandHandlerType::identifier();
	}

	virtual std::list<std::string> commands( const config::NamedConfiguration& cfgi) const
	{
		const Config* cfg = dynamic_cast<const Config*>(&cfgi);
		if (!cfg) throw std::logic_error( "internal: wrong configuration interface passed to command handler constructor");
		return cfg->commands();
	}
};


template <class CommandHandlerType>
class ScriptCommandHandlerBuilder :public ConfiguredBuilder
{
public:
	ScriptCommandHandlerBuilder( const char* classname_, const char* title, const char* section, const char* keyword, const char* id)
		:ConfiguredBuilder( title, section, keyword, id)
		,m_classname(classname_){}

	virtual ~ScriptCommandHandlerBuilder(){}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return ObjectConstructorBase::CMD_HANDLER_OBJECT;
	}

	virtual config::NamedConfiguration* configuration( const char* logPrefix)
	{
		return new ScriptCommandHandlerConfig<CommandHandlerType>( m_classname, m_title, logPrefix, m_keyword);
	}

	virtual ObjectConstructorBase* constructor()
	{
		return new ScriptCommandHandlerConstructor<CommandHandlerType>();
	}

private:
	const char* m_classname;
};

}}//namespace

#endif

