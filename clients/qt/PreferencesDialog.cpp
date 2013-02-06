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

#include "PreferencesDialog.hpp"
#include "LoadMode.hpp"
#include "global.hpp"

#include <QFormLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>

PreferencesDialogInterface::PreferencesDialogInterface( QWidget *_parent )
	: QWidget( _parent )
{
	setupUi( this );
}

PreferencesDialogDeveloper::PreferencesDialogDeveloper( QWidget *_parent )
	: QWidget( _parent )
{
	setupUi( this );
}

PreferencesDialog::PreferencesDialog( ApplicationSettings &_settings, QStringList _languages, QWidget *_parent )
	: QDialog( _parent ), m_settings( _settings ), m_languages( _languages )
{
	initialize( );
	loadSettings( );
}

void PreferencesDialog::initialize( )
{
	setupUi( this );
	
	m_interface = new PreferencesDialogInterface( this );
	m_developer = new PreferencesDialogDeveloper( this );

	stackedWidget->addWidget( m_interface );
	stackedWidget->addWidget( m_developer );
	stackedWidget->setCurrentIndex( 0 );

	listWidget->addItem( new QListWidgetItem( QIcon( QString( ":/images/interface.png") ),
		tr( "Interface" ), listWidget ) );
	listWidget->addItem( new QListWidgetItem( QIcon( QString( ":/images/development.png") ),
		tr( "Developer" ), listWidget ) );
	listWidget->setCurrentRow( 0 );
	
	connect( buttonBox->button( QDialogButtonBox::Save ), SIGNAL( clicked( ) ),
		this, SLOT( apply( ) ) );
		
	connect( buttonBox->button( QDialogButtonBox::Cancel ), SIGNAL( clicked( ) ),
		this, SLOT( cancel( ) ) );	

	connect( buttonBox->button( QDialogButtonBox::RestoreDefaults ), SIGNAL( clicked( ) ),
		this, SLOT( restoreDefaults( ) ) );	
	
	// interface
	
	if( !m_languages.empty( ) ) {
		connect( m_interface->systemLocale, SIGNAL( toggled( bool ) ),
			this, SLOT( toggleLocale( bool ) ) );
		connect( m_interface->manualLocale, SIGNAL( toggled( bool ) ),
			this, SLOT( toggleLocale( bool ) ) );

		foreach( QString language, m_languages ) {
			QLocale myLocale( language );
			QString printableLanguage = myLocale.languageToString( myLocale.language( ) ) + " (" + language + ")";
			m_interface->locales->addItem( printableLanguage, language );
		}
	}
	
	// developer
	
	connect( m_developer->uiLoadModeLocalFile, SIGNAL( toggled( bool ) ),
		this, SLOT( toggleLoadMode( bool ) ) );
		
	connect( m_developer->uiLoadModeNetwork, SIGNAL( toggled( bool ) ),
		this, SLOT( toggleLoadMode( bool ) ) );

	connect( m_developer->dataLoadModeLocalFile, SIGNAL( toggled( bool ) ),
		this, SLOT( toggleLoadMode( bool ) ) );
		
	connect( m_developer->dataLoadModeNetwork, SIGNAL( toggled( bool ) ),
		this, SLOT( toggleLoadMode( bool ) ) );	
}

void PreferencesDialog::loadSettings( )
{	
	// interface

	m_interface->mdi->setChecked( m_settings.mdi );

	if( !m_languages.empty( ) ) {
		QString lang = m_settings.locale;
		if( lang == SYSTEM_LANGUAGE ) {
			m_interface->systemLocale->setChecked( true );
			m_interface->manualLocale->setChecked( false );
		} else {
			m_interface->systemLocale->setChecked( false );
			m_interface->manualLocale->setChecked( true );
			int idx = m_interface->locales->findData( m_settings.locale );
			if( idx != -1 ) {
				m_interface->locales->setCurrentIndex( idx );
			}
		}
	}
	
	m_interface->rememberLogin->setChecked( m_settings.saveUsername );
	m_interface->saveRestoreState->setChecked( m_settings.saveRestoreState );
	
	// developer
	
	m_developer->uiLoadModeLocalFile->setChecked( false );
	m_developer->uiLoadModeNetwork->setChecked( false );
	m_developer->dataLoadModeLocalFile->setChecked( false );
	m_developer->dataLoadModeNetwork->setChecked( false );
	m_developer->uiFormsDir->setEnabled( false );
	m_developer->uiFormTranslationsDir->setEnabled( false );
	m_developer->uiFormResourcesDir->setEnabled( false );
	m_developer->dataLoaderDir->setEnabled( false );
	switch( m_settings.uiLoadMode ) {
		case LocalFile:			
			m_developer->uiLoadModeLocalFile->setChecked( true );
			m_developer->uiFormsDir->setEnabled( true );
			m_developer->uiFormTranslationsDir->setEnabled( true );
			m_developer->uiFormResourcesDir->setEnabled( true );
			break;

		case Network:
			m_developer->uiLoadModeNetwork->setChecked( true );
			break;
		
		case Unknown:
			break;
	}
	switch( m_settings.dataLoadMode ) {
		case LocalFile:			
			m_developer->dataLoadModeLocalFile->setChecked( true );
			m_developer->dataLoaderDir->setEnabled( true );
			break;

		case Network:
			m_developer->dataLoadModeNetwork->setChecked( true );
			break;

		case Unknown:
			break;
	}
	
	m_developer->debug->setChecked( m_settings.debug );
	m_developer->developer->setChecked( m_settings.developEnabled );
	
	m_developer->uiFormsDir->setFileName( m_settings.uiFormsDir );
	m_developer->uiFormTranslationsDir->setFileName( m_settings.uiFormTranslationsDir );
	m_developer->uiFormResourcesDir->setFileName( m_settings.uiFormResourcesDir );
	m_developer->dataLoaderDir->setFileName( m_settings.dataLoaderDir );
}

