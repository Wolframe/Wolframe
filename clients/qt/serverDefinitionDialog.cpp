#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>

#include "serverDefinitionDialog.hpp"
#ifdef WITH_SSL
#include "ui_serverDefinitionDialog.h"
#else
#include "ui_serverDefinitionDialogNoSSL.h"
#endif

#include "connection.hpp"
#include "WolframeClient.hpp"

ServerDefinitionDialog::ServerDefinitionDialog( ConnectionParameters& params, QWidget *_parent ) :
	QDialog( _parent ), ui( new Ui::ServerDefinitionDialog ), m_params( params )
{
	ui->setupUi( this );
	if ( !params.isEmpty() )	{
		ui->nameEdit->setText( params.name );
	}

	ui->nameEdit->setText( params.name );
	ui->hostEdit->setText( params.host );
	if ( params.port == 0 )
		ui->portSpinBox->setValue( 7661 );
	else
		ui->portSpinBox->setValue( params.port );

#ifdef WITH_SSL
	if ( params.SSL )	{
		ui->SSLcheck->setChecked( true );
		if ( params.clientCertificate )	{
			ui->clientCertCheck->setChecked( true );
			ui->certEdit->setText( params.SSLcertificate );
			ui->keyEdit->setText( params.SSLkey );
		}
		else	{
			ui->clientCertCheck->setChecked( false );
			ui->certEdit->setText( "" );
			ui->keyEdit->setText( "" );
		}
		if ( params.SSLverify )	{
			ui->SSLverifyCheck->setChecked( true );
			ui->CAbundleEdit->setText( params.SSLCAbundle );
		}
		else	{
			ui->SSLverifyCheck->setChecked( false );
			ui->CAbundleEdit->setText( "" );
		}
	}
	else	{
		ui->SSLcheck->setChecked( false );
		ui->clientCertCheck->setChecked( false );
		ui->certEdit->setText( "" );
		ui->keyEdit->setText( "" );
		ui->SSLverifyCheck->setChecked( false );
		ui->CAbundleEdit->setText( "" );
	}
#endif

	if ( params.timeout == 0 )
		ui->timeoutSpinBox->setValue( 15 );
	else
		ui->timeoutSpinBox->setValue( params.timeout );

#ifdef WITH_SSL
	connect( ui->certFileBttn, SIGNAL( clicked() ), this, SLOT( selectCertFile() ));
	connect( ui->keyFileBttn, SIGNAL( clicked() ), this, SLOT( selectKeyFile() ));
	connect( ui->CAbundleFileBttn, SIGNAL( clicked() ), this, SLOT( selectCAbundleFile() ));
	connect( ui->SSLcheck, SIGNAL( clicked() ), this, SLOT( SSLtoggle() ));
	connect( ui->clientCertCheck, SIGNAL( clicked() ), this, SLOT( updateSSLfields() ));
	connect( ui->SSLverifyCheck, SIGNAL( clicked() ), this, SLOT( updateSSLfields() ));
#endif
	connect( ui->testBttn, SIGNAL( clicked() ), this, SLOT( testConnection() ));
	connect( ui->nameEdit, SIGNAL( textChanged( QString )), this, SLOT( updateButtons() ));
	connect( ui->hostEdit, SIGNAL( textChanged( QString )), this, SLOT( updateButtons() ));
#ifdef WITH_SSL
	updateSSLfields();
#endif
	updateButtons();
}

ServerDefinitionDialog::~ServerDefinitionDialog()
{
	delete ui;
}

void ServerDefinitionDialog::updateSSLfields()
{
#ifdef WITH_SSL
	if ( ui->SSLcheck->isChecked() )	{
		ui->clientCertCheck->setEnabled( true );
		if ( ui->clientCertCheck->isChecked() )	{
			ui->certificateLbl->setEnabled( true );
			ui->certEdit->setEnabled( true );
			ui->certFileBttn->setEnabled( true );
			ui->keyLbl->setEnabled( true );
			ui->keyEdit->setEnabled( true );
			ui->keyFileBttn->setEnabled( true );
		}
		else	{
			ui->certificateLbl->setEnabled( false );
			ui->certEdit->setEnabled( false );
			ui->certFileBttn->setEnabled( false );
			ui->keyLbl->setEnabled( false );
			ui->keyEdit->setEnabled( false );
			ui->keyFileBttn->setEnabled( false );
		}
		ui->SSLverifyCheck->setEnabled( true );
		if ( ui->SSLverifyCheck->isChecked() )	{
			ui->CAbundleLbl->setEnabled( true );
			ui->CAbundleEdit->setEnabled( true );
			ui->CAbundleFileBttn->setEnabled( true );
		}
		else	{
			ui->CAbundleLbl->setEnabled( false );
			ui->CAbundleEdit->setEnabled( false );
			ui->CAbundleFileBttn->setEnabled( false );
		}
	}
	else	{
		ui->clientCertCheck->setEnabled( false );
		ui->certificateLbl->setEnabled( false );
		ui->certEdit->setEnabled( false );
		ui->certFileBttn->setEnabled( false );
		ui->keyLbl->setEnabled( false );
		ui->keyEdit->setEnabled( false );
		ui->keyFileBttn->setEnabled( false );
		ui->SSLverifyCheck->setEnabled( false );
		ui->CAbundleLbl->setEnabled( false );
		ui->CAbundleEdit->setEnabled( false );
		ui->CAbundleFileBttn->setEnabled( false );
	}
#endif
}

