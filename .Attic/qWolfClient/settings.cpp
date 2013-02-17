#include <QSettings>

#include "settings.hpp"

// default values for application settings
ApplicationSettings::ApplicationSettings()
{
	mainWindowPos = QPoint( 200, 200 );	// main window position
	mainWindowSize = QSize( 400, 400 );	// main window size

	saveUsername = true;			// persistent username
	lastUsername.clear();			// empty last username
	lastConnection.clear();			// empty last used connection

	developEnabled = false;			// development menu disabled
}

void ApplicationSettings::write( const QString& organization, const QString& application )
{
	QSettings settings( organization, application );

	settings.setValue( "position", mainWindowPos );
	settings.setValue( "size", mainWindowSize );
	settings.setValue( "developer", developEnabled );

	settings.beginGroup( "Login" );
	settings.setValue( "remember", saveUsername );
	settings.setValue( "user", lastUsername );
	settings.setValue( "connection", lastConnection );
	settings.endGroup();

	int size = settings.beginReadArray( "Connections" );
	for ( int i = 0; i < size; ++i )	{
		settings.setArrayIndex( i );
		settings.remove( "" );
	}
	settings.endArray();

	settings.beginWriteArray( "Connections" );
	for ( int i = 0; i < connectionParams.size(); ++i )	{
		settings.setArrayIndex( i );
		settings.setValue( "name", connectionParams[ i ].name );
		settings.setValue( "host", connectionParams[ i ].host );
		settings.setValue( "port", connectionParams[ i ].port );
		settings.setValue( "SSL", connectionParams[ i ].SSL );
		settings.setValue( "clientCertificate", connectionParams[ i ].clientCertificate );
		settings.setValue( "certificate", connectionParams[ i ].SSLcertificate );
		settings.setValue( "key", connectionParams[ i ].SSLkey );
		settings.setValue( "verify", connectionParams[ i ].SSLverify );
		settings.setValue( "CAbundle", connectionParams[ i ].SSLCAbundle );
		settings.setValue( "timeout", connectionParams[ i ].timeout );
	}
	settings.endArray();
}

void ApplicationSettings::read( const QString& organization, const QString& application )
{
	QSettings settings( organization, application );

	mainWindowPos = settings.value( "position", QPoint( 200, 200 ) ).toPoint();
	mainWindowSize = settings.value( "size", QSize( 400, 400 ) ).toSize();
	developEnabled = settings.value( "developer", false ).toBool();

	settings.beginGroup( "Login" );
	saveUsername = settings.value( "remember", true ).toBool();
	lastUsername = settings.value( "user" ).toString();
	lastConnection = settings.value( "connection" ).toString();
	settings.endGroup();

	int size = settings.beginReadArray( "Connections" );
	for ( int i = 0; i < size; ++i )	{
		settings.setArrayIndex( i );

		ConnectionParameters	params;
		params.name = settings.value( "name" ).toString();
		params.host = settings.value( "host" ).toString();
		params.port = settings.value( "port" ).toUInt();
		params.SSL = settings.value( "SSL" ).toBool();
		params.clientCertificate = settings.value( "clientCertificate" ).toBool();
		params.SSLcertificate = settings.value( "certificate" ).toString();
		params.SSLkey = settings.value( "key" ).toString();
		params.SSLverify = settings.value( "verify" ).toBool();
		params.SSLCAbundle = settings.value( "CAbundle" ).toString();
		params.timeout = settings.value( "timeout" ).toUInt();

		connectionParams.append( params );
	}
	settings.endArray();
}
