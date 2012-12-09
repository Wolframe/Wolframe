//
// PreferencesDialog.cpp
//

#include "PreferencesDialog.hpp"
#include "Preferences.hpp"
#include "LoadMode.hpp"
#include "global.hpp"

#include <QFormLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>


PreferencesDialog::PreferencesDialog( QWidget *_parent )
	: QDialog( _parent ), m_languages( )
{
	initialize( );
	loadSettings( );
}

PreferencesDialog::PreferencesDialog( QStringList _languages, QWidget *_parent )
	: QDialog( _parent ), m_languages( _languages )
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
	m_uiLoadModeLocalDatabase = new QRadioButton( tr( "Local &database" ) );
	m_uiLoadModeNetwork = new QRadioButton( tr( "&Network" ) );
	QVBoxLayout *vbox1 = new QVBoxLayout( );
	vbox1->addWidget( m_uiLoadModeLocalFile );
	vbox1->addWidget( m_uiLoadModeLocalDatabase );
	vbox1->addWidget( m_uiLoadModeNetwork );
	vbox1->addStretch( 1 );
	groupBox1->setLayout( vbox1 );
	formLayout->addRow( tr( "&UI load mode:" ), groupBox1 );

	QGroupBox *groupBox2 = new QGroupBox( );
	m_dataLoadModeLocalFile = new QRadioButton( tr( "Local &file" ) );
	m_dataLoadModeLocalDatabase = new QRadioButton( tr( "Local &database" ) );
	m_dataLoadModeNetwork = new QRadioButton( tr( "&Network" ) );
	QVBoxLayout *vbox2 = new QVBoxLayout( );
	vbox2->addWidget( m_dataLoadModeLocalFile );
	vbox2->addWidget( m_dataLoadModeLocalDatabase );
	vbox2->addWidget( m_dataLoadModeNetwork );
	vbox2->addStretch( 1 );
	groupBox2->setLayout( vbox2 );
	formLayout->addRow( tr( "&Data load mode:" ), groupBox2 );

	m_host = new QLineEdit( this );
	formLayout->addRow( tr( "&Host:" ), m_host );
	m_host->setFocus( );

	m_port = new QSpinBox( this );
	m_port->setRange( 1024, 49151 );
	formLayout->addRow( tr( "&Port:" ), m_port );

	m_secure = new QCheckBox( this );
	formLayout->addRow( tr( "&Secure:" ), m_secure );

	m_checkSSL = new QCheckBox( this );
	formLayout->addRow( tr( "&Check SSL:" ), m_checkSSL );
	
	m_clientCertFile = new FileChooser( FileChooser::SelectExistingFile,
		tr( "Select a file as client certificate" ), this );
	formLayout->addRow( tr( "&Client Certificate:" ), m_clientCertFile );
	
	m_clientKeyFile = new FileChooser( FileChooser::SelectExistingFile,
		tr( "Select a file as client key" ), this );
	formLayout->addRow( tr( "Client &Key:" ), m_clientKeyFile );
	
	m_CACertFile = new FileChooser( FileChooser::SelectExistingFile,
		tr( "Select a file as certificate authority key" ), this );
	formLayout->addRow( tr( "C&A file:" ), m_CACertFile );
		
	m_dbName = new FileChooser( FileChooser::SelectExistingFile,
		tr( "Select a file where to store a local sqlite database" ), this );
	formLayout->addRow( tr( "&Db file:" ), m_dbName );

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
	
	m_buttons = new QDialogButtonBox( this );
	m_buttons->addButton( QDialogButtonBox::Ok );
	m_buttons->addButton( QDialogButtonBox::Cancel );
	m_buttons->button( QDialogButtonBox::Ok )->setText( tr( "Apply" ) );
	m_buttons->button( QDialogButtonBox::Cancel )->setText( tr( "Cancel" ) );
	formLayout->addRow( m_buttons );
	setLayout( formLayout );
	
	connect( m_secure, SIGNAL( stateChanged( int ) ),
		this, SLOT( toggleSecure( int ) ) );
	
	connect( m_uiLoadModeLocalFile, SIGNAL( toggled( bool ) ),
		this, SLOT( toggleLoadMode( bool ) ) );
		
	connect( m_uiLoadModeLocalDatabase, SIGNAL( toggled( bool ) ),
		this, SLOT( toggleLoadMode( bool ) ) );

	connect( m_uiLoadModeNetwork, SIGNAL( toggled( bool ) ),
		this, SLOT( toggleLoadMode( bool ) ) );

	connect( m_dataLoadModeLocalFile, SIGNAL( toggled( bool ) ),
		this, SLOT( toggleLoadMode( bool ) ) );
		
	connect( m_dataLoadModeLocalDatabase, SIGNAL( toggled( bool ) ),
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
	Preferences *prefs = Preferences::instance( );
	
	m_host->setText( prefs->host( ) );
	m_port->setValue( prefs->port( ) );
	m_secure->setChecked( prefs->secure( ) );
	m_checkSSL->setChecked( prefs->checkSSL( ) );
	m_clientCertFile->setFileName( prefs->clientCertFile( ) );
	m_clientCertFile->setEnabled( prefs->secure( ) );
	m_clientKeyFile->setFileName( prefs->clientKeyFile( ) );
	m_clientKeyFile->setEnabled( prefs->secure( ) );
	m_CACertFile->setFileName( prefs->caCertFile( ) );
	m_CACertFile->setEnabled( prefs->secure( ) );
	m_uiLoadModeLocalFile->setChecked( false );
	m_uiLoadModeLocalDatabase->setChecked( false );
	m_uiLoadModeNetwork->setChecked( false );
	m_dataLoadModeLocalFile->setChecked( false );
	m_dataLoadModeLocalDatabase->setChecked( false );
	m_dataLoadModeNetwork->setChecked( false );
	m_dbName->setEnabled( false );
	m_uiFormsDir->setEnabled( false );
	m_uiFormTranslationsDir->setEnabled( false );
	m_uiFormResourcesDir->setEnabled( false );
	m_dataLoaderDir->setEnabled( false );
	switch( prefs->uiLoadMode( ) ) {
		case Preferences::LocalFile:			
			m_uiLoadModeLocalFile->setChecked( true );
			m_uiFormsDir->setEnabled( true );
			m_uiFormTranslationsDir->setEnabled( true );
			m_uiFormResourcesDir->setEnabled( true );
			break;

		case Preferences::LocalDb:
			m_uiLoadModeLocalDatabase->setChecked( true );
			m_dbName->setEnabled( true );
			break;

		case Preferences::Network:
			m_uiLoadModeNetwork->setChecked( true );
			break;
	}
	switch( prefs->dataLoadMode( ) ) {
		case Preferences::LocalFile:			
			m_dataLoadModeLocalFile->setChecked( true );
			m_dataLoaderDir->setEnabled( true );
			break;

		case Preferences::LocalDb:
			m_dataLoadModeLocalDatabase->setChecked( true );
			m_dbName->setEnabled( true );
			break;

		case Preferences::Network:
			m_dataLoadModeNetwork->setChecked( true );
			break;
	}
	m_dbName->setFileName( prefs->dbName( ) );
	m_debug->setChecked( prefs->debug( ) );
	m_uiFormsDir->setFileName( prefs->uiFormsDir( ) );
	m_uiFormTranslationsDir->setFileName( prefs->uiFormTranslationsDir( ) );
	m_uiFormResourcesDir->setFileName( prefs->uiFormResourcesDir( ) );
	m_dataLoaderDir->setFileName( prefs->dataLoaderDir( ) );		
	if( !m_languages.empty( ) ) {
		QString lang = prefs->locale( );
		if( lang == SYSTEM_LANGUAGE ) {
			m_systemLanguage->setChecked( true );
		} else {
			m_systemLanguage->setChecked( false );
			int idx = m_locale->findData( prefs->locale( ) );
			if( idx != -1 ) {
				m_locale->setCurrentIndex( idx );
			}
		}
	}
}

