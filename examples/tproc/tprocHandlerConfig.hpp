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
#include "config/descriptionBase.hpp"
#include "ddl/compilerInterface.hpp"
#include "standardConfigs.hpp"

namespace _Wolframe {
namespace tproc {

struct ScriptConfigStruct
{
	std::string name;
	std::string path;
	std::string main;
	std::vector<std::string> module;

	static const config::DescriptionBase* description();
};

struct FormConfigStruct
{
	std::string path;
	std::string name;

	static const config::DescriptionBase* description();
};

struct DirectMapConfigStruct
{
	std::string name;
	std::string ddlname;
	FormConfigStruct input;
	FormConfigStruct output;
	std::string function;

	static const config::DescriptionBase* description();
};

struct ConfigurationStruct
{
	std::vector<ScriptConfigStruct> script;		//< script definitions
	std::vector<DirectMapConfigStruct> directmap;	//< direct map definitions
	std::size_t input_bufsize;			//< size of input network message buffers in bytes (should only be configured for testing)
	std::size_t output_bufsize;			//< size of output network message buffers in bytes (should only be configured for testing)

	static const config::DescriptionBase* description();
};


class Configuration :public config::ConfigurationBase
{
public:
	Configuration();
	Configuration( const Configuration& o)	:config::ConfigurationBase(o),m_data(o.m_data),m_cmds(o.m_cmds),m_envs(o.m_envs),m_compilers(o.m_compilers){}

	bool parse( const config::ConfigurationTree& pt, const std::string& node, const module::ModulesDirectory* modules );

	///\brief return all currently available commands
	const std::vector< CountedReference<protocol::CommandBase> >& getCommands( const char* privileges=0) const;

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
	bool defineScript( const ScriptConfigStruct& sc);
	bool defineDirectMap( const DirectMapConfigStruct& dm);

	ConfigurationStruct m_data;
private:
	std::vector< CountedReference<protocol::CommandBase> > m_cmds;		//< factories for the command handlers
	std::vector< CountedReference<protocol::CommandEnvironment> > m_envs;	//< static environments of the command handlers
	std::vector< CountedReference<ddl::CompilerInterface> > m_compilers;	//< compilers for translating DDL definitions
};
}}//namespace
#endif


