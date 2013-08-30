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
// database audit implementation
//

#include <stdexcept>
#include <boost/algorithm/string.hpp>

#include "logger-v1.hpp"
#include "DBaudit.hpp"

namespace _Wolframe {
namespace AAAA {

//****  DBauditor  **********************************************************
DBauditor::DBauditor( const std::string& dbLabel )
	: m_dbLabel( dbLabel )
{
	m_db = NULL;

	if ( m_dbLabel.empty() )
		throw std::logic_error( "Empty database reference in DBauditContainer" );

	MOD_LOG_DEBUG << "Database auditor created with database reference '" << m_dbLabel << "'";
}

DBauditor::~DBauditor()
{
}

bool DBauditor::resolveDB( const db::DatabaseProvider& db )
{
	if ( m_db == NULL && ! m_dbLabel.empty() )	{
		m_db = db.database( m_dbLabel );
		if ( m_db )	{
			MOD_LOG_TRACE << "Database audit: database reference '" << m_dbLabel << "' resolved";
			return true;
		}
		else	{
			MOD_LOG_ERROR << "Database audit: database labeled '" << m_dbLabel << "' not found !";
			return false;
		}
	}
	return true;
}

bool DBauditor::audit( const Information& auditObject )
{
	switch( auditObject.type())	{
		case Information::CONNECTION:
		case Information::LOGIN:
		case Information::LOGOUT:
		case Information::TRANSACTION:
			return true;
	}
	return false;
}

}} // namespace _Wolframe::AAAA
