/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
/// \file logLevel.hpp
/// \brief Defines allowed log levels of the logger
///

#ifndef _LOG_LEVEL_HPP_INCLUDED
#define _LOG_LEVEL_HPP_INCLUDED

#include <string>
#include <iosfwd>

namespace _Wolframe	{
namespace log	{

class LogLevel
{
public:
	enum Level	{
		LOGLEVEL_DATA,		///< log everything, including data
		LOGLEVEL_TRACE,		///< trace functions calls
		LOGLEVEL_DEBUG,		///< log operations
		LOGLEVEL_INFO,		///< information message
		LOGLEVEL_NOTICE,	///< normal but significant condition
		LOGLEVEL_WARNING,	///< warning condition
		LOGLEVEL_ERROR,		///< normal error condition
		LOGLEVEL_SEVERE,	///< severe error condition
		LOGLEVEL_CRITICAL,	///< critical error condition
		LOGLEVEL_ALERT,		///< action must be taken immediately
		LOGLEVEL_FATAL,		///< system is unusable
		LOGLEVEL_UNDEFINED	///< log this only under special conditions
	};

	/// convert string to a loglevel
	static Level strToLogLevel( const std::string& str );
};


/// output loglevel to an output stream
template< typename CharT, typename TraitsT >
inline std::basic_ostream< CharT, TraitsT > &operator<< ( std::basic_ostream< CharT, TraitsT >& s,
							  LogLevel::Level l )
{
	static const CharT *const str[] = {
		"DATA", "TRACE", "DEBUG", "INFO", "NOTICE", "WARNING",
		"ERROR", "SEVERE", "CRITICAL", "ALERT", "FATAL" };
	if( static_cast< size_t >( l ) < ( sizeof( str ) / sizeof( *str ) ) )	{
		s << str[l];
	}
	else	{
		s << "Unknown enum used '" << static_cast< int >( l ) << "' as log level";
	}
	return s;
}

}} // namespace _Wolframe::log

#endif // _LOG_LEVEL_HPP_INCLUDED
