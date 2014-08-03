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
/// \file appdevel/protocolHandlerModuleMacros.hpp
/// \brief Macros for a module for a protocol handler
#include "module/moduleInterface.hpp"
#include "module/constructor.hpp"
#include "cmdbind/protocolHandler.hpp"
#include "processor/procProviderInterface.hpp"

/// \brief Defines a Wolframe protocol handler module after the includes section.
#define WF_PROTOCOL_HANDLER(NAME,CLASSDEF)\
{\
	class ProtocolHandlerConstructor\
		:public _Wolframe::SimpleObjectConstructor<_Wolframe::cmdbind::ProtocolHandlerUnit>\
	{\
	public:\
		ProtocolHandlerConstructor(){}\
		virtual ~ProtocolHandlerConstructor(){}\
		virtual _Wolframe::cmdbind::ProtocolHandlerUnit* object() const\
		{\
			return new CLASSDEF();\
		}\
		virtual const char* objectClassName() const\
		{\
			return NAME "ProtocolHandler";\
		}\
		virtual ObjectConstructorBase::ObjectType objectType() const\
		{\
			return ObjectConstructorBase::CMD_HANDLER_OBJECT;\
		}\
	};\
	class ProtocolHandlerBuilder\
		:public _Wolframe::module::SimpleBuilder\
	{\
	public:\
		ProtocolHandlerBuilder()\
			:_Wolframe::module::SimpleBuilder(NAME)\
		{}\
		virtual ~ProtocolHandlerBuilder(){}\
		virtual _Wolframe::ObjectConstructorBase::ObjectType objectType() const\
		{\
			return _Wolframe::ObjectConstructorBase::PROTOCOL_HANDLER_OBJECT;\
		}\
		virtual _Wolframe::ObjectConstructorBase* constructor()\
		{\
			return new ProtocolHandlerConstructor();\
		}\
	};\
	struct Constructor\
	{\
		static _Wolframe::module::BuilderBase* impl()\
		{\
			return new ProtocolHandlerBuilder();\
		}\
	};\
	(*this)(&Constructor ::impl);\
}