void PreferencesDialog::apply( )
{
	Preferences *prefs = Preferences::instance( );
	
	prefs->setHost( m_host->text( ) );
	prefs->setPort( m_port->value( ) );
	prefs->setSecure( m_secure->isChecked( ) );
	prefs->setCheckSSL( m_checkSSL->isChecked( ) );
	prefs->setClientCertFile( m_clientCertFile->fileName( ) );
	prefs->setClientKeyFile( m_clientKeyFile->fileName( ) );
	prefs->setCaCertFile( m_CACertFile->fileName( ) );
	if( m_uiLoadModeLocalFile->isChecked( ) ) {
		prefs->setUiLoadMode( Preferences::LocalFile );
	} else if( m_uiLoadModeLocalDatabase->isChecked( ) ) {
		prefs->setUiLoadMode( Preferences::LocalDb );
	} else if( m_uiLoadModeNetwork->isChecked( ) ) {
		prefs->setUiLoadMode( Preferences::Network );
	}
	if( m_dataLoadModeLocalFile->isChecked( ) ) {
		prefs->setDataLoadMode( Preferences::LocalFile );
	} else if( m_dataLoadModeLocalDatabase->isChecked( ) ) {
		prefs->setDataLoadMode( Preferences::LocalDb );
	} else if( m_dataLoadModeNetwork->isChecked( ) ) {
		prefs->setDataLoadMode( Preferences::Network );
	}
	prefs->setDbName( m_dbName->fileName( ) );
	prefs->setDebug( m_debug->isChecked( ) );
	prefs->setUiFormsDir( m_uiFormsDir->fileName( ) );
	prefs->setUiFormTranslationsDir( m_uiFormTranslationsDir->fileName( ) );
	prefs->setUiFormResourcesDir( m_uiFormResourcesDir->fileName( ) );
	prefs->setDataLoaderDir( m_dataLoaderDir->fileName( ) );
	if( !m_languages.empty( ) ) {
		if( m_systemLanguage->isChecked( ) ) {
			prefs->setLocale( SYSTEM_LANGUAGE );
		} else {
			QString lang = m_locale->itemData( m_locale->currentIndex( ) ).toString( );
			prefs->setLocale( lang );
		}
	}
	
	prefs->storeSettings( );

	emit prefsChanged( );
	
	accept( );	
}

