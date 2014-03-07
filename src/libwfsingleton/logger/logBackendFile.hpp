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
///
/// \file logBackendFile.hpp
/// \brief header file for the logger which logs to a simple logfile
///

#ifndef _LOG_BACKEND_FILE_HPP_INCLUDED
#define _LOG_BACKEND_FILE_HPP_INCLUDED

#include "logger/logLevel.hpp"
#include "logger/logBackend.hpp"

#include <fstream>

namespace _Wolframe {
namespace log {

class LogfileBackend
{
public:
	LogfileBackend( );

	~LogfileBackend( );

	void setLevel( const LogLevel::Level level_ );

	void setFilename( const std::string filename );

	void reopen( );

	void log( const LogLevel::Level level, const std::string& msg );

	LogLevel::Level level() const	{ return m_logLevel; }
private:
	LogLevel::Level	m_logLevel;
	std::ofstream	m_logFile;
	std::string	m_filename;
	bool		m_isOpen;
};

}} // namespace _Wolframe::log

#endif // _LOG_BACKEND_FILE_HPP_INCLUDED
