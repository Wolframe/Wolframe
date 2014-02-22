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

#include "logger-v1.hpp"
#include "system/globalRngGen.hpp"

#include <boost/thread/mutex.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/locks.hpp>

namespace _Wolframe	{

#ifdef _WIN32
	#define	DEFAULT_RANDOM_DEVICE	(MS_DEF_PROV)
#else
	#define	DEFAULT_RANDOM_DEVICE	"/dev/urandom";
#endif

// We don't want an implicit initialized random device
#define	NO_IMPLICIT_RANDOM_DEVICE	1

static boost::scoped_ptr< RandomGenerator >	m_t;
static boost::mutex				m_mutex;
static bool					m_initialized = false;

RandomGenerator& RandomGenerator::instance()
{
	if ( !m_initialized )	{
		boost::lock_guard< boost::mutex > lock( m_mutex );
		if ( !m_initialized )	{
#ifdef NO_IMPLICIT_RANDOM_DEVICE
			throw std::logic_error( "Uninitialized random generator instance called." );
#else
			m_t.reset( new RandomGenerator() );
			m_initialized = true;
#endif
		}
	}
	return *m_t;
}

RandomGenerator& RandomGenerator::instance( const std::string &rndDev )
{
	if ( !m_initialized )	{
		boost::lock_guard< boost::mutex > lock( m_mutex );
		if ( !m_initialized )	{
			m_t.reset( new RandomGenerator( rndDev ));
			m_initialized = true;
		}
	}
	return *m_t;
}

RandomGenerator::RandomGenerator( const std::string& rndDev )
{
	if ( rndDev.empty() )	{
		LOG_INFO << "Empty random generator device. Using default device.";
		m_device = DEFAULT_RANDOM_DEVICE;
	}
	else
		m_device = rndDev;
	LOG_DEBUG << "Random generator initialized. Using device '" << m_device << "'";
}

RandomGenerator::RandomGenerator()
{
	m_device = DEFAULT_RANDOM_DEVICE;
	LOG_DEBUG << "Random generator initialized with the default device (" << m_device << ")";
}

RandomGenerator::~RandomGenerator()
{}

void RandomGenerator::device( const std::string &rndDev )
{
	m_device = rndDev;
}

const std::string& RandomGenerator::device() const
{
	return m_device;
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

