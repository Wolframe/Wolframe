/************************************************************************

 Copyright (C) 2011 Project Wolframe.
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
///\brief Implementation of the commands of the tproc connection handler
#include "countedReference.hpp"
#include "tprocHandlerConfig.hpp"
#include "protocol/lineCommandHandler.hpp"
#include "config/description.hpp"
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

using namespace _Wolframe;
using namespace _Wolframe::tproc;

const config::DescriptionBase* CommandConfigStruct::description()
{
	struct ThisDescription :public config::Description<CommandConfigStruct>
	{
		ThisDescription()
		{
			(*this)
			( "name",	&CommandConfigStruct::name)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const config::DescriptionBase* ConfigurationStruct::description()
{
	struct ThisDescription :public config::Description<ConfigurationStruct>
	{
		ThisDescription()
		{
			(*this)
			( "command",	&ConfigurationStruct::command)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

Configuration::Configuration()
	:ConfigurationBase( "Tproc", 0, "tproc") {}

bool Configuration::parse( const config::ConfigurationTree& pt, const std::string&, const module::ModulesDirectory*)
{
	try
	{
		std::string errmsg;
		if (!m_data.description()->parse( (void*)&m_data, (const boost::property_tree::ptree&)pt, errmsg))
		{
			LOG_ERROR << "Error in configuration: " << errmsg;
			return false;
		}
	}
	catch (std::exception& e)
	{
		LOG_ERROR << "Error parsing configuration: " << e.what();
		return false;
	}
	return true;
}

bool Configuration::test() const
{
	return true;
}

bool Configuration::check() const
{
	return true;
}

void Configuration::print( std::ostream& o, size_t i) const
{
	std::vector<CommandConfigStruct>::const_iterator itr=m_data.command.begin(),end=m_data.command.end();
	for (;itr!=end; ++itr)
	{
		while (i-->0) o << "\t";
		o << "Command " << itr->name;
	}
}

void Configuration::setCanonicalPathes( const std::string&)
{
}


