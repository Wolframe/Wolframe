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
//
//

#include <stdexcept>
#include <boost/algorithm/string.hpp>
#include "logger.hpp"
#include "DBauthentication.hpp"

namespace _Wolframe {
namespace AAAA {

DBauthContainer::DBauthContainer( const DatabaseAuthConfig& conf )
{
	assert ( boost::algorithm::iequals( conf.m_dbConfig.typeName(), "DB reference" ));

	m_db = NULL;
	m_dbLabel = conf.m_dbConfig.dbName();
	if ( m_dbLabel.empty() )
		throw std::logic_error( "Empty database reference in DBauthContainer" );

	LOG_NOTICE << "Database authenticator with database reference '" << m_dbLabel << "'";
}

DBauthContainer::~DBauthContainer()
{
}


bool DBauthContainer::resolveDB( const db::DBprovider& db )
{
	if ( m_db == NULL && ! m_dbLabel.empty() )	{
		m_db = db.database( m_dbLabel );
		if ( m_db )	{
			LOG_NOTICE << "Database authenticator: database reference '" << m_dbLabel << "' resolved";
			return true;
		}
		else	{
			LOG_ERROR << "Database authenticator: database labeled '" << m_dbLabel << "' not found !";
			return false;
		}
	}
	return true;
}

}} // namespace _Wolframe::AAAA

