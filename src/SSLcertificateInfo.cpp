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
// SSLcertificateInfo.cpp
//

#include "system/SSLcertificateInfo.hpp"

#include <string>
#include <cstring>
#include <openssl/x509.h>


static time_t timeFromASN1( const ASN1_TIME *aTime )
{
	time_t lResult = 0;

	char lBuffer[24];
	char * pBuffer = lBuffer;

	size_t lTimeLength = aTime->length;
	char * pString = (char *)aTime->data;

	if ( aTime->type == V_ASN1_UTCTIME )	{
		if ((lTimeLength < 11) || (lTimeLength > 17))
			return 0;
		memcpy(pBuffer, pString, 10);
		pBuffer += 10;
		pString += 10;
	}
	else	{
		if ( lTimeLength < 13 )
			return 0;
		memcpy(pBuffer, pString, 12);
		pBuffer += 12;
		pString += 12;
	}

	if (( *pString == 'Z' ) || ( *pString == '-' ) || ( *pString == '+' ))	{
		*(pBuffer++) = '0';
		*(pBuffer++) = '0';
	}
	else	{
		*(pBuffer++) = *(pString++);
		*(pBuffer++) = *(pString++);
		// Skip any fractional seconds...
		if ( *pString == '.' )	{
			pString++;
			while (( *pString >= '0' ) && ( *pString <= '9' ))
				pString++;
		}
	}

	*(pBuffer++) = 'Z';
	*(pBuffer++) = '\0';

	time_t lSecondsFromUCT;
	if ( *pString == 'Z' )
		lSecondsFromUCT = 0;
	else	{
		if (( *pString != '+' ) && ( pString[5] != '-' ))
			return 0;

		lSecondsFromUCT = (( pString[1] - '0' ) * 10 + ( pString[2] - '0' )) * 60;
		lSecondsFromUCT += ( pString[3] - '0' ) * 10 + ( pString[4] - '0' );
		if (*pString == '-')
			lSecondsFromUCT = -lSecondsFromUCT;
	}

	tm lTime;
	lTime.tm_sec  = (( lBuffer[10] - '0') * 10 ) + ( lBuffer[11] - '0' );
	lTime.tm_min  = (( lBuffer[8] - '0') * 10 ) + ( lBuffer[9] - '0' );
	lTime.tm_hour = (( lBuffer[6] - '0') * 10 ) + ( lBuffer[7] - '0' );
	lTime.tm_mday = (( lBuffer[4] - '0') * 10 ) + ( lBuffer[5] - '0' );
	lTime.tm_mon  = ((( lBuffer[2] - '0') * 10 ) + ( lBuffer[3] - '0' )) - 1;
	lTime.tm_year = (( lBuffer[0] - '0') * 10 ) + ( lBuffer[1] - '0' );
	if ( lTime.tm_year < 50 )
		lTime.tm_year += 100; // RFC 2459
	lTime.tm_wday = 0;
	lTime.tm_yday = 0;
	lTime.tm_isdst = 0;  // No DST adjustment requested

	 lResult = mktime(&lTime);
	 if ((time_t)-1 != lResult )	{
		if ( 0 != lTime.tm_isdst )
			lResult -= 3600;  // mktime may adjust for DST  (OS dependent)
		lResult += lSecondsFromUCT;
	 }
	 else
		lResult = 0;

	return lResult;
}

namespace _Wolframe {
namespace net {

static const std::size_t BUFFER_SIZE = 2048;
static const std::size_t BUFFER_SPACE = BUFFER_SIZE - 1;

SSLcertificateInfo::SSLcertificateInfo( X509* cert )
{
	char	buf[ BUFFER_SIZE + 1 ];

	m_serialNumber = ASN1_INTEGER_get( X509_get_serialNumber( cert ));

	memset( buf, 0, BUFFER_SIZE );
	X509_NAME_oneline( X509_get_issuer_name( cert ), buf, BUFFER_SPACE );
	m_issuer = std::string( buf );

	ASN1_TIME *at = X509_get_notBefore( cert );
	m_notBefore = timeFromASN1( at );

	at = X509_get_notAfter( cert );
	m_notAfter = timeFromASN1( at );

	memset( buf, 0, BUFFER_SIZE );
	X509_NAME_oneline( X509_get_subject_name( cert ), buf, BUFFER_SPACE );
	m_subject = std::string( buf );

	// this one should be maybe be interpreted differently
	memset( buf, 0, BUFFER_SIZE );
	if ( X509_NAME_get_text_by_NID( X509_get_subject_name( cert ),
						      NID_commonName, buf, 2047 ) != -1 )
		m_commonName = std::string( buf );
}

}} // namespace _Wolframe::net
