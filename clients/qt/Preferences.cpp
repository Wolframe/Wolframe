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
}

void Preferences::storeSettings( )
{
	QSettings s( SETTINGS_DOMAIN, SETTINGS_APP );
	s.setValue( "wolframe/host", m_host );
	s.setValue( "wolframe/port", m_port );
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
