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
///\file tprocHandlerConfig.hpp
///\brief tproc handler configuration
#ifndef _Wolframe_TPROC_HANDLER_CONFIGURATION_HPP_INCLUDED
#define _Wolframe_TPROC_HANDLER_CONFIGURATION_HPP_INCLUDED
#include <vector>
#include <string>
#include "execCommandHandler.hpp"
#include "config/configurationBase.hpp"
#include "standardConfigs.hpp"

namespace _Wolframe {
namespace tproc {

///\class Configuration
///\brief Configuration structure
class Configuration :public config::ConfigurationBase
{
public:
	Configuration()
		:ConfigurationBase( "tproc", 0, "tproc")
		,m_input_bufsize(1024)
		,m_output_bufsize(1024) {}

	Configuration( const std::vector<cmdbind::ExecCommandHandler::Command>& cc, std::size_t ib, std::size_t ob)
		:config::ConfigurationBase("tproc", 0, "tproc")
		,m_commands(cc)
		,m_input_bufsize(ib)
		,m_output_bufsize(ob){}

	Configuration( const Configuration& o)
		:config::ConfigurationBase(o)
		,m_commands(o.m_commands)
		,m_input_bufsize(o.m_input_bufsize)
		,m_output_bufsize(o.m_output_bufsize){}

	///\brief interface implementation of ConfigurationBase::parse( const config::ConfigurationNode&, const std::string&, const module::ModulesDirectory*)
	bool parse( const config::ConfigurationNode& pt, const std::string& node, const module::ModulesDirectory* modules );

	///\brief interface implementation of ConfigurationBase::test() const
	virtual bool test() const;

	///\brief interface implementation of ConfigurationBase::check() const
	virtual bool check() const;

	///\brief interface implementation of ConfigurationBase::print(std::ostream& os, size_t indent) const
	virtual void print( std::ostream&, size_t indent=0) const;

	///\brief return size of the buffer used for input network messages in bytes
	std::size_t input_bufsize() const						{return m_input_bufsize;}
	///\brief return size of the buffer used for output network messages in bytes
	std::size_t output_bufsize() const						{return m_output_bufsize;}

	///\brief return all currently available commands
	const std::vector<cmdbind::ExecCommandHandler::Command>& commands() const	{return m_commands;}

private:
	std::vector<cmdbind::ExecCommandHandler::Command> m_commands;			//< command definitions
	std::size_t m_input_bufsize;							//< size of input network message buffers in bytes (not configured, but hardcoded by the messaging layer)
	std::size_t m_output_bufsize;							//< size of output network message buffers in bytes (not configured, but hardcoded by the messaging layer)

public:
	///\brief Set the buffer sizes for tests
	///\remark Should only be called in tests or by the core
	///\param[in] ib size of input buffer in bytes
	///\param[in] ob size of output buffer in bytes
	void setBuffers( std::size_t ib, std::size_t ob)
	{
		m_input_bufsize = ib;
		m_output_bufsize = ob;
	}
};
}}//namespace
#endif


