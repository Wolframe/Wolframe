/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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

#include "AAAA/audit.hpp"

#ifndef _DB_AUDIT_HPP_INCLUDED
#define _DB_AUDIT_HPP_INCLUDED

namespace _Wolframe {
namespace AAAA {

class DBauditConfig : public config::NamedConfiguration
{
	friend class DBauditConstructor;
public:
	DBauditConfig( const char* cfgName, const char* logParent, const char* logName )
		: config::NamedConfiguration( cfgName, logParent, logName )
	{ m_required = true; }

	const char* objectName() const		{ return "DatabaseAudit"; }

	/// methods
	bool parse( const config::ConfigurationTree& pt, const std::string& node,
		    const module::ModulesDirectory* modules );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
private:
	bool			m_required;
	std::string		m_dbConfig;
};


class DBauditor : public AuditUnit
{
public:
	DBauditor( const std::string& dbLabel );
	~DBauditor();
	const char* typeName() const		{ return "DatabaseAudit"; }

	bool resolveDB( const db::DatabaseProvider& db );

	bool required()				{ return m_required; }

	bool audit( const Information& auditObject );
private:
	bool			m_required;
	std::string		m_dbLabel;
	const db::Database*	m_db;
};

}} // namespace _Wolframe::AAAA

#endif // _DB_AUDIT_HPP_INCLUDED
