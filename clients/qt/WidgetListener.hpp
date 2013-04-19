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

#ifndef _WIDGET_LISTENER_HPP_INCLUDED
#define _WIDGET_LISTENER_HPP_INCLUDED
#include "WidgetVisitor.hpp"
#include "DataLoader.hpp"
#include <QObject>
#include <QWidget>
#include <QSharedPointer>

///\class WidgetListener
///\brief Structure to redirect Qt widget signals to widget visitor events
class WidgetListener :public QObject
{
	Q_OBJECT
public:
	///\brief Function to check, if data signal lister has to be created for a widget
	static bool hasDataSignals( const QWidget* widget_);

public:
	///\brief Constructor
	WidgetListener( QWidget* widget_, DataLoader* dataLoader_);
	virtual ~WidgetListener();

	void handleDataSignal( WidgetVisitor::DataSignalType dt);
	void setDebug( bool v)	{m_debug=v;}

public slots:
	void changed()			{handleDataSignal( WidgetVisitor::SigChanged);}
	void activated()		{handleDataSignal( WidgetVisitor::SigActivated);}
	void entered()			{handleDataSignal( WidgetVisitor::SigEntered);}
	void pressed()			{handleDataSignal( WidgetVisitor::SigPressed);}
	void clicked()			{handleDataSignal( WidgetVisitor::SigClicked);}
	void doubleclicked()		{handleDataSignal( WidgetVisitor::SigDoubleClicked);}

	void showContextMenu( const QPoint& pos);

private:
	WidgetVisitor::StateR m_state;
	DataLoader* m_dataLoader;
	bool m_debug;
};

typedef QSharedPointer<WidgetListener> WidgetListenerR;

#endif

