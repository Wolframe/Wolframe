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
#include "WidgetVisitor_QTreeWidget.hpp"
#include <QDebug>

WidgetVisitorState_QTreeWidget::WidgetVisitorState_QTreeWidget( QWidget* widget_)
	:WidgetVisitor::State(widget_)
	,m_treeWidget(qobject_cast<QTreeWidget*>(widget_))
	,m_elementname(widget_->objectName().toAscii())
	,m_mode(Init)
{
	QTreeWidgetItem* header = m_treeWidget->headerItem();
	for( int ii = 0; ii < header->columnCount(); ii++) {
		QByteArray headerText = header->data( ii, Qt::DisplayRole).toByteArray();
		m_headers << headerText;
	}
	if (m_elementname.isEmpty())
	{
		qCritical() << "empty or illegal object name defined for tree widget";
	}
	m_stk.push_back( StackElement( m_treeWidget->invisibleRootItem()));
	m_dataelements_init = DataElements( "item", 0);
	m_dataelements_init.push_back( m_elementname);
	m_dataelements_tree = DataElements( "item", "id", 0);
	m_dataelements_tree.append( m_headers);
	m_dataelements_list = DataElements( "id", 0);
	m_dataelements_list.append( m_headers);
}

void WidgetVisitorState_QTreeWidget::clear()
{
	m_treeWidget->clear();
	m_mode = Init;
	m_stk.clear();
	m_stk.push_back( StackElement( m_treeWidget->invisibleRootItem()));
}

bool WidgetVisitorState_QTreeWidget::enter( const QByteArray& name, bool writemode)
{
	if (m_mode != List && strcmp( name, "item") == 0)
	{
		m_mode = Tree;
		if (writemode)
		{
			m_stk.push_back( StackElement( new QTreeWidgetItem( m_stk.top().item)));
		}
		else
		{
			if (m_stk.top().readpos < m_stk.top().item->childCount()) return false;
			m_stk.push_back( m_stk.top().item->child( m_stk.top().readpos++));
		}
		return true;
	}
	else if (m_mode != Tree && name == m_elementname)
	{
		m_mode = List;
		if (m_stk.size() != 1)
		{
			qCritical() << "illegal header in middle of content (stack size=" << m_stk.size() << ")";
			return false;
		}
		if (writemode)
		{
			m_stk.push_back( StackElement( new QTreeWidgetItem( m_stk.top().item)));
		}
		else
		{
			if (m_stk.top().readpos < m_stk.top().item->childCount()) return false;
			m_stk.push_back( m_stk.top().item->child( m_stk.top().readpos++));
		}
		return true;
	}
	return false;
}

bool WidgetVisitorState_QTreeWidget::leave( bool /*writemode*/)
{
	if (m_stk.isEmpty()) return false;
	m_stk.pop_back();
	return true;
}

bool WidgetVisitorState_QTreeWidget::isRepeatingDataElement( const QByteArray& name)
{
	if (m_mode != List && name == "item") return true;
	if (m_mode != Tree && name == m_elementname) return true;
	return false;
}

QVariant WidgetVisitorState_QTreeWidget::property( const QByteArray& name)
{
	if (strcmp( name,"selected") == 0)
	{
		if (m_treeWidget->selectionMode() == QAbstractItemView::SingleSelection)
		{
			foreach (const QTreeWidgetItem* sel, m_treeWidget->selectedItems())
			{
				return sel->data( 0, Qt::UserRole);
			}
		}
		else
		{
			QList<QVariant> idlist;
			foreach (const QTreeWidgetItem* sel, m_treeWidget->selectedItems())
			{
				idlist.push_back( sel->data( 0, Qt::UserRole));
			}
			return QVariant( idlist);
		}
	}
	if (m_stk.isEmpty()) return QVariant();
	int col = m_headers.indexOf( name);
	if (col != -1)
	{
		return QVariant( m_stk.top().item->text( col));
	}
	if (strcmp( name,"id") == 0)
	{
		return m_stk.top().item->data( 0, Qt::UserRole);
	}
	return QVariant();
}

bool WidgetVisitorState_QTreeWidget::setProperty( const QByteArray& name, const QVariant& data)
{
	if (m_stk.isEmpty()) return false;
	int col = m_headers.indexOf( name);
	if (col != -1)
	{
		m_stk.top().item->setText( col, data.toString());
		return true;
	}
	if (strcmp( name,"id") == 0)
	{
		m_stk.top().item->setData( 0, Qt::UserRole, data);
		return true;
	}
	return false;
}

const QList<QByteArray>& WidgetVisitorState_QTreeWidget::dataelements() const
{
	static const QList<QByteArray> noDataElements;
	switch (m_mode)
	{
		case Init: return m_dataelements_init;
		case Tree: return m_dataelements_tree;
		case List: return m_dataelements_list;
	}
	return noDataElements;
}


void WidgetVisitorState_QTreeWidget::setState( const QVariant& state)
{
	qDebug() << "Restoring tree state for tree" << m_elementname;
	QList<QVariant> statelist = state.toList();
	QList<QVariant>::const_iterator stateitr = statelist.begin();
	if (stateitr != statelist.end())
	{
		int nof_expanded = stateitr->toInt();
		QList<QVariant> expanded;
		QList<QVariant> selected;

		for (++stateitr; nof_expanded > 0 && stateitr != statelist.end(); ++stateitr,--nof_expanded)
		{
			expanded.push_back( *stateitr);
		}
		for (++stateitr; stateitr != statelist.end(); ++stateitr)
		{
			selected.push_back( *stateitr);
		}

		QTreeWidgetItemIterator ei( m_treeWidget);
		for (; *ei; ++ei)
		{
			(*ei)->setExpanded( expanded.contains( (*ei)->data( 0, Qt::UserRole)));
		}
		QTreeWidgetItemIterator si( m_treeWidget);
		for (; *si; ++si)
		{
			if (selected.contains( (*si)->data( 0, Qt::UserRole)))
			{
				(*si)->setSelected( true);
				// better than nothing, scroll to the position of the last selection (usually one)
				m_treeWidget->scrollToItem( *si);
			}
			else
			{
				(*si)->setSelected( false);
			}
		}
	}
	if (m_mode == List)
	{
		for( int ii = 0; ii < m_headers.size(); ii++)
		{
			m_treeWidget->resizeColumnToContents( ii);
		}
	}
}

QVariant WidgetVisitorState_QTreeWidget::getState() const
{
	// encoded state:
	// 0: number of expanded elements
	// 1..: expanded elements
	// rest: selected elements
	QList<QVariant> rt;
	QTreeWidgetItemIterator xi( m_treeWidget);
	for (; *xi; ++xi)
	{
		if ((*xi)->isExpanded())
		{
			rt.push_back( (*xi)->data( 0, Qt::UserRole));
		}
	}
	rt.push_front( QVariant( rt.size()));
	QTreeWidgetItemIterator si( m_treeWidget);
	for (; *si; ++si)
	{
		if ((*si)->isSelected())
		{
			rt.push_back( (*xi)->data( 0, Qt::UserRole));
		}
	}
	return QVariant(rt);
}


