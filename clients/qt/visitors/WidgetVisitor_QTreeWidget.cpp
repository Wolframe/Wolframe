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
			if (m_stk.top().readpos >= m_stk.top().item->childCount()) return false;
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
			if (m_stk.top().readpos >= m_stk.top().item->childCount()) return false;
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

enum StateTag {None,Open,Close,Expand,Select,ExpandSelect};
static const char g_tagchr[] = "_ocES*";

static QVariant getStateElementKey( StateTag tag, const QVariant& elem)
{
	QString rt;
	rt.push_back( (QChar)g_tagchr[ (int)tag]);
	rt.append( elem.toString());
	return QVariant(rt);
}

static QString stateElementValue( const QVariant& elemkey)
{
	QString kk = elemkey.toString();
	return kk.mid( 1, kk.size()-1);
}

static StateTag stateElementTag( const QVariant& elemkey)
{
	QString kk = elemkey.toString();
	const char* pos = strchr( g_tagchr, kk.at( 0).toAscii());
	return pos?(StateTag)(pos-g_tagchr):None;
}

static void skipTag( QList<QVariant>::const_iterator& itr, const QList<QVariant>::const_iterator end)
{
	++itr;
	for (int taglevel=1; itr!=end && taglevel > 0; ++itr)
	{
		switch (stateElementTag(*itr))
		{
			case Open: ++taglevel; break;
			case Close: --taglevel; break;
			case None:
			case Expand:
			case Select:
			case ExpandSelect: break;
		}
	}
	--itr;
}

static QTreeWidgetItem* findchild( const QTreeWidgetItem* item, int keyidx, const QVariant& elemkey)
{
	int ii=0,chldcnt = item->childCount();
	QString elemkeystr = stateElementValue( elemkey);
	for (; ii<chldcnt; ++ii)
	{
		QTreeWidgetItem* chld = item->child( ii);
		if (elemkeystr == chld->data( keyidx, Qt::UserRole).toString()) return chld;
	}
	return 0;
}

void WidgetVisitorState_QTreeWidget::setState( const QVariant& state)
{
	qDebug() << "Restoring tree state for tree" << m_elementname;
	QStack<StackElement> stk;
	int keyidx = m_headers.indexOf( "id");
	if (keyidx < 0) keyidx = 0; //... first element is key if "id" not defined

	stk.push_back( m_treeWidget->invisibleRootItem());

	QList<QVariant> statelist = state.toList();
	QList<QVariant>::const_iterator stateitr = statelist.begin();
	QTreeWidgetItem* chld;

	for (; !stk.isEmpty() && stateitr != statelist.end(); ++stateitr)
	{
		switch (stateElementTag(*stateitr))
		{
			case Open:
				chld = findchild( stk.top().item, keyidx, *stateitr);
				if (chld)
				{
					stk.push_back( chld);
				}
				else
				{
					skipTag( stateitr, statelist.end());
				}
				break;
			case Close:
				stk.pop_back();
				break;
			case None:
				break;
			case Expand:
				stk.top().item->setExpanded( true);
				break;
			case Select:
				stk.top().item->setSelected( true);
				// better than nothing, scroll to the position of the last selection (usually one)
				m_treeWidget->scrollToItem( stk.top().item);
				break;
			case ExpandSelect:
				stk.top().item->setExpanded( true);
				stk.top().item->setSelected( true);
				// better than nothing, scroll to the position of the last selection (usually one)
				m_treeWidget->scrollToItem( stk.top().item);
				stk.top().item->setExpanded( true);
				break;
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
	QList<QVariant> rt;
	QStack<StackElement> stk;
	int keyidx = m_headers.indexOf( "id");
	if (keyidx < 0) keyidx = 0; //... first element is key if "id" not defined

	stk.push_back( m_treeWidget->invisibleRootItem());
	while (!stk.isEmpty())
	{
		StateTag stateTag = None;
		if (stk.top().item->isExpanded())
		{
			stateTag = stk.top().item->isSelected()?ExpandSelect:Expand;
		}
		else if (stk.top().item->isSelected())
		{
			stateTag = Select;
		}
		if (stateTag != None)
		{
			rt.push_back( getStateElementKey( stateTag, stk.top().item->data( 0, Qt::UserRole)));
		}
		if (stk.top().readpos >= stk.top().item->childCount())
		{
			rt.push_back( getStateElementKey( Close, QVariant()));
			stk.pop_back();
			continue;
		}
		else if (stk.size() == 1 || stk.top().item->isExpanded())
		{
			rt.push_back( getStateElementKey( Open, stk.top().item->data( 0, Qt::UserRole)));
			stk.push_back( stk.top().item->child( stk.top().readpos++));
		}
		else
		{
			stk.top().readpos++;
		}
	}
	return QVariant(rt);
}


