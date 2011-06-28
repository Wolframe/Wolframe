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
// database audit
//

#include "AAAA/auditContainer.hpp"
#include "database/databaseReference.hpp"

#ifndef _DB_AUDIT_HPP_INCLUDED
#define _DB_AUDIT_HPP_INCLUDED

namespace _Wolframe {
namespace AAAA {

class DBauditConfig : public module::ModuleConfiguration< DBauditConfig, AuditConfiguration >
{
	friend class DBauditContainer;
	friend class config::ConfigurationParser;
public:
	DBauditConfig( const char* cfgName, const char* logParent, const char* logName )
		: ModuleConfiguration< DBauditConfig, AuditConfiguration >( cfgName, logParent, logName ),
		  m_dbConfig( "", logParent, "" )		{}

	const char* typeName() const				{ return "DatabaseAudit"; }

	/// methods
	bool check() const					{ return m_dbConfig.check(); }
	void print( std::ostream& os, size_t indent ) const	{
		std::string indStr( indent, ' ' );
		os << indStr << sectionName();
		m_dbConfig.print( os, 0 );
	}

	void setCanonicalPathes( const std::string& refPath )	{ m_dbConfig.setCanonicalPathes( refPath ); }
private:
	db::ReferenceConfig	m_dbConfig;
};


class DBauditContainer : public module::ModuleContainer< DBauditContainer, DBauditConfig,
		AuditContainer, AuditConfiguration >
{
public:
	DBauditContainer( const DBauditConfig& conf );
	~DBauditContainer();

	const char* typeName() const				{ return "DatabaseAudit"; }

	bool resolveDB( const db::DatabaseProvider& db );
private:
	std::string		m_dbLabel;
	const db::Database*	m_db;
};

}} // namespace _Wolframe::AAAA

#endif // _DB_AUDIT_HPP_INCLUDED