void PreferencesDialog::cancel( )
{
	close( );
}

void PreferencesDialog::toggleSecure( int /* state */ )
{
	bool secure = m_secure->isChecked( );
	m_checkSSL->setEnabled( secure );
	m_clientCertFile->setEnabled( secure );
	m_clientKeyFile->setEnabled( secure );
	m_CACertFile->setEnabled( secure );
}

void PreferencesDialog::toggleLoadMode( bool /* checked */ )
{
	m_dbName->setEnabled( m_uiLoadModeLocalDatabase->isChecked( ) || m_dataLoadModeLocalDatabase->isChecked( ) );
	m_host->setEnabled( m_uiLoadModeNetwork->isChecked( ) || m_dataLoadModeNetwork->isChecked( ) );
	m_port->setEnabled( m_uiLoadModeNetwork->isChecked( ) | m_dataLoadModeNetwork->isChecked( ) );
	m_secure->setEnabled( m_uiLoadModeNetwork->isChecked( ) || m_dataLoadModeNetwork->isChecked( ) );
	m_clientCertFile->setEnabled( ( m_uiLoadModeNetwork->isChecked( ) || m_dataLoadModeNetwork->isChecked( ) ) && m_secure->isChecked( ) );
	m_clientKeyFile->setEnabled( ( m_uiLoadModeNetwork->isChecked( ) || m_dataLoadModeNetwork->isChecked( ) ) && m_secure->isChecked( ) );
	m_CACertFile->setEnabled( ( m_uiLoadModeNetwork->isChecked( ) || m_dataLoadModeNetwork->isChecked( ) ) && m_secure->isChecked( ) );
	m_uiFormsDir->setEnabled( m_uiLoadModeLocalFile->isChecked( ) );
	m_uiFormTranslationsDir->setEnabled( m_uiLoadModeLocalFile->isChecked( ) );
	m_uiFormResourcesDir->setEnabled( m_uiLoadModeLocalFile->isChecked( ) );
	m_dataLoaderDir->setEnabled( m_dataLoadModeLocalFile->isChecked( ) );
}

void PreferencesDialog::toggleSystemLanguage( int /* state */ )
{
	m_locale->setEnabled( !( m_systemLanguage->isChecked( ) ) );
}

