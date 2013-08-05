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
// Global random number generator implementation
//

#include <stdexcept>
#include <cstring>

#ifndef _WIN32
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#else
#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#include <wincrypt.h>
#endif

#include "logger/logger-v1.hpp"
#include "globalRngGen.hpp"

namespace _Wolframe	{

#ifdef _WIN32
	#define	DEFAULT_RANDOM_DEVICE	(MS_DEF_PROV)
#else
	#define	DEFAULT_RANDOM_DEVICE	"/dev/urandom";
#endif

RandomGenerator::RandomGenerator( const std::string& rndDev )
{
	if ( rndDev.empty() )	{
		m_device = DEFAULT_RANDOM_DEVICE;
	}
	else
		m_device = rndDev;
	LOG_DEBUG << "Random generator initialized. Using device '" << m_device << "'";
}

RandomGenerator::RandomGenerator()
{
	throw std::logic_error( "Random generator default constructor called. This should not happen." );
}

unsigned RandomGenerator::random() const
{
	unsigned ret;
	generate((unsigned char*)(&ret), sizeof( ret ));
	return ret;
}

void RandomGenerator::generate( unsigned char* buffer, size_t bytes ) const
{
#ifndef _WIN32
	int hndl = open( m_device.c_str(), O_RDONLY );
	if ( hndl < 0 )	{
		std::string errMsg = "Error opening '" + m_device + "': ";
		throw std::runtime_error( errMsg );
	}

	int rndBytes = read( hndl, buffer, bytes );
	if ( rndBytes < 0 )	{
		std::string errMsg = "Error reading '" + m_device + "'";
		throw std::runtime_error( errMsg );
	}
	else if ( rndBytes < (int)bytes )	{
		std::string errMsg = "Not enough entropy in '" + m_device + "'";
		throw std::logic_error( errMsg );
	}

	close( hndl );
#else
	HCRYPTPROV provider = 0;

	if( !CryptAcquireContext( &provider, 0, m_device.c_str( ), PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT ) ) {
		throw std::runtime_error( "Error opening cyrpto context" );
	}

	if( !CryptGenRandom( provider, bytes, (BYTE *)buffer ) ) {
		CryptReleaseContext( provider, 0 );
		throw std::runtime_error( "Error generating random data" );
	}

	if( !CryptReleaseContext( provider, 0 ) ) {
		throw std::runtime_error( "Error closing cyrpto context" );
	}
#endif
}

} // namespace _Wolframe

