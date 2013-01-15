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
// Audit provider implementation
//

#include <stdexcept>

#include "AAAAproviderImpl.hpp"
#include "moduleDirectory.hpp"
#include "logger-v1.hpp"
#include "boost/algorithm/string.hpp"

namespace _Wolframe {
namespace AAAA {

AuditProvider::AuditProvider( const std::list< config::NamedConfiguration* >& confs,
			const module::ModulesDirectory* modules )
{
	for ( std::list<config::NamedConfiguration*>::const_iterator it = confs.begin();
								it != confs.end(); it++ )	{
		module::ConfiguredBuilder* builder = modules->getBuilder((*it)->className());
		if ( builder )	{
			ConfiguredObjectConstructor< AuditUnit >* audit =
					dynamic_cast< ConfiguredObjectConstructor< AuditUnit >* >( builder->constructor());
			if ( audit == NULL )	{
				LOG_ALERT << "AuditProvider: '" << builder->objectClassName()
					  << "' is not an Audit Unit builder";
				throw std::logic_error( "object is not an AuditUnit builder" );
			}
			m_auditors.push_back( audit->object( **it ) );
			LOG_TRACE << "'" << audit->objectClassName() << "' audit unit registered";
		}
		else	{
			LOG_ALERT << "AuditProvider: unknown audit type '" << (*it)->className() << "'";
			throw std::domain_error( "Unknown auditing mechanism type in AAAAprovider constructor. See log" );
		}
	}
}

AuditProvider::~AuditProvider()
{
	for ( std::list< AuditUnit* >::iterator it = m_auditors.begin();
						it != m_auditors.end(); it++ )
		delete *it;
}

bool AuditProvider::resolveDB( const db::DatabaseProvider& db )
{
	for ( std::list< AuditUnit* >::iterator it = m_auditors.begin();
						it != m_auditors.end(); it++ )
		if ( ! (*it)->resolveDB( db ) )
			return false;
	return true;
}


//********************************************************************************
StandardAudit::StandardAudit( const std::list< AuditUnit* >& units, bool mandatory )
	: m_auditUnits( units ), m_mandatory( mandatory )
{
}

StandardAudit::~StandardAudit()
{
}

void StandardAudit::close()
{
}

// audit request
bool StandardAudit::audit( const Information& auditObject )
{
	bool hasSucceded = false;

	for ( std::list< AuditUnit* >::const_iterator au = m_auditUnits.begin();
						au != m_auditUnits.end(); au++ )	{
		bool res = (*au)->audit( auditObject );
		if ( res )
			hasSucceded = true;
		else	{
			if ((*au)->required() )
				return false;
		}
	}

	if ( m_mandatory && ! hasSucceded )
		return false;
	return true;
}

}} // namespace _Wolframe::AAAA
