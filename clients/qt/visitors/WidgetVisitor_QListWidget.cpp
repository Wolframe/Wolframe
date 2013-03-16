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
#include "WidgetVisitor_QListWidget.hpp"
#include <QDebug>

WidgetVisitorState_QListWidget::WidgetVisitorState_QListWidget( QWidget* widget_)
	:WidgetVisitor::State(widget_)
	,m_listWidget(qobject_cast<QListWidget*>(widget_)){}

void WidgetVisitorState_QListWidget::clear()
{
	m_listWidget->clear();
}

QVariant WidgetVisitorState_QListWidget::property( const QByteArray& name)
{
	if (strcmp( name,"selected") == 0)
	{
		QList<QVariant> rt;
		foreach( QListWidgetItem *item, m_listWidget->selectedItems())
		{
			rt.append( QVariant( item->text()));
		}
		return QVariant( rt);
	}
	return QVariant();
}

bool WidgetVisitorState_QListWidget::setProperty( const QByteArray& name, const QVariant& data)
{
	if (strcmp( name,"value") == 0)
	{
		m_listWidget->addItem( data.toString());
		return true;
	}
	if (strcmp( name,"selected") == 0)
	{
		QList<QListWidgetItem *> items = m_listWidget->findItems( data.toString(), Qt::MatchExactly);
		foreach( QListWidgetItem *item, items)
		{
			item->setSelected( true);
		}
	}
	return false;
}

const QList<QByteArray>& WidgetVisitorState_QListWidget::dataelements() const
{
	static const DataElements ar( "select", 0);
	return ar;
}

void WidgetVisitorState_QListWidget::setState( const QVariant& state)
{
	qDebug() << "Restoring tree state for list widget" << m_listWidget->objectName();
	for( int i = 0; i < m_listWidget->count( ); i++ )
	{
		QListWidgetItem *item = m_listWidget->item( i );
		item->setSelected( false);
	}
	QList<QVariant> selected = state.toList();
	foreach (const QVariant& elem, selected)
	{
		QList<QListWidgetItem*> itemlist = m_listWidget->findItems( elem.toString(), Qt::MatchExactly);
		QList<QListWidgetItem*>::iterator ti = itemlist.begin(), te = itemlist.end();
		for (; ti != te; ++ti)
		{
			(*ti)->setSelected( true);
		}
	}
}

QVariant WidgetVisitorState_QListWidget::getState() const
{
	QList<QVariant> selected;
	foreach (const QListWidgetItem* item, m_listWidget->selectedItems())
	{
		selected.push_back( QVariant( item->text()));
	}
	return QVariant(selected);
}



