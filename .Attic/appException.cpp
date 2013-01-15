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
//
// appException.cpp
//

#include <string>
#include <exception>
#include <cstdarg>
#include <cstdio>

#include "ErrorCode.hpp"
#include "appException.hpp"


static const size_t WHAT_BUFFER_SIZE = 255;


namespace _Wolframe	{

appException::appException( ErrorCode::Error error, ErrorSeverity::Severity severity, const char* format, ... )
{
	char	buf[WHAT_BUFFER_SIZE];
	va_list	ap;

	error_ = error;
	severity_ = severity;

	va_start( ap, format );
	vsnprintf( buf, WHAT_BUFFER_SIZE, format, ap );
	va_end( ap );

	what_ = std::string( buf );
}


systemException::systemException( ErrorCode::Error error, ErrorSeverity::Severity severity,
				ErrorModule::Module module, const char* msg )
{
	error_ = error;
	severity_ = severity;
	module_ = module;
	msg_ = msg;
}

} // namespace _Wolframe

