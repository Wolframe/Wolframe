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
// application exception base class
//

#ifndef _EXCEPTION_BASE_HPP_INCLUDED
#define _EXCEPTION_BASE_HPP_INCLUDED

#include <string>
#include <exception>

#include "system/errorCode.hpp"

namespace _Wolframe {

class appException : public std::exception
{
public:
	appException( ErrorCode::Error error, ErrorSeverity::Severity severity, const char* Format, ... );
	~appException() throw ()	{}

	ErrorCode::Error error()			{ return m_error; }
	ErrorSeverity::Severity severity( void )	{ return m_severity; }
	const char* what() const throw()		{ return m_what.c_str(); }
private:
	ErrorCode::Error	m_error;
	ErrorSeverity::Severity	m_severity;
	std::string		m_what;
};

template <typename C> inline std::basic_ostream<C>& operator<< ( std::basic_ostream<C>& o,
								 appException const& e )
{
	return o << e.what();
}


class systemException : public std::exception
{
public:
	systemException( ErrorCode::Error error, ErrorSeverity::Severity severity,
			 ErrorModule::Module module = ErrorModule::UNKNOWN, const char* msg = NULL );
	~systemException() throw ()	{}

	ErrorCode::Error error()			{ return error_; }
	ErrorSeverity::Severity severity()		{ return severity_; }
	ErrorModule::Module module()			{ return module_; }
	const char* msg()				{ return msg_; }
	const char* what() const throw ()		{ return msg_; }
private:
	ErrorCode::Error		error_;
	ErrorSeverity::Severity		severity_;
	ErrorModule::Module		module_;
	const char*			msg_;
};

template <typename C> inline std::basic_ostream<C>& operator<< ( std::basic_ostream<C>& o,
								 systemException const& e )
{
	return o << e.what();
}

} // namespace _Wolframe

#endif // _EXCEPTION_BASE_HPP_INCLUDED
