//
// PreferencesDialog.cpp
//

#include "PreferencesDialog.hpp"
#include "Preferences.hpp"

#include <QFormLayout>

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
	
	m_host = new QLineEdit( this );
	formLayout->addRow( tr( "&Host:" ), m_host );
	m_host->setFocus( );

	m_port = new QSpinBox( this );
	m_port->setRange( 1024, 49151 );
	formLayout->addRow( tr( "&Port:" ), m_port );

	m_secure = new QCheckBox( this );
	formLayout->addRow( tr( "&Secure:" ), m_secure );

	m_clientCertFile = new FileChooser( this );
	formLayout->addRow( tr( "&Client Certificate:" ), m_clientCertFile );
	
	m_clientKeyFile = new FileChooser( this );
	formLayout->addRow( tr( "Client &Key:" ), m_clientKeyFile );
	
	m_CACertFile = new FileChooser( this );
	formLayout->addRow( tr( "C&A file:" ), m_CACertFile );
	
	m_buttons = new QDialogButtonBox( this );
	m_buttons->addButton( QDialogButtonBox::Ok );
	m_buttons->addButton( QDialogButtonBox::Cancel );
	m_buttons->button( QDialogButtonBox::Ok )->setText( tr( "Apply" ) );
	m_buttons->button( QDialogButtonBox::Cancel )->setText( tr( "Cancel" ) );
	formLayout->addRow( m_buttons );
	setLayout( formLayout );
	
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
	m_clientKeyFile->setFileName( prefs->clientKeyFile( ) );
	m_CACertFile->setFileName( prefs->caCertFile( ) );
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
	
// store settings to config file/registry, TODO: move to prefs dialog later	
	prefs->storeSettings( );
	
	close( );
}

void PreferencesDialog::cancel( )
{
	close( );
}

} // namespace QtClient
} // namespace _Wolframe
