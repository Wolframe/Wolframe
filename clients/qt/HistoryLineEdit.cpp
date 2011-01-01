//
// HistoryLineEdit.cpp
//

#include "HistoryLineEdit.hpp"

#include <QFile>
#include <QTextStream>

namespace _SMERP {
	namespace QtClient {

HistoryLineEdit::HistoryLineEdit( QWidget *_parent ) : QLineEdit( _parent ),
	m_historyFilename( ".history" )
{
	initialize( );
	readHistory( );
}

void HistoryLineEdit::initialize( )
{
	m_history = new QStringList;

	QObject::connect( this, SIGNAL( returnPressed( ) ), this, SLOT( returnPressed( ) ) );
}

HistoryLineEdit::~HistoryLineEdit( )
{
	saveHistory( );

	delete m_history;
}

void HistoryLineEdit::returnPressed( )
{
	m_history->append( text( ) );
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

} // namespace QtClient
} // namespace _SMERP
