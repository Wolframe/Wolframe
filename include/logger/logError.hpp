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
/// \file logError.hpp
/// \brief Error markers for logger output stream
///

#ifndef _LOG_ERROR_HPP_INCLUDED
#define _LOG_ERROR_HPP_INCLUDED

namespace _Wolframe {
namespace log {

class LogError {
public:
	/// Internal enum representing possible error conversion
	/// methods
	enum Error {
		LOGERROR_UNDEF,		///< undefined
		LOGERROR_STRERROR,	///< Posix strerror_r
		LOGERROR_WINERROR	///< Windows GetLastError/FormatMessage
	};

private:
	enum Error _error;

public:
	bool operator==( const LogError& e ) const {
		return _error == e._error;
	}

	enum Error error( ) const {
		return _error;
	}

	LogError( const enum Error& e = LOGERROR_UNDEF ) : _error( e ) { }

	/// output stream marker for logging the strerror of the last
	/// POSIX system call in human readable format
	static const LogError LogStrerror;

	/// output stream marker for logging the Windows error of the last
	/// Windows API call in human readable format
	static const LogError LogWinerror;
};

}} // namespace _Wolframe::log

#endif // _LOG_ERROR_HPP_INCLUDED
