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
// wolframedCommandLine.hpp
//

#ifndef _WOLFRAMED_COMMANDLINE_HPP_INCLUDED
#define _WOLFRAMED_COMMANDLINE_HPP_INCLUDED

#include "logger/logLevel.hpp"
#include "appConfig.hpp"

#include <string>
#include <boost/program_options.hpp>

namespace _Wolframe	{
namespace config	{

struct CmdLineConfig	{
	enum Command_t	{
		PRINT_HELP,
		PRINT_VERSION,
		CHECK_CONFIG,
		TEST_CONFIG,
		PRINT_CONFIG,
#if defined(_WIN32)
		INSTALL_SERVICE,
		REMOVE_SERVICE,
		RUN_SERVICE,
#endif
		DEFAULT
	};

	Command_t	command;
#if !defined(_WIN32)
	bool		foreground;
	std::string	user;
	std::string	group;
	std::string	pidFile;
#endif
	log::LogLevel::Level				debugLevel;
	std::string					cfgFile;
	ApplicationConfiguration::ConfigFileType	cfgType;
	bool						useLogConfig;
private:
	std::string					errMsg_;
	boost::program_options::options_description	options_;

public:
	CmdLineConfig();
	bool parse( int argc, char* argv[] );
	std::string errMsg( void )		{ return errMsg_; }
	void usage( std::ostream& os ) const	{ options_.print( os ); }
};

}} // namespace _Wolframe::config

#endif // _COMMANDLINE_HPP_INCLUDED
