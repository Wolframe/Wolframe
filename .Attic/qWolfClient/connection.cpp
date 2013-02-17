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
		if ( SSL )	{
			str += "SSL on";
			if ( clientCertificate )
				str += "\n    - send client certificate";
			if ( SSLverify )
				str += "\n    - verify server certificate";
		}
		else
			str += "SSL off";
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
	return ret;
}

bool ConnectionParameters::test( QString& error ) const
{
	error = QObject::tr( "Connection testing is not yet implemented" );
	return false;
}


