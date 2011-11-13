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
// database audit implementation
//

#include <stdexcept>
#include <boost/algorithm/string.hpp>

#include "logger-v1.hpp"
#include "DBaudit.hpp"

namespace _Wolframe {
namespace AAAA {

DBauditContainer::DBauditContainer( const DBauditConfig& conf )
{
	m_db = NULL;
	m_dbLabel = conf.m_dbConfig.label();

	if ( m_dbLabel.empty() )
		throw std::logic_error( "Empty database reference in DBauditContainer" );

	MOD_LOG_NOTICE << "Database auditor created with database reference '" << m_dbLabel << "'";
}

DBauditContainer::~DBauditContainer()
{
}

bool DBauditContainer::resolveDB( const db::DatabaseProvider& db )
{
	if ( m_db == NULL && ! m_dbLabel.empty() )	{
		// Aba: Windows, don't know how to fix this:
		// DBaudit.obj : error LNK2019: unresolved external symbol
		// "public: class _Wolframe::db::Database const 
		// * __thiscall _Wolframe::db::DatabaseProvider::database(class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const &)const " (?database@DatabaseProvider@db@_Wolframe@@QBEPBVDatabase@23@ABV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z) referenced in function "public: bool __thiscall _Wolframe::AAAA::DBauditContainer::resolveDB(class _Wolframe::db::DatabaseProvider const &)" (?resolveDB@DBauditContainer@AAAA@_Wolframe@@QAE_NABVDatabaseProvider@db@3@@Z)
		m_db = db.database( m_dbLabel );
		if ( m_db )	{
			MOD_LOG_NOTICE << "Database audit: database reference '" << m_dbLabel << "' resolved";
			return true;
		}
		else	{
			MOD_LOG_ERROR << "Database audit: database labeled '" << m_dbLabel << "' not found !";
			return false;
		}
	}
	return true;
}

}} // namespace _Wolframe::AAAA
