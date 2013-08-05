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
//
//

#include <string>
#include <ostream>

#include "logger/logger-v1.hpp"
#include "SaslAuth.hpp"
#include "utils/fileUtils.hpp"

#include <boost/filesystem.hpp>

using namespace _Wolframe::utils;

namespace _Wolframe {
namespace AAAA {

/// Text file authentication
bool SaslAuthConfig::check() const
{
	if ( m_service.empty() )	{
		MOD_LOG_ERROR << logPrefix() << "a SASL service must be configured";
		return false;
	}
	return true;
}

void SaslAuthConfig::print( std::ostream& os, size_t indent ) const
{
	std::string indStr( indent, ' ' );
	os << indStr << sectionName() << std::endl;
	os << indStr << "   Identifier: " << m_identifier << std::endl;
	os << indStr << "   SASL service: " << m_service << std::endl;
	if( !m_confPath.empty( ) ) {
		os << indStr << "   SASL local configuration: " << m_confPath << std::endl;
	}
}

void SaslAuthConfig::setCanonicalPathes( const std::string& refPath )
{
	using namespace boost::filesystem;

	if ( ! m_confPath.empty() )	{
		std::string oldPath = m_confPath;
		m_confPath = utils::getCanonicalPath( m_confPath, refPath );
		if ( oldPath != m_confPath )	{
/*MBa ?!?*/		LOG_NOTICE << logPrefix() << "Using absolute for SASL configuration filename '"
				   << m_confPath << "' instead of '" << oldPath << "'";
		}
	}
}

SaslAuthenticator::SaslAuthenticator( const std::string& Identifier,
		   const std::string& service, const std::string& confPath )
	: AuthenticationUnit( Identifier ),
	  m_service( service ), m_confPath( confPath )
{
	MOD_LOG_DEBUG << "SASL authenticator created for service '" << m_service << "'";
	if( !m_confPath.empty( ) ) {
		MOD_LOG_DEBUG << "     and with SASL configuration '" << confPath << "'";
	}
}

SaslAuthenticator::~SaslAuthenticator()
{
}

}} // namespace _Wolframe::AAAA

