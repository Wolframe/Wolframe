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
///\file tprocHandlerConfig.hpp
///\brief tproc handler configuration

#ifndef _Wolframe_TPROC_HANDLER_CONFIGURATION_HPP_INCLUDED
#define _Wolframe_TPROC_HANDLER_CONFIGURATION_HPP_INCLUDED
#include <vector>
#include <string>
#include "protocol/commandHandler.hpp"
#include "protocol/lineCommandHandler.hpp"
#include "config/descriptionBase.hpp"
#include "standardConfigs.hpp"

namespace _Wolframe {
namespace tproc {

struct CommandConfigStruct
{
	std::string name;		///< name of the command

	static const config::DescriptionBase* description();
};

struct ConfigurationStruct
{
	std::vector<CommandConfigStruct> command;
	std::size_t input_bufsize;			///< size of input network message buffers in bytes (should only be configured for testing)
	std::size_t output_bufsize;			///< size of output network message buffers in bytes (should only be configured for testing)

	ConfigurationStruct( const ConfigurationStruct& o)
		:command(o.command),input_bufsize(o.input_bufsize),output_bufsize(o.output_bufsize){}
	ConfigurationStruct()
		:input_bufsize(1024),output_bufsize(1024){}

	static const config::DescriptionBase* description();
};


class Configuration :public config::ConfigurationBase
{
public:
	Configuration();
	Configuration( const Configuration& o)	:config::ConfigurationBase(o),m_data(o.m_data){}

	///\brief parse the configuration and initialize this configuration structure
	bool parse( const config::ConfigurationTree& pt, const std::string& node, const module::ModulesDirectory* modules);

	///\brief interface implementation of ConfigurationBase::test() const
	virtual bool test() const;

	///\brief interface implementation of ConfigurationBase::check() const
	virtual bool check() const;

	///\brief interface implementation of ConfigurationBase::print(std::ostream& os, size_t indent) const
	virtual void print( std::ostream&, size_t indent=0) const;

	///\brief interface implementation of ConfigurationBase::setCanonicalPathes(const std::string&)
	virtual void setCanonicalPathes( const std::string&);

	///\brief return size of the buffer used for input network messages in bytes
	std::size_t input_bufsize() const		{return m_data.input_bufsize;}
	///\brief return size of the buffer used for output network messages in bytes
	std::size_t output_bufsize() const		{return m_data.output_bufsize;}

protected:
	ConfigurationStruct m_data;
};
}}//namespace
#endif


