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
///\file appdevel/runtimeEnvironmentModuleMacros.hpp
///\brief Macros for a module for a configurable runtime environment for a binding language or a binding language universe
#include "appdevel/module/runtimeEnvironmentConstructor.hpp"
#include "module/moduleInterface.hpp"
#include "processor/procProviderInterface.hpp"
#include <boost/lexical_cast.hpp>

//\brief Defines a Wolframe module for programs with a runtime environment (e.g. programs with shared resources) after the includes section.
#define RUNTIME_ENVIRONMENT_MODULE(NAME,DESCRIPTION,CONFIG_SECTION,CONFIG_TITLE,CLASSDEF,CONFIGDEF)\
	static const char* _Wolframe__moduleName()\
	{\
		return #NAME;\
	}\
	static const char* _Wolframe__moduleDescription()\
	{\
		return DESCRIPTION;\
	}\
	class NAME ##Constructor \
		:public _Wolframe::module::RuntimeEnvironmentConstructor\
	{\
	public:\
		NAME ##Constructor(){}\
		virtual ~NAME ##Constructor(){}\
		virtual CLASSDEF* object( const _Wolframe::config::NamedConfiguration& cfgi)\
		{\
			CLASSDEF* rt = new CLASSDEF( cfgi);\
			return rt;\
		}\
		virtual const char* objectClassName() const\
		{\
			return #NAME;\
		}\
		virtual bool checkReferences( const _Wolframe::config::NamedConfiguration&, const _Wolframe::proc::ProcessorProviderInterface*) const\
		{\
			return true;\
		}\
	};\
	class NAME ##Builder\
		:public _Wolframe::module::ConfiguredBuilder\
	{\
	public:\
		NAME ##Builder()\
			:_Wolframe::module::ConfiguredBuilder( DESCRIPTION, CONFIG_SECTION, CONFIG_TITLE, #NAME)\
		{\
			int err;\
		}\
		virtual ~NAME ## Builder()\
		{\
		}\
		virtual _Wolframe::ObjectConstructorBase::ObjectType objectType() const\
		{\
			return _Wolframe::ObjectConstructorBase::RUNTIME_ENVIRONMENT_OBJECT;\
		}\
		virtual _Wolframe::config::NamedConfiguration* configuration( const char* logPrefix)\
		{\
			return new CONFIGDEF( #NAME, CONFIG_SECTION, logPrefix, CONFIG_TITLE);\
		}\
		virtual _Wolframe::ObjectConstructorBase* constructor()\
		{\
			return new NAME ## Constructor();\
		}\
	};



