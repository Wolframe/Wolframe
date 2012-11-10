//
// StorageWidget.cpp
//

#include "StorageWidget.hpp"

#include <QFormLayout>
#include <QGroupBox>
#include <QTreeWidgetItem>

#include "Preferences.hpp"

#include "FileFormLoader.hpp"
#include "SqliteFormLoader.hpp"
#include "NetworkFormLoader.hpp"

namespace _Wolframe {
	namespace QtClient {

StorageWidget::StorageWidget( QWidget *_parent )
	: QWidget( _parent )
{
	initialize( );
	loadSettings( );
	loadTree( );
}

void StorageWidget::initialize( )
{
	QFormLayout *formLayout = new QFormLayout( );
	formLayout->setMargin( 0 );
	
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

	m_clientCertFile = new FileChooser( this );
	formLayout->addRow( tr( "&Client Certificate:" ), m_clientCertFile );
	
	m_clientKeyFile = new FileChooser( this );
	formLayout->addRow( tr( "Client &Key:" ), m_clientKeyFile );
	
	m_CACertFile = new FileChooser( this );
	formLayout->addRow( tr( "C&A file:" ), m_CACertFile );
		
	m_dbName = new FileChooser( this );
	formLayout->addRow( tr( "&Db file:" ), m_dbName );
	
	m_treeWidget = new QTreeWidget( this );
	QStringList labels;
	labels << tr( "Item" ) << tr( "Type" ) << tr( "Language" ) << tr( "Version" );
	m_treeWidget->setHeaderLabels( labels );
	formLayout->addRow( m_treeWidget );
	
	m_toggleAll = new QCheckBox( this );
	formLayout->addRow( tr( "&Toggle all:" ), m_toggleAll );
	
	setLayout( formLayout );
	
	connect( m_secure, SIGNAL( stateChanged( int ) ),
		this, SLOT( toggleSecure( int ) ) );
	
	connect( m_loadModeLocalFile, SIGNAL( toggled( bool ) ),
		this, SLOT( toggleLoadMode( bool ) ) );
		
	connect( m_loadModeLocalDatabase, SIGNAL( toggled( bool ) ),
		this, SLOT( toggleLoadMode( bool ) ) );

	connect( m_loadModeNetwork, SIGNAL( toggled( bool ) ),
		this, SLOT( toggleLoadMode( bool ) ) );
}

void StorageWidget::loadSettings( )
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
	switch( prefs->loadMode( ) ) {
		case Preferences::LocalFile:
			m_loadModeLocalFile->setChecked( true );
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
	m_toggleAll->setChecked( false );
}

void StorageWidget::toggleSecure( int /* state */ )
{
	bool secure = m_secure->isChecked( );
	m_clientCertFile->setEnabled( secure );
	m_clientKeyFile->setEnabled( secure );
	m_CACertFile->setEnabled( secure );
}

void StorageWidget::toggleLoadMode( bool /* checked */ )
{
	m_dbName->setEnabled( m_loadModeLocalDatabase->isChecked( ) );
	m_host->setEnabled( m_loadModeNetwork->isChecked( ) );
	m_port->setEnabled( m_loadModeNetwork->isChecked( ) );
	m_secure->setEnabled( m_loadModeNetwork->isChecked( ) );
	m_clientCertFile->setEnabled( m_loadModeNetwork->isChecked( ) && m_secure->isChecked( ) );
	m_clientKeyFile->setEnabled( m_loadModeNetwork->isChecked( ) && m_secure->isChecked( ) );
	m_CACertFile->setEnabled( m_loadModeNetwork->isChecked( ) && m_secure->isChecked( ) );
}

void StorageWidget::loadTree( )
{
	QTreeWidgetItem *_parent = m_treeWidget->invisibleRootItem( );
	QTreeWidgetItem *item = new QTreeWidgetItem( _parent );
	item->setText( 0, "forms" );
	_parent->addChild( item );
}

} // namespace QtClient
} // namespace _Wolframe
