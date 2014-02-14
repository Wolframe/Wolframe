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
//
// appConfig.hpp
//

#ifndef _APP_CONFIG_HPP_INCLUDED
#define _APP_CONFIG_HPP_INCLUDED

#include "config/configurationBase.hpp"
#include <cstddef>
#include <string>
#include <map>
#include <vector>

namespace _Wolframe {

// forward declarations for configuration elements
namespace net { struct Configuration;	}
namespace log { struct LoggerConfiguration;	}
struct HandlerConfiguration;


namespace config	{

struct ServiceConfiguration;
struct LoggerConfiguration;
struct CmdLineConfig;		// forward declaration for the command line structure

/// application configuration structure
struct ApplicationConfiguration	{
	std::string			configFile;
	// from command line
	bool				foreground;

	// daemon / service configuration
	ServiceConfiguration		*serviceCfg;
	// network server configuration
	net::Configuration		*serverCfg;
	// logger configuration
	log::LoggerConfiguration	*loggerCfg;
	// the thing that does the job
	HandlerConfiguration		*handlerCfg;

public:
	enum ConfigFileType	{
		CONFIG_INFO,
		CONFIG_XML,
		CONFIG_UNDEFINED
	};

	ApplicationConfiguration();
	~ApplicationConfiguration();

	static ConfigFileType fileType( const char *filename, ConfigFileType type );

	bool parseModules( const char *filename, ConfigFileType type );
	const std::list< std::string >& moduleList() const
						{ return m_modFiles; }
	const std::string& moduleFolder() const	{ return m_modFolder; }

	void addModules( const module::ModulesDirectory* modules )
						{ m_modDir = modules; }
	bool parse( const char *filename, ConfigFileType type );
	///\brief Finalize configuration for daemon
	void finalize( const CmdLineConfig& cmdLine );
	///\brief Finalize configuration for simple program (tests, wolfilter) running in foreground
	void finalize();

	bool check() const;
	bool test() const;
	void print( std::ostream& os ) const;

	static const char* chooseFile( const char *globalFile,
				       const char *userFile,
				       const char *localFile );
private:
	ConfigFileType				m_type;
	std::vector< ConfigurationBase* >	m_conf;
	std::map< std::string, std::size_t >	m_section;
	std::string				m_modFolder;
	std::list< std::string >		m_modFiles;

	const module::ModulesDirectory*		m_modDir;
public:
	bool addConfig( const std::string& nodeName, ConfigurationBase* conf );
};

} // namespace config
} // namespace _Wolframe

#endif // _APP_CONFIG_HPP_INCLUDED
