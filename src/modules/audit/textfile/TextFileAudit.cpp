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
// file audit implementation
//
#include <stdexcept>

#include "logger-v1.hpp"
#include "TextFileAudit.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "miscUtils.hpp"

namespace _Wolframe {
namespace AAAA {

/// Audit to file
bool TextFileAuditConfig::check() const
{
	if ( m_file.empty() )	{
		MOD_LOG_ERROR << logPrefix() << "Audit filename cannot be empty";
		return false;
	}
	return true;
}

void TextFileAuditConfig::print( std::ostream& os, size_t indent ) const
{
	std::string indStr( indent, ' ' );
	os << indStr << sectionName() << ": " << m_file << std::endl;
}

void TextFileAuditConfig::setCanonicalPathes( const std::string& refPath )
{
	using namespace boost::filesystem;

	if ( ! m_file.empty() )	{
		if ( ! path( m_file ).is_absolute() )
			m_file = resolvePath( absolute( m_file,
							path( refPath ).branch_path()).string());
		else
			m_file = resolvePath( m_file );
	}
}


TextFileAuditor::TextFileAuditor( const std::string& filename )
	: m_file( filename )
{
	MOD_LOG_NOTICE << "Text file auditor created with file '" << m_file << "'";
}

TextFileAuditor::~TextFileAuditor()
{
}


TextFileAuditContainer::TextFileAuditContainer( const TextFileAuditConfig& conf )
{
	m_audit = new TextFileAuditor( conf.m_file );
	MOD_LOG_NOTICE << "Text file auditor container created";
}

}} // namespace _Wolframe::AAAA
