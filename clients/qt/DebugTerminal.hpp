/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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

#ifndef _DEBUG_TERMINAL_HPP_INCLUDED
#define _DEBUG_TERMINAL_HPP_INCLUDED

#include <QWidget>
#include <QTextEdit>

#include "HistoryLineEdit.hpp"
#include "WolframeClient.hpp"


	class DebugTerminal : public QWidget
	{
	Q_OBJECT

	private:
		QTextEdit *m_output;
		HistoryLineEdit *m_input;
		WolframeClient *m_wolframeClient;

	public:
		DebugTerminal( WolframeClient *_wolframeClient, QWidget *_parent = 0 );
		virtual ~DebugTerminal( );
		void bringToFront( );

	public slots:
		void sendLine( QString line );
		void sendComment( QString line );
	
	private:
		void initialize( );

	protected:
		virtual bool focusNextPrevChild( bool next );

	private slots:
		void lineEntered( QString line );
		void networkError( QString error );
		void lineReceived( QString line );

	};

#endif // _DEBUG_TERMINAL_HPP_INCLUDED
