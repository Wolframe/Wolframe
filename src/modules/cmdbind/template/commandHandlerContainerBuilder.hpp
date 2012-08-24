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
///\file modules/cmdbind/template/commandHandlerContainerBuilder.hpp
///\brief Interface template for object builder of peer command handlers
#ifndef _Wolframe_MODULE_COMMAND_HANDLER_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#define _Wolframe_MODULE_COMMAND_HANDLER_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#include "cmdbind/commandHandler.hpp"
#include "moduleInterface.hpp"
#include "container.hpp"

namespace _Wolframe {
namespace module {

template <class CommandHandler>
class CommandHandlerContainer :public Container, public CommandHandler
{
public:
	CommandHandlerContainer( const char* name_)
		:CommandHandler()
		,m_name(name_){}

	virtual ~CommandHandlerContainer(){}

	virtual const char* objectName() const
	{
		return m_name.c_str();
	}

	virtual void dispose()	{ delete this; }
private:
	std::string m_name;
};

template <class CommandHandler>
class CommandHandlerContainerBuilder :public ContainerBuilder
{
public:
	CommandHandlerContainerBuilder( const char* name_)
		:ContainerBuilder(name_)
		,m_name(name_){}

	virtual ~CommandHandlerContainerBuilder(){}

	virtual Container* object()
	{
		return new CommandHandlerContainer<CommandHandler>(m_name.c_str());
	}

private:
	std::string m_name;
};

}}//namespace

#define DECLARE_COMMAND_HANDLER(NAME,CPPID,OBJ) \
namespace {\
struct CPPID\
{\
	static ContainerBuilder* constructor()\
	{\
		return new CommandHandlerContainerBuilder<OBJ>(NAME);\
	}\
};\
}//anonymous namespace
//end DECLARE_COMMAND_HANDLER

#endif

