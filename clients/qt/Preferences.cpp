//
// Preferences.cpp
//

#include "Preferences.hpp"

#include <QSettings>

namespace _Wolframe {
	namespace QtClient {

QScopedPointer<Preferences> Preferences::m_instance;

Preferences::Preferences( QObject *_parent )
	: QObject( _parent )
{
	loadSettings( );
}

Preferences::~Preferences( )
{
}

void Preferences::loadSettings( )
{
	QSettings s( SETTINGS_DOMAIN, SETTINGS_APP );
	m_host = s.value( "wolframe/host", "andreasbaumann.dyndns.org" ).toString( );
	m_port = s.value( "wolframe/port", 7661 ).toString( ).toUShort( );
	m_secure = s.value( "wolframe/secure", false ).toBool( );
	m_clientCertFile = s.value( "wolframe/client-cert-file", "./certs/client.crt" ).toString( );
	m_clientKeyFile = s.value( "wolframe/client-key-file", "./private/client.key" ) .toString( );
	m_CACertFile = s.value( "wolframe/ca-cert-file", "./certs/CAclient.cert.pem" ).toString( );
}

void Preferences::storeSettings( )
{
	QSettings s( SETTINGS_DOMAIN, SETTINGS_APP );
	s.setValue( "wolframe/host", m_host );
	s.setValue( "wolframe/port", m_port );
	s.setValue( "wolframe/secure", m_secure );
	s.setValue( "wolframe/client-cert-file", m_clientCertFile );
	s.setValue( "wolframe/client-key-file", m_clientKeyFile );
	s.setValue( "wolframe/ca-cert-file", m_CACertFile );
}

Preferences *Preferences::instance( )
{
	if( !m_instance ) {
		m_instance.reset( new Preferences( ) );
	}
	
	return m_instance.data( );
}

} // namespace QtClient
} // namespace _Wolframe
