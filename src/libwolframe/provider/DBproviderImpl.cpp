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
// database.cpp
//

#include "DBproviderImpl.hpp"
#include "moduleDirectory.hpp"
#include "logger/logger-v1.hpp"
#include <boost/algorithm/string.hpp>

namespace _Wolframe	{
namespace db	{

/********  DatabaseProvider PIMPL  *****************************************/
DatabaseProvider::DatabaseProvider( const DBproviderConfig* conf,
				    const module::ModulesDirectory* modules ) :
	m_impl( new DatabaseProvider_Impl( conf, modules ))	{}

DatabaseProvider::~DatabaseProvider()
{
	delete m_impl;
}

Database* DatabaseProvider::database( const std::string& ID ) const
{
	return m_impl->database( ID );
}


/********  DatabaseProvider PIMPL implementation ***************************/
DatabaseProvider::DatabaseProvider_Impl::DatabaseProvider_Impl( const DBproviderConfig* conf,
								const module::ModulesDirectory* modules )
{
	for ( std::list< config::NamedConfiguration* >::const_iterator it = conf->m_config.begin();
									it != conf->m_config.end(); it++ )	{
		module::ConfiguredBuilder* builder = modules->getBuilder((*it)->className());
		if ( builder )	{
			ConfiguredObjectConstructor< db::DatabaseUnit >* db =
					dynamic_cast< ConfiguredObjectConstructor< db::DatabaseUnit >* >( builder->constructor());
			if ( db == NULL )	{
				LOG_ALERT << "DatabaseProvider: '" << builder->objectClassName()
					  << "' is not a Database Unit builder";
				throw std::logic_error( "Object is not an DatabaseUnit builder" );
			}
			m_db.push_back( db->object(**it ) );
			LOG_TRACE << "'" << db->objectClassName() << "' database unit registered";
		}
		else	{
			LOG_ALERT << "DatabaseProvider: unknown database module '" << (*it)->className() << "'";
			throw std::domain_error( "Unknown database type in DBprovider constructor. See log" );
		}
	}
}

DatabaseProvider::DatabaseProvider_Impl::~DatabaseProvider_Impl()
{
	for ( std::list< db::DatabaseUnit* >::iterator it = m_db.begin();
							it != m_db.end(); it++ )
		delete *it;
}

Database* DatabaseProvider::DatabaseProvider_Impl::database( const std::string& id ) const
{
	for ( std::list<db::DatabaseUnit* >::const_iterator it = m_db.begin();
							it != m_db.end(); it++ )	{
		if ( (*it)->ID() == id )
			return (*it)->database();
	}
	return NULL;
}

}} // namespace _Wolframe::db
