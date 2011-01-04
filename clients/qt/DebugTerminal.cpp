//
// DebugTerminal.cpp
//

#include "DebugTerminal.hpp"

#include <QBoxLayout>
#include <QLabel>
#include <QTextStream>
#include <QCompleter>
#include <QColor>
#include <QRegExp>

namespace _SMERP {
	namespace QtClient {

DebugTerminal::DebugTerminal( SMERPClient *_smerpClient, QWidget *_parent ) :
	QWidget( _parent, Qt::Tool | Qt::WindowTitleHint ),
	m_smerpClient( _smerpClient )
{
	initialize( );
}

void DebugTerminal::initialize( )
{
	setWindowTitle( tr( "Debug Terminal" ) );
	setFixedSize( 640, 480 );

	QBoxLayout *l = new QBoxLayout( QBoxLayout::TopToBottom, this );

	QLabel *label = new QLabel( tr( "Ctrl-D enables/disables the protocol debug window" ), this );
	l->addWidget( label );

	m_output = new QTextEdit( this );
	m_output->setReadOnly( true );
	l->addWidget( m_output );

	m_input = new HistoryLineEdit( this );
	l->addWidget( m_input );
	m_input->setFocus( );

	QStringList wordList;
	wordList << "connect" << "sconnect" << "quit" << "caps" << "help";

	QCompleter *completer = new QCompleter( wordList, this );
	completer->setCaseSensitivity( Qt::CaseInsensitive );
	completer->setCompletionMode( QCompleter::InlineCompletion );
	m_input->setCompleter( completer );

	QObject::connect( m_input, SIGNAL( lineEntered( QString ) ), this, SLOT( lineEntered( QString ) ) );

	QObject::connect( m_smerpClient, SIGNAL( error( QString ) ), this, SLOT( networkError( QString ) ) );
	QObject::connect( m_smerpClient, SIGNAL( lineReceived( QString ) ), this, SLOT( lineReceived( QString ) ) );
}

DebugTerminal::~DebugTerminal( )
{
	delete m_input;
	delete m_output;
}

void DebugTerminal::bringToFront( )
{
	show( );
	activateWindow( );
	m_input->setFocus( );
}

void DebugTerminal::lineEntered( QString line )
{
	m_output->append( line );

	if( line.toLower( ).startsWith( "connect" ) ||
		line.toLower( ).startsWith( "sconnect" ) ) {
		QRegExp rx( "^s?connect\\s+([^:]+):(\\d+)$", Qt::CaseInsensitive );
		QStringList m;
		if( rx.indexIn( line ) != -1 ) {
			QString host = rx.cap( 1 );
			unsigned short port = rx.cap( 2 ).toUShort( );
			m_smerpClient->setHost( host );
			m_smerpClient->setPort( port );
			m_smerpClient->setSecure( line.toLower( ).startsWith( "sconnect" ) );
			m_smerpClient->connect( );
		} else {
			m_output->setTextColor( QColor( "red" ) );
			m_output->append( "illegal connect parameters, expecting '(s)connect <host>:<port>'.." );
			m_output->setTextColor( QColor( "black" ) );
		}
	} else if( line.toLower( ).startsWith( "quit" ) ) {
		m_smerpClient->disconnect( );
	} else if( line.toLower( ).startsWith( "help" ) ) {
		m_output->setTextColor( QColor( "blue" ) );
		m_output->append( "HELP - show this help page" );
		m_output->append( "CONNECT host:port - connect to SMERP server (insecure)" );
		m_output->append( "SCONNECT host:port - connect to SMERP server (secure)" );
		m_output->append( "QUIT - terminate connection to SMERP server" );
		m_output->setTextColor( QColor( "black" ) );
	} else if( line.toLower( ).startsWith( "caps" ) ) {
		m_output->setTextColor( QColor( "blue" ) );
		m_output->append( "OK QUIT CAPS" );
		m_output->setTextColor( QColor( "black" ) );
	} else {
		m_smerpClient->sendLine( line );
	}
}

bool DebugTerminal::focusNextPrevChild( bool next )
{
	if( next ) {
		m_input->complete( );
		return false;
	}
	return true;
}

void DebugTerminal::networkError( QString error )
{
	m_output->setTextColor( QColor( "red" ) );
	m_output->append( error );
	m_output->setTextColor( QColor( "black" ) );
}

void DebugTerminal::lineReceived( QString line )
{
	m_output->setTextColor( QColor( "blue" ) );
	m_output->append( line );
	m_output->setTextColor( QColor( "black" ) );
}

} // namespace QtClient
} // namespace _SMERP
