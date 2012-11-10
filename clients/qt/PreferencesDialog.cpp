//
// PreferencesDialog.cpp
//

#include "PreferencesDialog.hpp"
#include "Preferences.hpp"
#include "LoadMode.hpp"

#include <QFormLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>

namespace _Wolframe {
	namespace QtClient {

PreferencesDialog::PreferencesDialog( QWidget *_parent ) :
	QDialog( _parent )
{
	initialize( );
	loadSettings( );
	
	setWindowTitle( tr( "Preferences" ) );
	setModal( true );
}

void PreferencesDialog::initialize( )
{
	QFormLayout *formLayout = new QFormLayout( );
	
	QGroupBox *groupBox = new QGroupBox( );
	m_loadModeLocalFile = new QRadioButton( tr( "Local &file" ) );
	m_loadModeLocalDatabase = new QRadioButton( tr( "Local &database" ) );
	m_loadModeNetwork = new QRadioButton( tr( "&Network" ) );
	
	QVBoxLayout *vbox = new QVBoxLayout( );
	vbox->addWidget( m_loadModeLocalFile );
	vbox->addWidget( m_loadModeLocalDatabase );
	vbox->addWidget( m_loadModeNetwork );
	vbox->addStretch( 1 );
	groupBox->setLayout( vbox );
	
	formLayout->addRow( tr( "&Load mode:" ), groupBox );

	m_host = new QLineEdit( this );
	formLayout->addRow( tr( "&Host:" ), m_host );
	m_host->setFocus( );

	m_port = new QSpinBox( this );
	m_port->setRange( 1024, 49151 );
	formLayout->addRow( tr( "&Port:" ), m_port );

	m_secure = new QCheckBox( this );
	formLayout->addRow( tr( "&Secure:" ), m_secure );

	m_clientCertFile = new FileChooser( FileChooser::SelectExistingFile, this );
	formLayout->addRow( tr( "&Client Certificate:" ), m_clientCertFile );
	
	m_clientKeyFile = new FileChooser( FileChooser::SelectExistingFile, this );
	formLayout->addRow( tr( "Client &Key:" ), m_clientKeyFile );
	
	m_CACertFile = new FileChooser( FileChooser::SelectExistingFile, this );
	formLayout->addRow( tr( "C&A file:" ), m_CACertFile );
		
	m_dbName = new FileChooser( FileChooser::SelectExistingFile, this );
	formLayout->addRow( tr( "&Db file:" ), m_dbName );

	m_uiFormsDir = new FileChooser( FileChooser::SelectExistingDir, this );
	formLayout->addRow( tr( "Form dir:" ), m_uiFormsDir );

	m_uiFormTranslationsDir = new FileChooser( FileChooser::SelectExistingDir, this );
	formLayout->addRow( tr( "I18N dir:" ), m_uiFormTranslationsDir );

	m_dataLoaderDir = new FileChooser( FileChooser::SelectExistingDir, this );
	formLayout->addRow( tr( "Data dir:" ), m_dataLoaderDir );

	m_debug = new QCheckBox( this );
	formLayout->addRow( tr( "&Debug:" ), m_debug );
	
	m_buttons = new QDialogButtonBox( this );
	m_buttons->addButton( QDialogButtonBox::Ok );
	m_buttons->addButton( QDialogButtonBox::Cancel );
	m_buttons->button( QDialogButtonBox::Ok )->setText( tr( "Apply" ) );
	m_buttons->button( QDialogButtonBox::Cancel )->setText( tr( "Cancel" ) );
	formLayout->addRow( m_buttons );
	setLayout( formLayout );
	
	connect( m_secure, SIGNAL( stateChanged( int ) ),
		this, SLOT( toggleSecure( int ) ) );
	
	connect( m_loadModeLocalFile, SIGNAL( toggled( bool ) ),
		this, SLOT( toggleLoadMode( bool ) ) );
		
	connect( m_loadModeLocalDatabase, SIGNAL( toggled( bool ) ),
		this, SLOT( toggleLoadMode( bool ) ) );

	connect( m_loadModeNetwork, SIGNAL( toggled( bool ) ),
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
	m_clientCertFile->setFileName( prefs->clientCertFile( ) );
	m_clientCertFile->setEnabled( prefs->secure( ) );
	m_clientKeyFile->setFileName( prefs->clientKeyFile( ) );
	m_clientKeyFile->setEnabled( prefs->secure( ) );
	m_CACertFile->setFileName( prefs->caCertFile( ) );
	m_CACertFile->setEnabled( prefs->secure( ) );
	m_loadModeLocalFile->setChecked( false );
	m_loadModeLocalDatabase->setChecked( false );
	m_loadModeNetwork->setChecked( false );
	m_dbName->setEnabled( false );
	m_uiFormsDir->setEnabled( false );
	m_uiFormTranslationsDir->setEnabled( false );
	m_dataLoaderDir->setEnabled( false );
	switch( prefs->loadMode( ) ) {
		case Preferences::LocalFile:			
			m_loadModeLocalFile->setChecked( true );
			m_uiFormsDir->setEnabled( true );
			m_uiFormTranslationsDir->setEnabled( true );
			m_dataLoaderDir->setEnabled( true );
			break;

		case Preferences::LocalDb:
			m_loadModeLocalDatabase->setChecked( true );
			m_dbName->setEnabled( true );
			break;

		case Preferences::Network:
			m_loadModeNetwork->setChecked( true );
			break;
	}
	m_dbName->setFileName( prefs->dbName( ) );
	m_debug->setChecked( prefs->debug( ) );
	m_uiFormsDir->setFileName( prefs->uiFormsDir( ) );
	m_uiFormTranslationsDir->setFileName( prefs->uiFormTranslationsDir( ) );
	m_dataLoaderDir->setFileName( prefs->dataLoaderDir( ) );
}

void PreferencesDialog::apply( )
{
	Preferences *prefs = Preferences::instance( );
	
	prefs->setHost( m_host->text( ) );
	prefs->setPort( m_port->value( ) );
	prefs->setSecure( m_secure->isChecked( ) );
	prefs->setClientCertFile( m_clientCertFile->fileName( ) );
	prefs->setClientKeyFile( m_clientKeyFile->fileName( ) );
	prefs->setCaCertFile( m_CACertFile->fileName( ) );
	if( m_loadModeLocalFile->isChecked( ) ) {
		prefs->setLoadMode( Preferences::LocalFile );
	} else if( m_loadModeLocalDatabase->isChecked( ) ) {
		prefs->setLoadMode( Preferences::LocalDb );
	} else if( m_loadModeNetwork->isChecked( ) ) {
		prefs->setLoadMode( Preferences::Network );
	}
	prefs->setDbName( m_dbName->fileName( ) );
	prefs->setDebug( m_debug->isChecked( ) );
	prefs->setUiFormsDir( m_uiFormsDir->fileName( ) );
	prefs->setUiFormTranslationsDir( m_uiFormTranslationsDir->fileName( ) );
	prefs->setDataLoaderDir( m_dataLoaderDir->fileName( ) );
	
	prefs->storeSettings( );
	
	close( );
	
	emit prefsChanged( );
}

void PreferencesDialog::cancel( )
{
	close( );
}

void PreferencesDialog::toggleSecure( int /* state */ )
{
	bool secure = m_secure->isChecked( );
	m_clientCertFile->setEnabled( secure );
	m_clientKeyFile->setEnabled( secure );
	m_CACertFile->setEnabled( secure );
}

void PreferencesDialog::toggleLoadMode( bool /* checked */ )
{
	m_dbName->setEnabled( m_loadModeLocalDatabase->isChecked( ) );
	m_host->setEnabled( m_loadModeNetwork->isChecked( ) );
	m_port->setEnabled( m_loadModeNetwork->isChecked( ) );
	m_secure->setEnabled( m_loadModeNetwork->isChecked( ) );
	m_clientCertFile->setEnabled( m_loadModeNetwork->isChecked( ) && m_secure->isChecked( ) );
	m_clientKeyFile->setEnabled( m_loadModeNetwork->isChecked( ) && m_secure->isChecked( ) );
	m_CACertFile->setEnabled( m_loadModeNetwork->isChecked( ) && m_secure->isChecked( ) );
	m_uiFormsDir->setEnabled( m_loadModeLocalFile->isChecked( ) );
	m_uiFormTranslationsDir->setEnabled( m_loadModeLocalFile->isChecked( ) );
	m_dataLoaderDir->setEnabled( m_loadModeLocalFile->isChecked( ) );
}

} // namespace QtClient
} // namespace _Wolframe
