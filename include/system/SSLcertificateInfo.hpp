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

#ifndef _SSL_CERTIFICATE_INFO_HPP_INCLUDED
#define _SSL_CERTIFICATE_INFO_HPP_INCLUDED

#include <string>
#include <ctime>

#ifdef WITH_SSL

#include <openssl/x509.h>

namespace _Wolframe {
	namespace net {

		class SSLcertificateInfo
		{
		public:
			SSLcertificateInfo( X509* cert );

			long serialNumber() const		{ return m_serialNumber; }
			const std::string& issuer() const	{ return m_issuer; }
			time_t notBefore() const		{ return m_notBefore; }
			time_t notAfter() const			{ return m_notAfter; }
			const std::string& subject() const	{ return m_subject; }
			const std::string& commonName() const	{ return m_commonName; }
		private:
			long m_serialNumber;
			std::string m_issuer;
			time_t m_notBefore;
			time_t m_notAfter;
			std::string m_subject;
			std::string m_commonName;
		};

	} // namespace net
} // namespace _Wolframe

#endif // defined( WITH_SSL )

#endif // _SSL_CERTIFICATE_INFO_HPP_INCLUDED
