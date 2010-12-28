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
	m_textEdit = new QTextEdit;

	QBoxLayout *l = new QBoxLayout( QBoxLayout::TopToBottom, this );
	l->addWidget( m_textEdit );
}

DebugTerminal::~DebugTerminal( )
{
}

} // namespace QtClient
} // namespace _SMERP
