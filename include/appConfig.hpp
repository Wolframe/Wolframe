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
//
// appConfig.hpp
//

#ifndef _APP_CONFIG_HPP_INCLUDED
#define _APP_CONFIG_HPP_INCLUDED

#include "config/configurationBase.hpp"
#include "config/configurationParser.hpp"

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

typedef bool (*ParseFunc_t)( ConfigurationBase&, const boost::property_tree::ptree&, const std::string& );

/// application configuration structure
struct ApplicationConfiguration	{
	std::string				configFile;
	// from command line
	bool					foreground;

	// daemon / service configuration
	ServiceConfiguration			*serviceConf;
	// network server configuration
	_Wolframe::net::Configuration		*serverConf;
	// logger configuration
	_Wolframe::log::LoggerConfiguration	*loggerConf;

	_Wolframe::HandlerConfiguration		*handlerConf;

public:
	enum ConfigFileType	{
		CONFIG_INFO,
		CONFIG_XML,
		CONFIG_UNDEFINED
	};

	ApplicationConfiguration();
	~ApplicationConfiguration();

	bool parse( const char *filename, ConfigFileType type );
	void finalize( const CmdLineConfig& cmdLine );

	bool check() const;
	bool test() const;
	void print( std::ostream& os ) const;

	static const char* chooseFile( const char *globalFile,
				       const char *userFile,
				       const char *localFile );
private:
	ConfigFileType				m_type;
	bool					m_forced;
	std::vector< ConfigurationBase* >	m_conf;
	std::vector< ParseFunc_t >		m_parse;
	std::map< std::string, std::size_t >	m_section;

	bool addConfig( const std::string& nodeName, ConfigurationBase* conf, ParseFunc_t parseFunc );
};

} // namespace config
} // namespace _Wolframe

#endif // _APP_CONFIG_HPP_INCLUDED
