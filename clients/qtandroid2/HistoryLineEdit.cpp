//
// HistoryLineEdit.cpp
//

#include "HistoryLineEdit.hpp"

#include <QFile>
#include <QTextStream>

namespace _Wolframe {
	namespace QtClient {

HistoryLineEdit::HistoryLineEdit( QWidget *_parent ) : QLineEdit( _parent ),
	m_historyFilename( ".history" ), m_pos( 0 )
{
	initialize( );
}

void HistoryLineEdit::initialize( )
{
	m_history = new QStringList;

	readHistory( );
	m_pos = m_history->size( ) - 1;

	connect( this, SIGNAL( returnPressed( ) ), this, SLOT( returnPressed( ) ) );
}

HistoryLineEdit::~HistoryLineEdit( )
{
	saveHistory( );

	delete m_history;
}

void HistoryLineEdit::returnPressed( )
{
	m_history->append( text( ) );
	m_pos = m_history->size( );
	emit lineEntered( text( ) );
	clear( );
}

void HistoryLineEdit::readHistory( )
{
	QFile f( m_historyFilename );
	if( !f.exists( ) ) return;
	f.open( QFile::ReadOnly );
	QTextStream s( &f );
	QString line;
	do {
		line = s.readLine( );
		m_history->append( line );
	} while( !line.isNull( ) );
	f.close( );
}

void HistoryLineEdit::saveHistory( )
{
	QFile f( m_historyFilename );
	f.open( QFile::WriteOnly );
	QTextStream s( &f );
	for(	QStringList::iterator it = m_history->begin( );
		it != m_history->end( );
		it++ ) {
		s << *it << endl;
	}
	f.close( );
}

void HistoryLineEdit::keyPressEvent( QKeyEvent *_event )
{
	if( _event->key( ) == Qt::Key_Up ) {
		if( m_history->size( ) > 0 ) {
			m_pos--;
			if( m_pos < 0 ) m_pos = 0;
			setText( m_history->at( m_pos ) );
		}
	} else if( _event->key( ) == Qt::Key_Down ) {
		if( m_history->size( ) > 0 ) {
			m_pos++;
			if( m_pos >= m_history->size( ) ) m_pos = m_history->size( ) - 1;
			setText( m_history->at( m_pos ) );
		}
	} else {
		QLineEdit::keyPressEvent( _event );
	}
}

void HistoryLineEdit::complete( )
{
	end( false );
}

} // namespace QtClient
} // namespace _Wolframe
