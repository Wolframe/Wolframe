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
#include "FormWidget.hpp"
#include <QAbstractButton>
#include <QAbstractScrollArea>
#include <QMenu>
#include <QDebug>

WidgetListener::~WidgetListener()
{
}

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
{
	if (widget_->property( "contextmenu").isValid())
	{
		widget_->setContextMenuPolicy( Qt::CustomContextMenu);
		connect( widget_, SIGNAL( customContextMenuRequested( const QPoint&)),
			this, SLOT( showContextMenu( const QPoint&)));
	}
}

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
		else if (receiver->property( "doctype").isValid())
		{
			WidgetMessageDispatcher dp( visitor);
			WidgetRequest domload = dp.getDomainLoadRequest( m_debug);
			if (!domload.content.isEmpty())
			{
				m_dataLoader->datarequest( domload.tag, domload.content);
			}
		}
		else if (receiver->property( "action").isValid())
		{
			WidgetMessageDispatcher dp( visitor);
			WidgetRequest domload = dp.getDomainLoadRequest( m_debug);
			if (!domload.content.isEmpty())
			{
				m_dataLoader->datarequest( domload.tag, domload.content);
			}
		}
	}
}

void WidgetListener::showContextMenu( const QPoint& pos)
{
	QPoint globalPos;
	QWidget* widget = m_state->widget();
	if (qobject_cast<QAbstractScrollArea*>(widget))
	{
		QAbstractScrollArea* as = qobject_cast<QAbstractScrollArea*>( widget);
		globalPos = as->viewport()->mapToGlobal( pos);
	}
	else
	{
		globalPos = widget->mapToGlobal( pos);
	}
	QMenu menu;
	WidgetVisitor visitor( m_state);
	QVariant contextmenudef_p( visitor.property( "contextmenu"));
	QList<QString> contextmenudef( contextmenudef_p.toString().split(','));

	foreach (const QString& item, contextmenudef)
	{
		if (item.trimmed().isEmpty())
		{
			menu.addSeparator();
		}
		else
		{
			QList<QString> action_text = item.split(':');
			if (action_text.size() == 1)
			{
				QString itemtext( action_text.at(0).trimmed());
				QAction* action = menu.addAction( itemtext);
				action->setData( QVariant( itemtext));
			}
			else if (action_text.size() == 2)
			{
				QAction* action = menu.addAction( action_text.at(1).trimmed());
				action->setData( QVariant( action_text.at(0).trimmed()));
			}
			else
			{
				qCritical() << "error in menu definition. more than one ':' for" << item;
			}
		}
	}
	QAction* selectedItem = menu.exec( globalPos);
	if (selectedItem)
	{
		FormWidget* form = visitor.formwidget();
		if (!form)
		{
			qCritical() << "no form associated with widget woth context menu action";
			return;
		}
		QVariant action = selectedItem->data();
		if (!action.isValid())
		{
			qCritical() << "no data associated context menu action";
			return;
		}
		form->executeMenuAction( widget, action.toString());
	}
}