void ServerDefinitionDialog::updateButtons()
{
	if ( ui->hostEdit->text().isEmpty() || ui->portSpinBox->value() <= 0 )	{
		ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled( false );
		ui->testBttn->setEnabled( false );
	}
	else	{
		ui->testBttn->setEnabled( true );
		if ( ui->nameEdit->text().isEmpty() )
			ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled( false );
		else
			ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled( true );
	}
}

void ServerDefinitionDialog::buildParams( ConnectionParameters& params )
{
	params.name = ui->nameEdit->text().simplified();
	params.host = ui->hostEdit->text().simplified();
	params.port = ui->portSpinBox->value();
	params.timeout = ui->timeoutSpinBox->value();
#ifdef WITH_SSL
	if ( ui->SSLcheck->isChecked() )	{
		params.SSL = true ;
		if ( ui->clientCertCheck->isChecked() )	{
			params.clientCertificate = true;
			params.SSLcertificate = ui->certEdit->text();
			params.SSLkey = ui->keyEdit->text();
		}
		else	{
			params.clientCertificate = false;
			params.SSLcertificate.clear();
			params.SSLkey.clear();
		}
		if ( ui->SSLverifyCheck->isChecked() )	{
			params.SSLverify = true;
			params.SSLCAbundle = ui->CAbundleEdit->text();
		}
		else	{
			params.SSLverify = false;
			params.SSLCAbundle.clear();
		}
	}
	else	{
		params.SSL = false ;
		params.clientCertificate = false;
		params.SSLcertificate.clear();
		params.SSLkey.clear();
		params.SSLverify = false;
		params.SSLCAbundle.clear();
	}
#else
	params.SSL = false ;
	params.clientCertificate = false;
	params.SSLcertificate.clear();
	params.SSLkey.clear();
	params.SSLverify = false;
	params.SSLCAbundle.clear();
#endif
}

void ServerDefinitionDialog::done( int retCode )
{
	if ( retCode == QDialog::Accepted )	{
		buildParams( m_params );
		QString	_error;
		if ( ! m_params.check( _error ))
			QMessageBox::critical( this, tr( "Parameters error"), _error );
		else
			QDialog::done( retCode );
	}
	else
		QDialog::done( retCode );
}


void ServerDefinitionDialog::SSLtoggle()
{
#ifdef WITH_SSL
	if ( ui->SSLcheck->isChecked() )	{
		if ( ui->portSpinBox->value() == 7661 )
			ui->portSpinBox->setValue( 7961 );
	}
	else	{
		if ( ui->portSpinBox->value() == 7961 )
			ui->portSpinBox->setValue( 7661 );
	}
	updateSSLfields();
#endif
}


void ServerDefinitionDialog::selectCertFile()
{
#ifdef WITH_SSL
	QString fileName = QFileDialog::getOpenFileName( this, "Select SSL certificate",
							 m_currentDir, "Certificate files (*.crt *.cert *.pem);;All files (*.*)" );
	if ( !fileName.isEmpty() )	{
		QDir dir = QFileInfo( fileName ).absoluteDir();
		m_currentDir = dir.absolutePath();
		ui->certEdit->setText( fileName );
	}
#endif
}

void ServerDefinitionDialog::selectKeyFile()
{
#ifdef WITH_SSL
	QString fileName = QFileDialog::getOpenFileName( this, "Select SSL key",
							 m_currentDir, "Key files (*.key *.pem);;All files (*.*)" );
	if ( !fileName.isEmpty() )	{
		QDir dir = QFileInfo( fileName ).absoluteDir();
		m_currentDir = dir.absolutePath();
		ui->keyEdit->setText( fileName );
	}
#endif
}

void ServerDefinitionDialog::selectCAbundleFile()
{
#ifdef WITH_SSL
	QString fileName = QFileDialog::getOpenFileName( this, "Select SSL CA bundle",
							 m_currentDir, "CA bundle files (*.crt *.cert *.pem);;All files (*.*)" );
	if ( !fileName.isEmpty() )	{
		QDir dir = QFileInfo( fileName ).absoluteDir();
		m_currentDir = dir.absolutePath();
		ui->CAbundleEdit->setText( fileName );
	}
#endif
}


void ServerDefinitionDialog::testConnection()
{
	buildParams( m_params );
	QString	error;
	if ( ! m_params.check( error ))	{
		QMessageBox::critical( this, tr( "Parameters error"), error );
		return;
	}

	ui->testBttn->setEnabled( false );

	ConnectionParameters testParms;
	buildParams( testParms );

	m_client = new WolframeClient( testParms );

	connect( m_client, SIGNAL( error( QString ) ),
		this, SLOT( error( QString ) ) );
	connect( m_client, SIGNAL( connected( ) ),
		this, SLOT( connected( ) ) );
	connect( m_client, SIGNAL( disconnected( ) ),
		this, SLOT( disconnected( ) ) );
		
	m_client->connect( );
}

void ServerDefinitionDialog::error( QString error )
{
	QMessageBox::critical( this, tr( "Connection error"), error );
	ui->testBttn->setEnabled( true );
	m_client->deleteLater( );
}

void ServerDefinitionDialog::connected( )
{
	disconnect( m_client, SIGNAL( error( QString ) ), 0, 0 );
	QMessageBox::information( this, tr( "Testing connection"), "Connection successful" );
	ui->testBttn->setEnabled( true );
	m_client->disconnect( );
}

void ServerDefinitionDialog::disconnected( )
{
	ui->testBttn->setEnabled( true );
	m_client->deleteLater( );
}
