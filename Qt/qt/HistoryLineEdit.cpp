/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
 All rights reserved.

 This file is part of Project Wolframe.

 Commercial Usage
    Licensees holding valid Project Wolframe Commercial licenses may
    use this file in accordance with the Project Wolframe
    Commercial License Agreement provided with the Software or,
    alternatively, in accordance with the terms contained
    in a written agreement between the licensee and Project Wolframe.

 GNU General Public License Usage
    Alternatively, you can redistribute this file and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Wolframe is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/

#include "HistoryLineEdit.hpp"

#include <QFile>
#include <QTextStream>


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

