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
	m_uiLoadModeLocalFile->setChecked( false );
	m_uiLoadModeLocalDatabase->setChecked( false );
	m_uiLoadModeNetwork->setChecked( false );
	m_dataLoadModeLocalFile->setChecked( false );
	m_dataLoadModeLocalDatabase->setChecked( false );
	m_dataLoadModeNetwork->setChecked( false );
	m_dbName->setEnabled( false );
	m_uiFormsDir->setEnabled( false );
	m_uiFormTranslationsDir->setEnabled( false );
	m_dataLoaderDir->setEnabled( false );
	switch( prefs->uiLoadMode( ) ) {
		case Preferences::LocalFile:			
			m_uiLoadModeLocalFile->setChecked( true );
			m_uiFormsDir->setEnabled( true );
			m_uiFormTranslationsDir->setEnabled( true );
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
	m_toggleAll->setChecked( false );
	m_uiFormsDir->setFileName( prefs->uiFormsDir( ) );
	m_uiFormTranslationsDir->setFileName( prefs->uiFormTranslationsDir( ) );
	m_dataLoaderDir->setFileName( prefs->dataLoaderDir( ) );
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
	m_dbName->setEnabled( m_uiLoadModeLocalDatabase->isChecked( ) || m_dataLoadModeLocalDatabase->isChecked( ) );
	m_host->setEnabled( m_uiLoadModeNetwork->isChecked( ) || m_dataLoadModeNetwork->isChecked( ) );
	m_port->setEnabled( m_uiLoadModeNetwork->isChecked( ) | m_dataLoadModeNetwork->isChecked( ) );
	m_secure->setEnabled( m_uiLoadModeNetwork->isChecked( ) || m_dataLoadModeNetwork->isChecked( ) );
	m_clientCertFile->setEnabled( ( m_uiLoadModeNetwork->isChecked( ) || m_dataLoadModeNetwork->isChecked( ) ) && m_secure->isChecked( ) );
	m_clientKeyFile->setEnabled( ( m_uiLoadModeNetwork->isChecked( ) || m_dataLoadModeNetwork->isChecked( ) ) && m_secure->isChecked( ) );
	m_CACertFile->setEnabled( ( m_uiLoadModeNetwork->isChecked( ) || m_dataLoadModeNetwork->isChecked( ) ) && m_secure->isChecked( ) );
	m_uiFormsDir->setEnabled( m_uiLoadModeLocalFile->isChecked( ) );
	m_uiFormTranslationsDir->setEnabled( m_uiLoadModeLocalFile->isChecked( ) );
	m_dataLoaderDir->setEnabled( m_dataLoadModeLocalFile->isChecked( ) );
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
