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
	m_dataelements_init = DataElements( "tree", 0);
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
}

bool WidgetVisitorState_QTreeWidget::enter( const QByteArray& name, bool writemode)
{
	if (strcmp( name, "tree") == 0)
	{
		if (!writemode && m_stk.top().readpos < m_stk.top().item->childCount()) return false;
		if (m_mode != Init) return false;

		m_stk.push_back( StackElement( m_stk.top().item));
		m_mode = Tree;
		return true;
	}
	else if (m_mode == Tree && strcmp( name, "item") == 0)
	{
		if (writemode)
		{
			m_stk.push_back( StackElement( new QTreeWidgetItem( m_stk.top().item)));
			return true;
		}
		else if (m_stk.top().readpos < m_stk.top().item->childCount())
		{
			m_stk.push_back( m_stk.top().item->child( m_stk.top().readpos++));
			return true;
		}
	}
	else if (name == m_elementname)
	{
		if (!writemode && m_stk.top().readpos < m_stk.top().item->childCount()) return false;
		if (m_mode == Tree) return false;
		if (m_stk.size() != 1)
		{
			qCritical() << "illegal header in middle of content";
			return false;
		}
		m_mode = List;
		if (writemode)
		{
			m_stk.push_back( StackElement( new QTreeWidgetItem( m_stk.top().item)));
			return true;
		}
		else
		{
			m_stk.push_back( m_stk.top().item->child( m_stk.top().readpos++));
			return true;
		}
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
	if (m_mode == Tree && name == "item") return true;
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
	if (m_mode == Init || (m_mode == Tree && m_stk.size() == 1)) return QVariant();
	if (strcmp( name,"id") == 0)
	{
		return m_stk.top().item->data( 0, Qt::UserRole);
	}
	int col = m_headers.indexOf( name);
	if (col != -1)
	{
		return QVariant( m_stk.top().item->text( col));
	}
	return QVariant();
}

bool WidgetVisitorState_QTreeWidget::setProperty( const QByteArray& name, const QVariant& data)
{
	if (m_stk.isEmpty()) return false;
	if (m_mode == Init || (m_mode == Tree && m_stk.size() == 1)) return false;

	if (strcmp( name,"id") == 0)
	{
		m_stk.top().item->setData( 0, Qt::UserRole, data);
		return true;
	}
	int col = m_headers.indexOf( name);
	if (col != -1)
	{
		m_stk.top().item->setText( col, data.toString());
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


typedef void (*UnfoldProc)( QTreeWidget* tree, QTreeWidgetItem* item);
void unfoldProc_SetExpanded( QTreeWidget* /*tree*/, QTreeWidgetItem* item)
{
	item->setExpanded( true);
}
void unfoldProc_SetSelected( QTreeWidget* tree, QTreeWidgetItem* item)
{
	item->setSelected( true);
	// better than nothing, scroll to the position of the last selection (usually one)
	tree->scrollToItem( item);
}

static void unfoldNodes( QTreeWidget* tree, const QList<QString>& stateList, QChar marker, UnfoldProc unfoldProc)
{
	QSet<QString> states;
	foreach (QString state, stateList)
	{
		if (state[0] == marker)
		{
			states.insert( state.mid( 1, state.length() - 1));
		}
	}
	QTreeWidgetItemIterator it( tree);
	while (*it)
	{
		QString id = (*it)->data( 0, Qt::UserRole).toString();
		if (states.contains( id))
		{
			unfoldProc( tree, *it);
		}
		++it;
	}
}

void WidgetVisitorState_QTreeWidget::setState( const QString& state)
{
	qDebug() << "Restoring tree state for tree" << m_elementname;
	QList<QString> stateList = state.split( "|", QString::SkipEmptyParts);
	// expand tree first, otherwise parents get selected if we select a non-expanded subtree!
	unfoldNodes( m_treeWidget, stateList, 'E', unfoldProc_SetExpanded);
	unfoldNodes( m_treeWidget, stateList, 'S', unfoldProc_SetSelected);
}

QString WidgetVisitorState_QTreeWidget::getState() const
{
	QTreeWidgetItemIterator it( m_treeWidget );
	QString state = "";
	while( *it ) {
		if( (*it)->isExpanded( ) ) {
			state.append( "E" );
			state.append( (*it)->data( 0, Qt::UserRole ).toString( ) );
			state.append( "|" );
		}
		if( (*it)->isSelected( ) ) {
			state.append( "S" );
			state.append( (*it)->data( 0, Qt::UserRole ).toString( ) );
			state.append( "|" );
		}
		++it;
	}
	return state;
}


