//
// LoginDialog.cpp
//

#include "LoginDialog.hpp"

#include <QFormLayout>

namespace _Wolframe {
	namespace QtClient {

LoginDialog::LoginDialog( WolframeClient *_wolframeClient, QWidget *_parent ) :
	QDialog( _parent ), m_wolframeClient( _wolframeClient ),
	m_succeeded( false )
{
	initialize( );
	setWindowTitle( tr( "Login" ) );
	setModal( true );
}

void LoginDialog::initialize( )
{
	QFormLayout *formLayout = new QFormLayout( );
	
	m_mechs = new QComboBox( this );
	formLayout->addRow( tr( "&Authentication method:" ), m_mechs );		
	m_username = new QComboBox( this );
	m_username->setEditable( true );
	m_password = new QLineEdit( this );
	m_password->setEchoMode( QLineEdit::Password );
	formLayout->addRow( tr( "&Username:" ), m_username );
	formLayout->addRow( tr( "&Password:" ), m_password );
	m_buttons = new QDialogButtonBox( this );
	m_buttons->addButton( QDialogButtonBox::Ok );
	m_buttons->addButton( QDialogButtonBox::Cancel );
	m_buttons->button( QDialogButtonBox::Ok )->setText( tr( "Login" ) );
	m_buttons->button( QDialogButtonBox::Cancel )->setText( tr( "Abort" ) );
	formLayout->addRow( m_buttons );
	setLayout( formLayout );
	
	connect( m_buttons->button( QDialogButtonBox::Cancel ), SIGNAL( clicked( ) ),
		this, SLOT( close( ) ) );
		
	connect( m_buttons->button( QDialogButtonBox::Ok ), SIGNAL( clicked( ) ),
		this, SLOT( login( ) ) );	

	connect( m_wolframeClient, SIGNAL( mechsReceived( QStringList ) ),
		this, SLOT( mechsReceived( QStringList ) ) );

	connect( m_wolframeClient, SIGNAL( authOk( ) ),
		this, SLOT( authOk( ) ) );

	connect( m_wolframeClient, SIGNAL( authFailed( ) ),
		this, SLOT( authFailed( ) ) );
		
	m_wolframeClient->auth( );		
}

void LoginDialog::closeEvent( QCloseEvent *_event )
{
	qDebug( ) << "LoginDialog closeEvent" << m_succeeded;
	
	if( !m_succeeded ) {
		emit authenticationFailed( );
	}
	
	_event->accept( );
}

void LoginDialog::keyPressEvent( QKeyEvent *_event )
{
	switch( _event->key( ) ) {
		case Qt::Key_Escape:
			_event->ignore( );
			break;
			
		case Qt::Key_Enter:
		case Qt::Key_Return:
			if( m_username->hasFocus( ) ) {
				m_password->setFocus( );
			} else if( m_password->hasFocus( ) ) {
				m_buttons->button( QDialogButtonBox::Ok )->click( );
			}
			break;
		
		default:
			QDialog::keyPressEvent( _event );
	}
}

void LoginDialog::mechsReceived( QStringList mechs )
{
	m_mechList = mechs;
	m_mechs->addItems( m_mechList );
}

void LoginDialog::login( )
{
	QString mech = m_mechs->itemText( m_mechs->currentIndex( ) );
	QString username = m_username->currentText( );
	QString password = m_password->text( );
	
	m_wolframeClient->mech( mech );
}

void LoginDialog::authOk( )
{
	close( );
	emit authenticationOk( );
}

void LoginDialog::authFailed( )
{
	close( );
	emit authenticationFailed( );
}

} // namespace QtClient
} // namespace _Wolframe
