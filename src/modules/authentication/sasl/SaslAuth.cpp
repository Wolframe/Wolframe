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
//
//

#include <string>
#include <ostream>

#include "logger-v1.hpp"
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
		LOG_ERROR << logPrefix() << "a SASL service must be configured";
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

//*********   SASL Authentication Unit   ************************************

SaslAuthUnit::SaslAuthUnit( const std::string& Identifier,
		   const std::string& service, const std::string& confPath )
	: AuthenticationUnit( Identifier ),
	  m_service( service ), m_confPath( confPath )
{
	LOG_DEBUG << "SASL authentication unit created for service '" << m_service << "'";
	if( !m_confPath.empty( ) ) {
		LOG_DEBUG << "     and with SASL configuration '" << confPath << "'";
	}
}

SaslAuthUnit::~SaslAuthUnit()
{
}

const char** SaslAuthUnit::mechs() const
{
	static const char* m[] = { "WOLFRAME-SASL", NULL };
	return m;
}

AuthenticatorSlice* SaslAuthUnit::slice( const std::string& /*mech*/,
					const net::RemoteEndpoint& /*client*/ )
{
	return new SaslAuthSlice( *this );
}

User* SaslAuthUnit::authenticatePlain(	const std::string& /*username*/,
					const std::string& /*password*/ ) const
{
	return NULL;
}

SaslAuthSlice::SaslAuthSlice( const SaslAuthUnit& backend )
	: m_backend( backend )
{
}

SaslAuthSlice::~SaslAuthSlice()
{
}

void SaslAuthSlice::dispose()
{
	delete this;
}

/// The input message
void SaslAuthSlice::messageIn( const std::string& /*message*/ )
{
}

/// The output message
std::string SaslAuthSlice::messageOut()
{	
	return std::string( );
}

/// The current status of the authenticator slice
AuthenticatorSlice::Status SaslAuthSlice::status() const
{
	return SYSTEM_FAILURE;
}
	
/// The authenticated user or NULL if not authenticated
User* SaslAuthSlice::user()
{
	return NULL;
}

}} // namespace _Wolframe::AAAA

