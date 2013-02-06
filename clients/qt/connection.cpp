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

#include <QObject>

#include <string>
#include <sstream>

#include "connection.hpp"

ConnectionParameters::ConnectionParameters()
{
//	name and host should be empty
	port = 0;
	SSL = false;
	clientCertificate = false;
//	SSLcertificate, SSLkey and SSLCAbundle should be empty
	SSLverify = false;
	timeout = 0;
}

bool ConnectionParameters::isEmpty() const
{
	if ( name.isEmpty() )
		return true;
	else
		return false;
}

QString ConnectionParameters::toString() const
{
	if ( !name.isEmpty() )	{
		QString str;
		str = QString( "Host: %1\nPort: %2\n" ).arg( host ).arg( port );
#ifdef WITH_SSL
		if ( SSL )	{
			str += "SSL on";
			if ( clientCertificate )
				str += "\n    - send client certificate";
			if ( SSLverify )
				str += "\n    - verify server certificate";
		}
		else
			str += "SSL off";
#else
		str += "<no SSL compiled in>";
#endif
		return str;
	}
	else
		return QString();
}

bool ConnectionParameters::check( QString& error ) const
{
	bool ret = true;
	if ( name.isEmpty() )	{
		error = QObject::tr( "Connection name cannot be empty" );
		ret = false;
	}
	if ( host.isEmpty() )	{
		if ( ! error.isEmpty() )
			error += "\n";
		error += QObject::tr( "Server hostname / IP address must be defined" );
		ret = false;
	}
	if ( !port )	{
		if ( ! error.isEmpty() )
			error += "\n";
		error += QObject::tr( "Server port must be defined (> 0)" );
		ret = false;
	}
#ifdef WITH_SSL
	if ( SSL )	{
		if ( clientCertificate )	{
			if ( SSLcertificate.isEmpty() )	{
				if ( ! error.isEmpty() )
					error += "\n";
				error += QObject::tr( "Client SSL certificate file must be defined" );
				ret = false;
			}
			if ( SSLkey.isEmpty() )	{
				if ( ! error.isEmpty() )
					error += "\n";
				error += QObject::tr( "Client SSL key file must be defined" );
				ret = false;
			}
		}
		if ( SSLverify )	{
			if ( SSLCAbundle.isEmpty() )	{
				if ( ! error.isEmpty() )
					error += "\n";
				error += QObject::tr( "SSL CA chain must be defined in order to verify the server SSL certificate" );
				ret = false;
			}
		}
	}
#endif
	return ret;
}

bool ConnectionParameters::test( QString& error ) const
{
	error = QObject::tr( "Connection testing is not yet implemented" );
	return false;
}


