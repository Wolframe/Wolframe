//
// DebugTerminal.cpp
//

#include "DebugTerminal.hpp"

#include <QBoxLayout>
#include <QFile>
#include <QTextStream>

namespace _SMERP {
	namespace QtClient {

DebugTerminal::DebugTerminal( QWidget *_parent ) : QWidget( _parent, Qt::Tool | Qt::WindowStaysOnTopHint | Qt::WindowTitleHint ),
	m_historyFilename( ".history" )
{
	initialize( );
	readHistory( );
}

void DebugTerminal::initialize( )
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

	m_inputHistory = new QStringList;
}

DebugTerminal::~DebugTerminal( )
{
	saveHistory( );

	delete m_inputHistory;
	delete m_input;
	delete m_output;
}

void DebugTerminal::returnPressed( )
{
	m_output->append( m_input->text( ) );
	m_inputHistory->append( m_input->text( ) );
	m_input->clear( );
}

void DebugTerminal::readHistory( )
{
	QFile f( m_historyFilename );
	if( !f.exists( ) ) return;
	f.open( QFile::ReadOnly );
	QTextStream s( &f );
	QString line;
	do {
		line = s.readLine( );
		m_inputHistory->append( line );
	} while( !line.isNull( ) );
	f.close( );
}

void DebugTerminal::saveHistory( )
{
	QFile f( m_historyFilename );
	f.open( QFile::WriteOnly );
	QTextStream s( &f );
	for(	QStringList::iterator it = m_inputHistory->begin( );
		it != m_inputHistory->end( );
		it++ ) {
		s << *it << endl;
	}
	f.close( );
}

} // namespace QtClient
} // namespace _SMERP