void PreferencesDialog::apply( )
{
	// interface

	m_settings.mdi = m_interface->mdi->isChecked( );

	if( !m_languages.empty( ) ) {
		if( m_interface->systemLocale->isChecked( ) ) {
			m_settings.locale = SYSTEM_LANGUAGE;
		} else {
			QString lang = m_interface->locales->itemData( m_interface->locales->currentIndex( ) ).toString( );
			m_settings.locale = lang;
		}
	}
	
	m_settings.saveUsername = m_interface->rememberLogin->isChecked( );
	m_settings.saveRestoreState = m_interface->saveRestoreState->isChecked( );
	m_interface->saveRestoreState->setChecked( false );
	
	// developer
	
	if( m_developer->uiLoadModeLocalFile->isChecked( ) ) {
		m_settings.uiLoadMode = LocalFile;
	} else if( m_developer->uiLoadModeNetwork->isChecked( ) ) {
		m_settings.uiLoadMode = Network;
	}
	if( m_developer->dataLoadModeLocalFile->isChecked( ) ) {
		m_settings.dataLoadMode = LocalFile;
	} else if( m_developer->dataLoadModeNetwork->isChecked( ) ) {
		m_settings.dataLoadMode = Network;
	}
	m_settings.debug = m_developer->debug->isChecked( );
	m_settings.developEnabled = m_developer->developer->isChecked( );
	m_settings.uiFormsDir = m_developer->uiFormsDir->fileName( );
	m_settings.uiFormTranslationsDir = m_developer->uiFormTranslationsDir->fileName( );
	m_settings.uiFormResourcesDir = m_developer->uiFormResourcesDir->fileName( );
	m_settings.dataLoaderDir = m_developer->dataLoaderDir->fileName( );
	
	accept( );	
}

void PreferencesDialog::restoreDefaults( )
{
	// interface

	m_interface->mdi->setChecked( DEFAULT_MDI );

	m_interface->systemLocale->setChecked( false );
	m_interface->manualLocale->setChecked( true );
	int idx = m_interface->locales->findData( DEFAULT_LOCALE );
	if( idx != -1 ) {
		m_interface->locales->setCurrentIndex( idx );
	}

	m_interface->rememberLogin->setChecked( false );
	m_interface->saveRestoreState->setChecked( false );

	// developer

	m_developer->uiLoadModeLocalFile->setChecked( false );
	m_developer->uiLoadModeNetwork->setChecked( false );
	m_developer->dataLoadModeLocalFile->setChecked( false );
	m_developer->dataLoadModeNetwork->setChecked( false );
	m_developer->uiFormsDir->setEnabled( false );
	m_developer->uiFormTranslationsDir->setEnabled( false );
	m_developer->uiFormResourcesDir->setEnabled( false );
	m_developer->dataLoaderDir->setEnabled( false );
	switch( DEFAULT_UILOADMODE ) {
		case LocalFile:			
			m_developer->uiLoadModeLocalFile->setChecked( true );
			m_developer->uiFormsDir->setEnabled( true );
			m_developer->uiFormTranslationsDir->setEnabled( true );
			m_developer->uiFormResourcesDir->setEnabled( true );
			break;

		case Network:
			m_developer->uiLoadModeNetwork->setChecked( true );
			break;
		
		case Unknown:
			break;
	}
	switch( DEFAULT_DATALOADMODE ) {
		case LocalFile:			
			m_developer->dataLoadModeLocalFile->setChecked( true );
			m_developer->dataLoaderDir->setEnabled( true );
			break;

		case Network:
			m_developer->dataLoadModeNetwork->setChecked( true );
			break;

		case Unknown:
			break;
	}
	m_developer->debug->setChecked( false );
	m_developer->developer->setChecked( false );
	m_developer->uiFormsDir->setFileName( DEFAULT_UI_FORMS_DIR );
	m_developer->uiFormTranslationsDir->setFileName( DEFAULT_UI_FORM_TRANSLATIONS_DIR );
	m_developer->uiFormResourcesDir->setFileName( DEFAULT_UI_FORM_RESOURCES_DIR );
	m_developer->dataLoaderDir->setFileName( DEFAULT_DATA_LOADER_DIR );
}

void PreferencesDialog::cancel( )
{
	close( );
}

void PreferencesDialog::toggleLoadMode( bool /* checked */ )
{
	m_developer->uiFormsDir->setEnabled( m_developer->uiLoadModeLocalFile->isChecked( ) );
	m_developer->uiFormTranslationsDir->setEnabled( m_developer->uiLoadModeLocalFile->isChecked( ) );
	m_developer->uiFormResourcesDir->setEnabled( m_developer->uiLoadModeLocalFile->isChecked( ) );
	m_developer->dataLoaderDir->setEnabled( m_developer->dataLoadModeLocalFile->isChecked( ) );
}

void PreferencesDialog::toggleLocale( bool /* checked */ )
{
	m_interface->locales->setEnabled( m_interface->manualLocale->isChecked( ) );
}
