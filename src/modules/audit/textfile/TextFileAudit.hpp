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
//
// file audit
//

#ifndef _FILE_AUDIT_HPP_INCLUDED
#define _FILE_AUDIT_HPP_INCLUDED

#include <string>

#include "AAAA/audit.hpp"
#include "module/constructor.hpp"

namespace _Wolframe {
namespace AAAA {

static const char* FILE_AUDIT_CLASS_NAME = "FileAudit";

class TextFileAuditConfig : public config::NamedConfiguration
{
	friend class TextFileAuditConstructor;
public:
	TextFileAuditConfig( const char* cfgName, const char* logParent, const char* logName )
		: config::NamedConfiguration( cfgName, logParent, logName ) {}

	const char* className() const		{ return FILE_AUDIT_CLASS_NAME; }

	/// methods
	bool parse( const config::ConfigurationTree& pt, const std::string& node,
		    const module::ModulesDirectory* modules );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );
private:
	bool		m_required;
	std::string	m_file;
};


class TextFileAuditor : public AuditUnit
{
public:
	TextFileAuditor( const std::string& filename );
	~TextFileAuditor();
	const char* className() const		{ return FILE_AUDIT_CLASS_NAME; }

	bool required()				{ return m_required; }
	bool audit( const Information& auditObject );
private:
	bool		m_required;
	std::string	m_file;
};


class TextFileAuditConstructor : public ConfiguredObjectConstructor< AuditUnit >
{
public:
	virtual ObjectConstructorBase::ObjectType objectType() const
						{ return AUDIT_OBJECT; }
	const char* objectClassName() const	{ return FILE_AUDIT_CLASS_NAME; }
	TextFileAuditor* object( const config::NamedConfiguration& conf );
};

}} // namespace _Wolframe::AAAA

#endif // _FILE_AUDIT_HPP_INCLUDED
