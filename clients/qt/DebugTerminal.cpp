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

#include "DebugTerminal.hpp"

#include <QBoxLayout>
#include <QLabel>
#include <QTextStream>
#include <QCompleter>
#include <QColor>
#include <QRegExp>


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

	QLabel *label = new QLabel( tr( "Ctrl-Alt-D enables/disables the protocol debug window" ), this );
	l->addWidget( label );

	m_output = new QTextEdit( this );
	m_output->setReadOnly( true );
	l->addWidget( m_output );

	m_input = new HistoryLineEdit( this );
	l->addWidget( m_input );
	m_input->setFocus( );

	QStringList wordList;
	wordList << "connect" << "quit" << "help";
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
	m_output->setTextColor( QColor( "green" ) );
	m_output->append( line );
	m_output->setTextColor( QColor( "black" ) );
}

void DebugTerminal::sendComment( QString line )
{
	m_output->setTextColor( QColor( "grey" ) );
	m_output->append( line );
	m_output->setTextColor( QColor( "black" ) );
}

void DebugTerminal::lineEntered( QString line )
{
	if( line.toLower( ).startsWith( "connect" ) ||
		line.toLower( ).startsWith( "sconnect" ) ) {
		QRegExp rx( "^s?connect\\s+([^:]+):(\\d+)$", Qt::CaseInsensitive );
		QStringList m;
		if( rx.indexIn( line ) != -1 ) {
			//~ QString host = rx.cap( 1 );
			//~ unsigned short port = rx.cap( 2 ).toUShort( );
			//~ m_wolframeClient->setHost( host );
			//~ m_wolframeClient->setPort( port );
//~ #ifdef WITH_SSL
			//~ m_wolframeClient->setSecure( line.toLower( ).startsWith( "sconnect" ) );
//~ #endif
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

