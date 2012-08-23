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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file tprocHandlerConfig.cpp
///\brief Implementation of the tproc connection handler configuration
#include "types/countedReference.hpp"
#include "tprocHandlerConfig.hpp"
#include "langbind/appGlobalContext.hpp"
#include "cmdbind/ioFilterCommandHandler.hpp"
#include "cmdbind/directmapCommandHandler.hpp"
#include "cmdbind/commandHandler.hpp"
#include "config/structSerialize.hpp"
#include "serialize/struct/filtermapDescription.hpp"
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "utils/miscUtils.hpp"

using namespace _Wolframe;
using namespace _Wolframe::tproc;

struct ConfigurationStruct
{
	std::vector<std::string> cmd;			//< command definitions

	///\brief Get the configuration structure description
	static const serialize::StructDescriptionBase* getStructDescription();
};

const serialize::StructDescriptionBase* ConfigurationStruct::getStructDescription()
{
	struct ThisDescription :public serialize::StructDescription<ConfigurationStruct>
	{
		ThisDescription()
		{
			(*this)
			( "cmd",	&ConfigurationStruct::cmd)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

bool Configuration::parse( const config::ConfigurationTree& pt, const std::string&, const module::ModulesDirectory*)
{
	try
	{
		ConfigurationStruct data;
		config::parseConfigStructure( data, (const boost::property_tree::ptree&)pt);

		std::vector<std::string>::const_iterator itr=data.cmd.begin();
		while (itr != data.cmd.end())
		{
			std::stringstream cmdstr( *itr);
			std::string nam;

			if (std::getline( cmdstr, nam, '='))
			{
				m_commands.push_back( cmdbind::ExecCommandHandler::Command( nam, cmdstr.str()));
			}
			else
			{
				m_commands.push_back( cmdbind::ExecCommandHandler::Command( *itr));
			}
			++itr;
		}
		return true;
	}
	catch (std::exception& e)
	{
		LOG_ERROR << "Error parsing configuration: " << e.what();
		return false;
	}
}

bool Configuration::test() const
{
	bool rt = true;
	langbind::GlobalContext* gctx = langbind::getGlobalContext();
	std::vector<cmdbind::ExecCommandHandler::Command>::const_iterator itr=m_commands.begin();
	while (itr != m_commands.end())
	{
		langbind::TransactionFunctionR tf;
#if WITH_LUA
		langbind::LuaScriptInstanceR li;
		if (gctx->getLuaScriptInstance( itr->m_procname, li))
		{}
		else
#endif
		if (gctx->getTransactionFunction( itr->m_procname, tf))
		{}
		else
		{
			LOG_ERROR << "Error in configuration. Function not found: '" << itr->m_procname << "'";
			rt = false;
		}

		++itr;
	}
	return rt;
}

bool Configuration::check() const
{
	return test();
}

void Configuration::print( std::ostream& o, size_t i) const
{
	std::vector<cmdbind::ExecCommandHandler::Command>::const_iterator itr=m_commands.begin(),end=m_commands.end();
	std::string ind( '\t', i);
	for (;itr!=end; ++itr)
	{
		o << ind << "command '" << itr->m_cmdname << "'" << " -> '" << itr->m_procname << "'" << std::endl;
	}
}


