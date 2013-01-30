//
// PreferencesDialog.cpp
//

#include "PreferencesDialog.hpp"
#include "LoadMode.hpp"
#include "global.hpp"

#include <QFormLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>

PreferencesDialog::PreferencesDialog( ApplicationSettings &_settings, QStringList _languages, QWidget *_parent )
	: QDialog( _parent ), m_settings( _settings ), m_languages( _languages )
{
	initialize( );
	loadSettings( );
}

void PreferencesDialog::initialize( )
{
	setWindowTitle( tr( "Preferences" ) );
	setModal( true );
	
	QFormLayout *formLayout = new QFormLayout( );

	QGroupBox *groupBox1 = new QGroupBox( );
	m_uiLoadModeLocalFile = new QRadioButton( tr( "Local &file" ) );
	m_uiLoadModeNetwork = new QRadioButton( tr( "&Network" ) );
	QVBoxLayout *vbox1 = new QVBoxLayout( );
	vbox1->addWidget( m_uiLoadModeLocalFile );
	vbox1->addWidget( m_uiLoadModeNetwork );
	vbox1->addStretch( 1 );
	groupBox1->setLayout( vbox1 );
	formLayout->addRow( tr( "&UI load mode:" ), groupBox1 );

	QGroupBox *groupBox2 = new QGroupBox( );
	m_dataLoadModeLocalFile = new QRadioButton( tr( "Local &file" ) );
	m_dataLoadModeNetwork = new QRadioButton( tr( "&Network" ) );
	QVBoxLayout *vbox2 = new QVBoxLayout( );
	vbox2->addWidget( m_dataLoadModeLocalFile );
	vbox2->addWidget( m_dataLoadModeNetwork );
	vbox2->addStretch( 1 );
	groupBox2->setLayout( vbox2 );
	formLayout->addRow( tr( "&Data load mode:" ), groupBox2 );

	m_uiFormsDir = new FileChooser( FileChooser::SelectExistingDir,
		tr( "Select a directory holding UI forms" ), this );
	formLayout->addRow( tr( "Form dir:" ), m_uiFormsDir );

	m_uiFormTranslationsDir = new FileChooser( FileChooser::SelectExistingDir,
		tr( "Select a directory holding UI form translations" ), this );
	formLayout->addRow( tr( "I18N dir:" ), m_uiFormTranslationsDir );
	
	m_uiFormResourcesDir = new FileChooser( FileChooser::SelectExistingDir,
		tr( "Select a directory holding UI form resources" ), this );
	formLayout->addRow( tr( "Resources dir:" ), m_uiFormResourcesDir );

	m_dataLoaderDir = new FileChooser( FileChooser::SelectExistingDir,
		tr( "Select a directory which contains local XML data" ), this );
	formLayout->addRow( tr( "Data dir:" ), m_dataLoaderDir );

	m_debug = new QCheckBox( this );
	formLayout->addRow( tr( "&Debug:" ), m_debug );

	m_developer = new QCheckBox( this );
	formLayout->addRow( tr( "D&eveloper:" ), m_developer );

	if( !m_languages.empty( ) ) {
		m_systemLanguage = new QCheckBox( this );
		formLayout->addRow( tr( "&Use system language:" ), m_systemLanguage );

		connect( m_systemLanguage, SIGNAL( stateChanged( int ) ),
			this, SLOT( toggleSystemLanguage( int ) ) );
		
		m_locale = new QComboBox( this );
		foreach( QString language, m_languages ) {
			QLocale myLocale( language );
			QString printableLanguage = myLocale.languageToString( myLocale.language( ) ) + " (" + language + ")";
			m_locale->addItem( printableLanguage, language );
		}
		formLayout->addRow( tr( "&Locale:" ), m_locale );		
	}

	m_mdi = new QCheckBox( this );
	formLayout->addRow( tr( "&MDI mode:" ), m_mdi );
	
	m_buttons = new QDialogButtonBox( this );
	m_buttons->addButton( QDialogButtonBox::Ok );
	m_buttons->addButton( QDialogButtonBox::Cancel );
	m_buttons->button( QDialogButtonBox::Ok )->setText( tr( "Apply" ) );
	m_buttons->button( QDialogButtonBox::Cancel )->setText( tr( "Cancel" ) );
	formLayout->addRow( m_buttons );
	setLayout( formLayout );
		
	connect( m_uiLoadModeLocalFile, SIGNAL( toggled( bool ) ),
		this, SLOT( toggleLoadMode( bool ) ) );
		
	connect( m_uiLoadModeNetwork, SIGNAL( toggled( bool ) ),
		this, SLOT( toggleLoadMode( bool ) ) );

	connect( m_dataLoadModeLocalFile, SIGNAL( toggled( bool ) ),
		this, SLOT( toggleLoadMode( bool ) ) );
		
	connect( m_dataLoadModeNetwork, SIGNAL( toggled( bool ) ),
		this, SLOT( toggleLoadMode( bool ) ) );
		
	connect( m_buttons->button( QDialogButtonBox::Ok ), SIGNAL( clicked( ) ),
		this, SLOT( apply( ) ) );
		
	connect( m_buttons->button( QDialogButtonBox::Cancel ), SIGNAL( clicked( ) ),
		this, SLOT( cancel( ) ) );	
}

