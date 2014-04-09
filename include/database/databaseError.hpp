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
///\brief Error class for databases
///\file database/databaseError.hpp
//
#ifndef _DATABASE_DATABASE_ERROR_HPP_INCLUDED
#define _DATABASE_DATABASE_ERROR_HPP_INCLUDED
#include "logger-v1.hpp"
#include "logger/logObject.hpp"
#include <string>
#include <cstdlib>
#include <stdexcept>

namespace _Wolframe {
namespace db {

class DatabaseError
{
public:
	DatabaseError()
		:errorcode(0),ip(0){}

	DatabaseError( const DatabaseError& o)
		:dbname(o.dbname)
		,errorclass(o.errorclass)
		,errorcode(o.errorcode)
		,errormsg(o.errormsg)
		,errordetail(o.errordetail)
		,errorhint(o.errorhint)
		,ip(o.ip)
		{}

	DatabaseError( const char* errorclass_,
			int errorcode_,
			const std::string& errormsg_,
			const std::string& detail_=std::string())
		:errorclass(errorclass_?errorclass_:"")
		,errorcode(errorcode_)
		,errormsg(errormsg_)
		,errordetail(detail_)
		,ip(0)
		{}

	std::string dbname;		//< error class identifier
	std::string errorclass;		//< error class identifier
	unsigned int errorcode;		//< error code
	std::string errormsg;		//< error message string
	std::string errordetail;	//< detailed error message string
	std::string errorhint;		//< error message hint specified in TDL for the user
	std::size_t ip;			//< instruction pointer in vm program
};

}}
#endif
