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
/// \file audit.hpp
/// \brief top-level header file for AAAA audit
///

#ifndef _AUDIT_HPP_INCLUDED
#define _AUDIT_HPP_INCLUDED

#include "AAAA/AAAAinformation.hpp"
#include "database/DBprovider.hpp"

namespace _Wolframe {
namespace AAAA {

/// Virtual base (interface) for auditor classes
class Auditor {
public:
	virtual ~Auditor(){}

	/// \brief Close the auditor
	virtual void close(){}

	virtual bool audit( const Information& auditObject ) = 0;
};


/// Audit Unit
/// This is the base class for audit unit implementations
class AuditUnit
{
public:
	virtual ~AuditUnit()				{}

	virtual const char* className() const = 0;

	virtual bool resolveDB( const db::DatabaseProvider& /*db*/ )
							{ return true; }
	virtual bool required() = 0;

	virtual bool audit( const Information& auditObject ) = 0;
};

}} // namespace _Wolframe::AAAA

#endif // _AUDIT_HPP_INCLUDED
