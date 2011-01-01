//
// DebugTerminal.cpp
//

#include "DebugTerminal.hpp"

#include <QBoxLayout>
#include <QTextStream>
#include <QCompleter>

namespace _SMERP {
	namespace QtClient {

DebugTerminal::DebugTerminal( QWidget *_parent ) : QWidget( _parent, Qt::Tool | Qt::WindowStaysOnTopHint | Qt::WindowTitleHint )
{
	initialize( );
}

void DebugTerminal::initialize( )
{
	setWindowTitle( tr( "Debug Terminal" ) );

	QBoxLayout *l = new QBoxLayout( QBoxLayout::TopToBottom, this );
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
}

DebugTerminal::~DebugTerminal( )
{
	delete m_input;
	delete m_output;
}

void DebugTerminal::lineEntered( QString line )
{
	m_output->append( line );
}

} // namespace QtClient
} // namespace _SMERP
