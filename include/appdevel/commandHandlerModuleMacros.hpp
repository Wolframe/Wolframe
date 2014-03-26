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
///\file appdevel/commandHandlerModuleMacros.hpp
///\brief Macros for a module for a configurable command handler and program type for a binding language
#include "module/moduleInterface.hpp"
#include "module/programTypeBuilder.hpp"
#include "cmdbind/commandHandlerConstructor.hpp"
#include "cmdbind/commandHandler.hpp"
#include "processor/procProviderInterface.hpp"

//\brief Defines a Wolframe command handler module after the includes section.
#define COMMAND_HANDLER_MODULE(DESCRIPTION,LANGNAME,CONFIG_SECTION,CONFIG_TITLE,CLASSDEF,CONFIGDEF)\
	class LANGNAME ##CommandHandlerConstructor\
		:public _Wolframe::cmdbind::CommandHandlerConstructor\
	{\
	public:\
		LANGNAME ##CommandHandlerConstructor(){}\
		virtual ~LANGNAME ##CommandHandlerConstructor(){}\
		virtual _Wolframe::cmdbind::CommandHandler* object( const _Wolframe::config::NamedConfiguration& cfgi)\
		{\
			const CONFIGDEF* cfg = dynamic_cast<const CONFIGDEF*>(&cfgi);\
			if (!cfg) throw std::logic_error( "internal: wrong configuration interface passed to " #LANGNAME " command handler constructor");\
			CLASSDEF* rt = new CLASSDEF( cfg->context());\
			return rt;\
		}\
		virtual const char* objectClassName() const\
		{\
			return #LANGNAME "CommandHandler";\
		}\
		virtual std::list<std::string> commands( const _Wolframe::config::NamedConfiguration& cfgi) const\
		{\
			const CONFIGDEF* cfg = dynamic_cast<const CONFIGDEF*>(&cfgi);\
			if (!cfg) throw std::logic_error( "internal: wrong configuration interface passed to " #LANGNAME " command handler method");\
			return cfg->commands();\
		}\
		virtual bool checkReferences( const _Wolframe::config::NamedConfiguration&, const _Wolframe::proc::ProcessorProviderInterface*) const	{return true;}\
	};\
	class LANGNAME ##CommandHandlerBuilder\
		:public _Wolframe::module::ConfiguredBuilder\
	{\
	public:\
		LANGNAME ##CommandHandlerBuilder()\
			:_Wolframe::module::ConfiguredBuilder( DESCRIPTION, CONFIG_SECTION, CONFIG_TITLE, #LANGNAME)\
		{}\
		virtual ~LANGNAME ##CommandHandlerBuilder(){}\
		virtual _Wolframe::ObjectConstructorBase::ObjectType objectType() const\
		{\
			return _Wolframe::ObjectConstructorBase::CMD_HANDLER_OBJECT;\
		}\
		virtual _Wolframe::config::NamedConfiguration* configuration( const char* logPrefix)\
		{\
			return new CONFIGDEF( #LANGNAME, m_title, logPrefix, m_keyword);\
		}\
		virtual _Wolframe::ObjectConstructorBase* constructor()\
		{\
			return new LANGNAME ##CommandHandlerConstructor();\
		}\
	};\
	static _Wolframe::module::ConfiguredBuilder* createCommandHandler()\
	{\
		static LANGNAME ##CommandHandlerBuilder rt;\
		return &rt;\
	}\
	enum {NofConfiguredBuilder=1};\
	static _Wolframe::module::ConfiguredBuilder* (*configuredBuilder[ NofConfiguredBuilder])() =\
	{\
		createCommandHandler\
	};\
	extern "C" {\
		_Wolframe::module::ModuleEntryPoint \
		entryPoint( 0, "command handler and form function handler for " #LANGNAME,\
				NofConfiguredBuilder, configuredBuilder,\
				0, 0);\
	}


//\brief Defines a Wolframe command handler with standalone programs module after the includes section.
#define COMMAND_HANDLER_MODULE_WITH_PROGRAMS(DESCRIPTION,LANGNAME,CONFIG_SECTION,CONFIG_TITLE,CLASSDEF,CONFIGDEF,CREATEPRGFUNC)\
	class LANGNAME ##CommandHandlerConstructor\
		:public _Wolframe::cmdbind::CommandHandlerConstructor\
	{\
	public:\
		LANGNAME ##CommandHandlerConstructor(){}\
		virtual ~LANGNAME ##CommandHandlerConstructor(){}\
		virtual _Wolframe::cmdbind::CommandHandler* object( const _Wolframe::config::NamedConfiguration& cfgi)\
		{\
			const CONFIGDEF* cfg = dynamic_cast<const CONFIGDEF*>(&cfgi);\
			if (!cfg) throw std::logic_error( "internal: wrong configuration interface passed to " #LANGNAME " command handler constructor");\
			CLASSDEF* rt = new CLASSDEF( cfg->context());\
			return rt;\
		}\
		virtual const char* objectClassName() const\
		{\
			return #LANGNAME "CommandHandler";\
		}\
		virtual std::list<std::string> commands( const _Wolframe::config::NamedConfiguration& cfgi) const\
		{\
			const CONFIGDEF* cfg = dynamic_cast<const CONFIGDEF*>(&cfgi);\
			if (!cfg) throw std::logic_error( "internal: wrong configuration interface passed to " #LANGNAME " command handler method");\
			return cfg->commands();\
		}\
		virtual bool checkReferences( const _Wolframe::config::NamedConfiguration&, const _Wolframe::proc::ProcessorProviderInterface*) const	{return true;}\
	};\
	class LANGNAME ##CommandHandlerBuilder\
		:public _Wolframe::module::ConfiguredBuilder\
	{\
	public:\
		LANGNAME ##CommandHandlerBuilder()\
			:_Wolframe::module::ConfiguredBuilder( DESCRIPTION, CONFIG_SECTION, CONFIG_TITLE, #LANGNAME)\
		{}\
		virtual ~LANGNAME ##CommandHandlerBuilder(){}\
		virtual _Wolframe::ObjectConstructorBase::ObjectType objectType() const\
		{\
			return _Wolframe::ObjectConstructorBase::CMD_HANDLER_OBJECT;\
		}\
		virtual _Wolframe::config::NamedConfiguration* configuration( const char* logPrefix)\
		{\
			return new CONFIGDEF( #LANGNAME, m_title, logPrefix, m_keyword);\
		}\
		virtual _Wolframe::ObjectConstructorBase* constructor()\
		{\
			return new LANGNAME ##CommandHandlerConstructor();\
		}\
	};\
	static _Wolframe::module::ConfiguredBuilder* createCommandHandler()\
	{\
		static LANGNAME ##CommandHandlerBuilder rt;\
		return &rt;\
	}\
	static _Wolframe::module::SimpleBuilder* createProgramType()\
	{\
		return new _Wolframe::module::ProgramTypeBuilder( #LANGNAME "ProgramType", #LANGNAME "FormFunc", CREATEPRGFUNC);\
	}\
	enum {NofConfiguredBuilder=1};\
	static _Wolframe::module::ConfiguredBuilder* (*configuredBuilder[ NofConfiguredBuilder])() =\
	{\
		createCommandHandler\
	};\
	enum {NofSimpleBuilder=1};\
	static _Wolframe::module::SimpleBuilder* (*simpleBuilder[ NofSimpleBuilder])() =\
	{\
		createProgramType\
	};\
	extern "C" {\
		_Wolframe::module::ModuleEntryPoint \
		entryPoint( 0, "command handler and form function program type for " #LANGNAME,\
				NofConfiguredBuilder, configuredBuilder,\
				NofSimpleBuilder, simpleBuilder);\
	}
