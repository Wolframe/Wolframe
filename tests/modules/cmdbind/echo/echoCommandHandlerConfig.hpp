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
/// \file echoCommandHandlerConfig.hpp
/// \brief Interface echo command handler configuration
#ifndef _Wolframe_ECHO_COMMAND_HANDLER_CONFIG_HPP_INCLUDED
#define _Wolframe_ECHO_COMMAND_HANDLER_CONFIG_HPP_INCLUDED
#include "serialize/descriptiveConfiguration.hpp"
#include <string>
#include <vector>

namespace _Wolframe {
namespace cmdbind {

/// \brief Echo command handler configuration definition
class EchoCommandHandlerConfig
	:public serialize::DescriptiveConfiguration
{
public:
	EchoCommandHandlerConfig( const char* title_="Echo", const char* sectionName_="Echo", const char* logParent_="CmdHandler", const char* logName_="Echo")
		:_Wolframe::serialize::DescriptiveConfiguration( sectionName_, logParent_, logName_, getStructDescription())
		,m_title(title_)
	{
		setBasePtr( (void*)this); // ... mandatory to set pointer to start of configuration
	}
	virtual ~EchoCommandHandlerConfig(){}

	const std::vector<std::string>& commands() const
	{
		return m_commands;
	}

public:
	/// \brief Structure description for serialization/parsing
	static const serialize::StructDescriptionBase* getStructDescription();

private:
	std::vector<std::string> m_commands;
	std::string m_title;
};

}}//namespace

#endif

