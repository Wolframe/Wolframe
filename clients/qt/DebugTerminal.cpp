//
// DebugTerminal.cpp
//

#include "DebugTerminal.hpp"

#include <QBoxLayout>
#include <QLabel>
#include <QTextStream>
#include <QCompleter>
#include <QColor>

namespace _SMERP {
	namespace QtClient {

DebugTerminal::DebugTerminal( SMERPClient *_smerpClient, QWidget *_parent ) :
	QWidget( _parent, Qt::Tool | Qt::WindowStaysOnTopHint | Qt::WindowTitleHint ),
	m_smerpClient( _smerpClient )
{
	initialize( );
}

void DebugTerminal::initialize( )
{
	setWindowTitle( tr( "Debug Terminal" ) );

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

void DebugTerminal::lineEntered( QString line )
{
	m_output->append( line );

	if( line.toLower( ).startsWith( "connect" ) ) {
		QString host = "localhost";
		unsigned int port = 7661;
		m_smerpClient->setHost( host );
		m_smerpClient->setPort( port );
		m_smerpClient->connect( );
	} else if( line.toLower( ).startsWith( "quit" ) ) {
		m_smerpClient->disconnect( );
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
