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

#include "global.hpp"
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

void ApplicationSettings::write( const QString &fileName )
{
	QSettings settings( fileName, QSettings::IniFormat );

	write( settings );
}

void ApplicationSettings::write( const QString& organization, const QString& application )
{
	QSettings settings( organization, application );
	
	write( settings );
}

void ApplicationSettings::write( QSettings &settings )
{
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
	
	settings.setValue( "mdi", mdi );
	settings.setValue( "debug", debug );
	settings.setValue( "locale", locale );
	settings.setValue( "uiLoadMode", LoadModeToStr( uiLoadMode ) );
	settings.setValue( "dataLoadMode", LoadModeToStr( dataLoadMode ) );
	settings.setValue( "uiFormsDir", uiFormsDir );
	settings.setValue( "uiFormTranslationsDir", uiFormTranslationsDir );
	settings.setValue( "uiFormResourcesDir", uiFormResourcesDir );
	settings.setValue( "dataLoaderDir", dataLoaderDir );

	settings.setValue( "keepState", saveRestoreState );
	settings.beginWriteArray( "WindowStates" );
	for( int i = 0; i < states.size( ); i++ ) {
		settings.setArrayIndex( i );
		settings.setValue( "form", states[i].form );
		settings.setValue( "position", states[i].position );
		settings.setValue( "size", states[i].size );
	}
	settings.endArray();
}

void ApplicationSettings::read( const QString &fileName )
{
	QSettings settings( fileName, QSettings::IniFormat );
	
	read( settings );
}

void ApplicationSettings::read( const QString& organization, const QString& application )
{
	QSettings settings( organization, application );
	
	read( settings );
}

void ApplicationSettings::read( QSettings &settings )
{
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

	mdi = settings.value( "mdi", false ).toBool( );
	debug = settings.value( "debug", false ).toBool( );
	locale = settings.value( "locale", SYSTEM_LANGUAGE ).toString( );
	uiLoadMode = LoadModeFromStr( settings.value( "uiLoadMode", LoadModeToStr( DEFAULT_UILOADMODE ) ).toString( ) );
	dataLoadMode = LoadModeFromStr( settings.value( "dataLoadMode", LoadModeToStr( DEFAULT_DATALOADMODE ) ).toString( ) );
	uiFormsDir = settings.value( "uiFormsDir", DEFAULT_UI_FORMS_DIR ).toString( );
	uiFormTranslationsDir = settings.value( "uiFormTranslationsDir", DEFAULT_UI_FORM_TRANSLATIONS_DIR ).toString( );
	uiFormResourcesDir = settings.value( "uiFormResourcesDir", DEFAULT_UI_FORM_RESOURCES_DIR ).toString( );
	dataLoaderDir = settings.value( "dataLoaderDir", DEFAULT_DATA_LOADER_DIR ).toString( );

	saveRestoreState = settings.value( "keepState", false ).toBool( );
	size = settings.beginReadArray( "WindowStates" );
	for ( int i = 0; i < size; ++i )	{
		settings.setArrayIndex( i );
		
		WinState state;
		state.form = settings.value( "form" ).toString( );
		state.position = settings.value( "position", QPoint( 0, 0 ) ).toPoint( );
		state.size = settings.value( "size",QSize( 350, 350 ) ).toSize( );
		
		states.append( state );
	}
	settings.endArray( );
}
