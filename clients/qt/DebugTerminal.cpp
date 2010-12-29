//
// DebugTerminal.cpp
//

#include "DebugTerminal.hpp"

#include <QBoxLayout>

namespace _SMERP {
	namespace QtClient {

DebugTerminal::DebugTerminal( QWidget *_parent ) : QWidget( _parent, Qt::Tool | Qt::WindowStaysOnTopHint | Qt::WindowTitleHint )
{
	setWindowTitle( tr( "Debug Terminal" ) );

	QBoxLayout *l = new QBoxLayout( QBoxLayout::TopToBottom, this );
	m_output = new QTextEdit;
	m_output->setReadOnly( true );
	l->addWidget( m_output );

	m_input = new QLineEdit;
	l->addWidget( m_input );
	m_input->setFocus( );

	QObject::connect( m_input, SIGNAL( returnPressed( ) ), this, SLOT( returnPressed( ) ) );
}

DebugTerminal::~DebugTerminal( )
{
	delete m_output;
	delete m_input;
}

void DebugTerminal::returnPressed( )
{
	m_output->append( m_input->text( ) );
	m_input->clear( );
}

} // namespace QtClient
} // namespace _SMERP