void PreferencesDialog::loadSettings( )
{	
	m_uiLoadModeLocalFile->setChecked( false );
	m_uiLoadModeNetwork->setChecked( false );
	m_dataLoadModeLocalFile->setChecked( false );
	m_dataLoadModeNetwork->setChecked( false );
	m_uiFormsDir->setEnabled( false );
	m_uiFormTranslationsDir->setEnabled( false );
	m_uiFormResourcesDir->setEnabled( false );
	m_dataLoaderDir->setEnabled( false );
	switch( m_settings.uiLoadMode ) {
		case LocalFile:			
			m_uiLoadModeLocalFile->setChecked( true );
			m_uiFormsDir->setEnabled( true );
			m_uiFormTranslationsDir->setEnabled( true );
			m_uiFormResourcesDir->setEnabled( true );
			break;

		case Network:
			m_uiLoadModeNetwork->setChecked( true );
			break;
		
		case Unknown:
			break;
	}
	switch( m_settings.dataLoadMode ) {
		case LocalFile:			
			m_dataLoadModeLocalFile->setChecked( true );
			m_dataLoaderDir->setEnabled( true );
			break;

		case Network:
			m_dataLoadModeNetwork->setChecked( true );
			break;

		case Unknown:
			break;
	}
	m_debug->setChecked( m_settings.debug );
	m_developer->setChecked( m_settings.developEnabled );
	m_uiFormsDir->setFileName( m_settings.uiFormsDir );
	m_uiFormTranslationsDir->setFileName( m_settings.uiFormTranslationsDir );
	m_uiFormResourcesDir->setFileName( m_settings.uiFormResourcesDir );
	m_dataLoaderDir->setFileName( m_settings.dataLoaderDir );
	if( !m_languages.empty( ) ) {
		QString lang = m_settings.locale;
		if( lang == SYSTEM_LANGUAGE ) {
			m_systemLanguage->setChecked( true );
		} else {
			m_systemLanguage->setChecked( false );
			int idx = m_locale->findData( m_settings.locale );
			if( idx != -1 ) {
				m_locale->setCurrentIndex( idx );
			}
		}
	}
	m_mdi->setChecked( m_settings.mdi );
}

void PreferencesDialog::apply( )
{
	if( m_uiLoadModeLocalFile->isChecked( ) ) {
		m_settings.uiLoadMode = LocalFile;
	} else if( m_uiLoadModeNetwork->isChecked( ) ) {
		m_settings.uiLoadMode = Network;
	}
	if( m_dataLoadModeLocalFile->isChecked( ) ) {
		m_settings.dataLoadMode = LocalFile;
	} else if( m_dataLoadModeNetwork->isChecked( ) ) {
		m_settings.dataLoadMode = Network;
	}
	m_settings.debug = m_debug->isChecked( );
	m_settings.developEnabled = m_developer->isChecked( );
	m_settings.uiFormsDir = m_uiFormsDir->fileName( );
	m_settings.uiFormTranslationsDir = m_uiFormTranslationsDir->fileName( );
	m_settings.uiFormResourcesDir = m_uiFormResourcesDir->fileName( );
	m_settings.dataLoaderDir = m_dataLoaderDir->fileName( );
	if( !m_languages.empty( ) ) {
		if( m_systemLanguage->isChecked( ) ) {
			m_settings.locale = SYSTEM_LANGUAGE;
		} else {
			QString lang = m_locale->itemData( m_locale->currentIndex( ) ).toString( );
			m_settings.locale = lang;
		}
	}
	m_settings.mdi = m_mdi->isChecked( );

	emit prefsChanged( );
	
	accept( );	
}

void PreferencesDialog::cancel( )
{
	close( );
}

void PreferencesDialog::toggleLoadMode( bool /* checked */ )
{
	m_uiFormsDir->setEnabled( m_uiLoadModeLocalFile->isChecked( ) );
	m_uiFormTranslationsDir->setEnabled( m_uiLoadModeLocalFile->isChecked( ) );
	m_uiFormResourcesDir->setEnabled( m_uiLoadModeLocalFile->isChecked( ) );
	m_dataLoaderDir->setEnabled( m_dataLoadModeLocalFile->isChecked( ) );
}

void PreferencesDialog::toggleSystemLanguage( int /* state */ )
{
	m_locale->setEnabled( !( m_systemLanguage->isChecked( ) ) );
}

