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
// auditing objects
//

#ifndef _AUDITOR_HPP_INCLUDED
#define _AUDITOR_HPP_INCLUDED

#include "configurationBase.hpp"
#include "database.hpp"

#include <string>
#include <list>

namespace _Wolframe {
namespace AAAA {


enum AuditType	{
	AUDIT_FILE,		///< Standard text file auditing (list of entries)
	AUDIT_DATABASE,		///< Database audit entries
	AUDIT_UNKNOWN		///< Unknown method (i.e. none)
};

class AuditConfigBase : public config::ConfigurationBase
{
private:
	const AuditType	m_type;
public:
	/// constructor
	AuditConfigBase( const AuditType Type,
			 const char* name, const char* logParent, const char* logName )
		: config::ConfigurationBase( name, logParent, logName ),
		  m_type( Type )
	{}

	AuditType type() const			{ return m_type; }
};


class FileAuditConfig : public AuditConfigBase
{
	friend class FileAuditor;
public:
	FileAuditConfig( const char* cfgName, const char* logParent, const char* logName )
		: AuditConfigBase( AUDIT_FILE, cfgName, logParent, logName )
	{}
	// ~FileAuditConfig();

	/// methods
	bool parse( const boost::property_tree::ptree& pt, const std::string& node );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );

private:
	std::string	m_file;
};


class DatabaseAuditConfig : public AuditConfigBase
{
	friend class DatabaseAuditor;
public:
	DatabaseAuditConfig( const char* cfgName, const char* logParent, const char* logName )
		: AuditConfigBase( AUDIT_DATABASE, cfgName, logParent, logName ),
		  m_dbConfig( "", logParent, "" )	{}

	/// methods
	bool parse( const boost::property_tree::ptree& pt, const std::string& node )
								{ return m_dbConfig.parse( pt, node ); }
	bool check() const					{ return m_dbConfig.check(); }
	void print( std::ostream& os, size_t indent ) const	{
		std::string indStr( indent, ' ' );
		os << indStr << sectionName() << ": " << std::endl;
		m_dbConfig.print( os, indent + 3 );
	}

	void setCanonicalPathes( const std::string& refPath )	{ m_dbConfig.setCanonicalPathes( refPath ); }

private:
	db::SingleDBConfiguration	m_dbConfig;
};


class AuditConfiguration : public config::ConfigurationBase
{
	friend class AAAAprovider;
public:
	/// constructor
	AuditConfiguration( const char* cfgName, const char* logParent, const char* logName )
		: config::ConfigurationBase( cfgName, logParent, logName )	{}
	~AuditConfiguration();

	/// methods
	bool parse( const boost::property_tree::ptree& pt, const std::string& node );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );

	// bool test() const;	// Not implemented yet, inherited from base
private:
	std::list<AuditConfigBase*>	m_config;
};


class Auditor
{
public:
	virtual bool resolveDB( db::DBprovider& /*db*/ )	{ return true; }
};


class FileAuditor : public Auditor
{
public:
	FileAuditor( FileAuditConfig& config );
private:
	std::string	m_file;
};


class DatabaseAuditor : public Auditor
{
public:
	DatabaseAuditor( DatabaseAuditConfig& config );
	bool resolveDB( db::DBprovider& db );
private:
	std::string	m_dbLabel;
	db::Database*	m_db;
};

}} // namespace _Wolframe::AAAA

#endif // _AUDITOR_HPP_INCLUDED
