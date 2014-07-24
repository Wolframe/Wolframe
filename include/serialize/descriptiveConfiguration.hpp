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
/// \file serialize/descriptiveConfiguration.hpp
/// \brief Named configuration defined by description
#ifndef _Wolframe_serialize_DESCRIPTIVE_CONFIG_HPP_INCLUDED
#define _Wolframe_serialize_DESCRIPTIVE_CONFIG_HPP_INCLUDED
#include "config/configurationBase.hpp"
#include "config/configurationTree.hpp"
#include "serialize/configSerialize.hpp"
#include "logger-v1.hpp"

namespace _Wolframe {
namespace serialize {

class DescriptiveConfiguration
	:public config::NamedConfiguration
{
public:
	DescriptiveConfiguration( const char* classname_, const char* logparent, const char* logname, const serialize::StructDescriptionBase* descr_)
		:config::NamedConfiguration(classname_,logparent,logname),m_classname(classname_)
		,m_descr(descr_)
		,m_baseptr(0)
	{}

	void setBasePtr( void* baseptr_)
	{
		m_baseptr = baseptr_;
	}

	virtual ~DescriptiveConfiguration()
	{}

	const char* className() const
	{
		return m_classname.c_str();
	}

	bool parse( const config::ConfigurationNode& pt, const std::string& node,
			const module::ModulesDirectory*)
	{
		try
		{
			if (!m_baseptr) throw std::logic_error("configuration structure base pointer not set with setBasePtr(void*)");
			serialize::parseConfigStructureVP( m_baseptr, m_descr, pt);
			return true;
		}
		catch (const std::runtime_error& e)
		{
			LOG_ERROR << logPrefix() << e.what() << " in " << node << " " << pt.position().logtext();
			return false;
		}
	}

	bool check() const
	{
		return true;
	}

	virtual void print( std::ostream& os, size_t indent=0) const
	{
		std::string indentstr( indent, ' ');
		os << indentstr << "Configuration of " << m_classname << ":" << std::endl;
		os << indentstr << serialize::structureToStringVP( m_baseptr, m_descr);
	}

private:
	const std::string m_classname;
	const serialize::StructDescriptionBase* m_descr;
	void* m_baseptr;
};

}} //namespace
#endif

