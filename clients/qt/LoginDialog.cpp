//
// LoginDialog.cpp
//

#include "LoginDialog.hpp"

#include <QFormLayout>

namespace _Wolframe {
	namespace QtClient {

LoginDialog::LoginDialog( WolframeClient *_wolframeClient, QWidget *_parent ) :
	QDialog( _parent ), m_wolframeClient( _wolframeClient ), m_succeeded( false )
{
	initialize( );
	setWindowTitle( tr( "Login" ) );
	setModal( true );
}

void LoginDialog::initialize( )
{
	QFormLayout *formLayout = new QFormLayout( );
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
	if( _event->key( ) == Qt::Key_Escape ) {
		_event->ignore( );
	}
}

void LoginDialog::login( )
{
	QString username = m_username->currentText( );
	QString password = m_password->text( );
	
	// TODO
	m_succeeded = m_wolframeClient->syncLogin( username, password );
	
	close( );
	
	if( m_succeeded ) {
		emit authenticationOk( );
	} else {
		emit authenticationFailed( );
	}
}

} // namespace QtClient
} // namespace _Wolframe
