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
#include "WidgetListener.hpp"
#include "WidgetVisitorStateConstructor.hpp"
#include "WidgetMessageDispatcher.hpp"
#include <QAbstractButton>

bool WidgetListener::hasDataSignals( const QWidget* widget_)
{
	foreach (const QByteArray& prop, widget_->dynamicPropertyNames())
	{
		if (prop.startsWith( "datasignal:") && widget_->property( prop).isValid()) return true;
	}
	return false;
}

WidgetListener::WidgetListener( QWidget* widget_, DataLoader* dataLoader_)
	:QObject()
	,m_state(createWidgetVisitorState(widget_))
	,m_dataLoader(dataLoader_)
	,m_debug(false)
{}

void WidgetListener::handleDataSignal( WidgetVisitor::DataSignalType dt)
{
	WidgetVisitor tv( m_state);
	foreach (QWidget* receiver, tv.get_datasignal_receivers( dt))
	{
		WidgetVisitor visitor( receiver);
		visitor.readAssignments();

		QAbstractButton* button = qobject_cast<QAbstractButton*>( receiver);
		if (button)
		{
			button->toggle();
			button->click();
		}
		else if (visitor.property( "doctype").isValid())
		{
			WidgetMessageDispatcher dp( visitor);
			WidgetMessageDispatcher::Request domload = dp.getDomainLoadRequest( m_debug);
			m_dataLoader->datarequest( domload.tag, domload.content);
		}
	}
}







