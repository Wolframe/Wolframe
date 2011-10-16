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
// database.cpp
//

#include "DBproviderImpl.hpp"

#include "logger.hpp"

#include <boost/algorithm/string.hpp>

/****  Impersonating the module loader  ******************************************************/
#include "PostgreSQL.hpp"
#include "SQLite.hpp"

using namespace _Wolframe;

static const size_t noDBmodules = 2;

static module::ContainerDescription< Container< db::DatabaseUnit > >
dbModules[ noDBmodules ] = {
	module::ContainerDescription< Container< db::DatabaseUnit > >( "PostgreSQL", &db::PostgreSQLcontainer::create ),
	module::ContainerDescription< Container< db::DatabaseUnit > >( "SQLite", &db::SQLiteContainer::create )
};
/****  End impersonating the module loader  **************************************************/

namespace _Wolframe	{
namespace db	{

/// DatabaseProvider PIMPL
DatabaseProvider::DatabaseProvider( const DBproviderConfig* conf ) :
	m_impl( new DatabaseProvider_Impl( conf ))	{}

DatabaseProvider::~DatabaseProvider()
{
	delete m_impl;
}

const Database* DatabaseProvider::database( const std::string& ID ) const
{
	return m_impl->database( ID );
}


/// DatabaseProvider PIMPL implementation

DatabaseProvider::DatabaseProvider_Impl::DatabaseProvider_Impl( const DBproviderConfig* conf )
{
	for ( std::list< config::ObjectConfiguration* >::const_iterator it = conf->m_dbConfig.begin();
							it != conf->m_dbConfig.end(); it++ )	{
		const char* dbType = (*it)->objectName();
		size_t i;
		for ( i = 0; i < noDBmodules; i++ )	{
			if ( boost::algorithm::iequals( dbModules[i].name, dbType ))	{
				Container< db::DatabaseUnit >* container = dbModules[i].createFunc( **it );
				m_db.push_back( container );
				break;
			}
		}
		if ( i >= noDBmodules )	{
			LOG_ALERT << "DatabaseProvider: unknown database type '" << dbType << "'";
			throw std::domain_error( "Unknown database type in DBprovider constructor. See log" );
		}
	}
}

DatabaseProvider::DatabaseProvider_Impl::~DatabaseProvider_Impl()
{
	for ( std::list< Container< db::DatabaseUnit >* >::const_iterator it = m_db.begin();
							it != m_db.end(); it++ )
		delete *it;
}


const Database* DatabaseProvider::DatabaseProvider_Impl::database( const std::string& id ) const
{
	for ( std::list< Container< db::DatabaseUnit >* >::const_iterator it = m_db.begin();
							it != m_db.end(); it++ )	{
		if ( (*it)->object().database().ID() == id )
			return &(*it)->object().database();
	}
	return NULL;
}

}} // namespace _Wolframe::db
