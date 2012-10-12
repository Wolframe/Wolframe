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

namespace _Wolframe {
	namespace QtClient {

DebugTerminal::DebugTerminal( WolframeClient *_wolframeClient, QWidget *_parent ) :
	QWidget( _parent, Qt::Tool | Qt::WindowTitleHint ),
	m_wolframeClient( _wolframeClient )
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
	wordList << "connect" << "quit" << "caps" << "help";
#ifdef WITH_SSL
	wordList << "sconnect";
#endif
	QCompleter *completer = new QCompleter( wordList, this );
	completer->setCaseSensitivity( Qt::CaseInsensitive );
	completer->setCompletionMode( QCompleter::InlineCompletion );
	m_input->setCompleter( completer );

	connect( m_input, SIGNAL( lineEntered( QString ) ), this, SLOT( lineEntered( QString ) ) );

	connect( m_wolframeClient, SIGNAL( error( QString ) ), this, SLOT( networkError( QString ) ) );
	connect( m_wolframeClient, SIGNAL( lineReceived( QString ) ), this, SLOT( lineReceived( QString ) ) );
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

void DebugTerminal::sendLine( QString line )
{
	m_output->setTextColor( QColor( "grey" ) );
	m_output->append( line );
	m_output->setTextColor( QColor( "black" ) );
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
			m_wolframeClient->setHost( host );
			m_wolframeClient->setPort( port );
#ifdef WITH_SSL
			m_wolframeClient->setSecure( line.toLower( ).startsWith( "sconnect" ) );
#endif
			m_wolframeClient->connect( );
		} else {
			m_output->setTextColor( QColor( "red" ) );
			m_output->append( "illegal connect parameters, expecting '(s)connect <host>:<port>'.." );
			m_output->setTextColor( QColor( "black" ) );
		}
	} else if( line.toLower( ).startsWith( "quit" ) ) {
		m_wolframeClient->disconnect( );
	} else if( line.toLower( ).startsWith( "help" ) ) {
		m_output->setTextColor( QColor( "blue" ) );
		m_output->append( "HELP - show this help page" );
		m_output->append( "CONNECT host:port - connect to Wolframe server (insecure)" );
#ifdef WITH_SSL
		m_output->append( "SCONNECT host:port - connect to Wolframe server (secure)" );
#endif
		m_output->append( "QUIT - terminate connection to Wolframe server" );
		m_output->setTextColor( QColor( "black" ) );
	} else if( line.toLower( ).startsWith( "caps" ) ) {
		m_output->setTextColor( QColor( "blue" ) );
		m_output->append( "OK QUIT CAPS" );
		m_output->setTextColor( QColor( "black" ) );
	} else {
		m_wolframeClient->sendLine( line );
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
} // namespace _Wolframe
