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
#include "WidgetVisitor_QTableWidget.hpp"
#include <QDebug>

WidgetVisitorState_QTableWidget::WidgetVisitorState_QTableWidget( QWidget* widget_)
	:WidgetVisitor::State(widget_)
	,m_tableWidget(qobject_cast<QTableWidget*>(widget_))
	,m_mode(Init)
	,m_row(-1)
	,m_column(-1)
	,m_rowcount(0)
	,m_columncount(0)
{
	m_rowcount = m_tableWidget->rowCount();
	for (int rr=0; rr<m_rowcount; ++rr)
	{
		QTableWidgetItem* item = m_tableWidget->verticalHeaderItem( rr);
		m_dataelements_row.push_back( item->text().toAscii());
		m_rowheaders[ m_dataelements_row.back()] = rr;
	}
	m_columncount = m_tableWidget->columnCount();
	for (int cc=0; cc<m_columncount; ++cc)
	{
		QTableWidgetItem* item = m_tableWidget->horizontalHeaderItem( cc);
		m_dataelements_col.push_back( item->text().toAscii());
		m_colheaders[ m_dataelements_col.back()] = cc;
	}
	m_dataelements_row.push_back( "id");
	m_dataelements_col.push_back( "id");
	m_dataelements_init = DataElements( "row", "column", 0);
}

void WidgetVisitorState_QTableWidget::clear()
{
	m_tableWidget->clearContents();
	m_mode = Init;
	m_row = -1;
	m_column = -1;
}

bool WidgetVisitorState_QTableWidget::enter( const QByteArray& name, bool writemode)
{
	if (writemode)
	{
		if (m_mode == Init && strcmp( name, "row") == 0 && m_tableWidget->columnCount())
		{
			m_mode = Row;
			return true;
		}
		if (m_mode == Init && strcmp( name, "column") == 0 && m_tableWidget->rowCount())
		{
			m_mode = Column;
			return true;
		}
	}
	else
	{
		if (m_mode == Init && strcmp( name, "row") == 0)
		{
			if (m_row +1 >= m_rowcount) return false;
			m_mode = Row;
			++m_row;
			return true;
		}
		if (m_mode == Init && strcmp( name, "column") == 0)
		{
			if (m_column +1 >= m_columncount) return false;
			m_mode = Column;
			++m_column;
			return true;
		}
	}
	return false;
}

bool WidgetVisitorState_QTableWidget::leave( bool writemode)
{
	if (m_mode == Init) return false;
	if (writemode)
	{
		if (m_mode == Row)
		{
			if (m_row < 0)
			{
				m_tableWidget->insertRow( m_row = m_rowcount++);
			}
			for (int col=0; col<m_items.size(); ++col)
			{
				QTableWidgetItem* item = m_tableWidget->item( m_row, col);
				item->setData( Qt::UserRole, m_items.at(col));
			}
		}
		else
		{
			if (m_column < 0)
			{
				m_tableWidget->insertColumn( m_column = m_columncount++);
			}
			for (int row=0; row<m_items.size(); ++row)
			{
				QTableWidgetItem* item = m_tableWidget->item( row, m_column);
				item->setData( Qt::UserRole, m_items.at( row));
			}
		}
	}
	m_row = -1;
	m_column = -1;
	m_mode = Init;
	return true;
}

bool WidgetVisitorState_QTableWidget::isRepeatingDataElement( const QByteArray& name)
{
	if (m_mode == Init && (name == "row" || name == "column")) return true;
	return false;
}

QVariant WidgetVisitorState_QTableWidget::property( const QByteArray& name)
{
	QHash<QByteArray,int>::const_iterator itr;
	if (m_mode == Row)
	{
		itr = m_colheaders.find( name);
		if (itr == m_colheaders.end()) return false;
		int col = itr.value();
		QTableWidgetItem* item = m_tableWidget->item( m_row, col);
		return item->data( Qt::UserRole);
	}
	if (m_mode == Column)
	{
		itr = m_rowheaders.find( name);
		if (itr == m_rowheaders.end()) return false;
		int row = itr.value();
		QTableWidgetItem* item = m_tableWidget->item( row, m_column);
		return item->data( Qt::UserRole);
	}
	return QVariant();
}

bool WidgetVisitorState_QTableWidget::setProperty( const QByteArray& name, const QVariant& data)
{
	QHash<QByteArray,int>::const_iterator itr;
	if (m_mode == Row)
	{
		if (name == "id")
		{
			itr = m_rowheaders.find( name);
			if (m_row >= 0 || itr == m_rowheaders.end()) return false;
			m_row = itr.value();
			return true;
		}
		itr = m_colheaders.find( name);
		if (itr == m_colheaders.end()) return false;
		int col = itr.value();
		while (col <= m_items.size()) m_items.push_back( QVariant());
		m_items[ col] = data;
		return true;
	}
	if (m_mode == Column)
	{
		if (name == "id")
		{
			itr = m_colheaders.find( name);
			if (m_column >= 0 || itr == m_colheaders.end()) return false;
			m_column = itr.value();
			return true;
		}
		itr = m_rowheaders.find( name);
		if (itr == m_rowheaders.end()) return false;
		int row = itr.value();
		while (row <= m_items.size()) m_items.push_back( QVariant());
		m_items[ row] = data;
		return true;
	}
	return false;
}

const QList<QByteArray>& WidgetVisitorState_QTableWidget::dataelements() const
{
	static const QList<QByteArray> noDataElements;
	switch (m_mode)
	{
		case Init: return m_dataelements_init;
		case Column: return m_dataelements_col;
		case Row: return m_dataelements_row;
	}
	return noDataElements;
}

void WidgetVisitorState_QTableWidget::setState( const QVariant& state)
{
	qDebug() << "Restoring Table state for table" << m_tableWidget->objectName();
	QTableWidgetSelectionRange all( 0, 0, m_tableWidget->rowCount(), m_tableWidget->columnCount());
	m_tableWidget->setRangeSelected( all, false);

	foreach (const QVariant& elem, state.toList())
	{
		QLine range = elem.toLine();
		int top = range.p1().x();
		int left = range.p1().y();
		int bottom = range.p2().x();
		int right = range.p2().y();
		QTableWidgetSelectionRange selected( top, left, bottom, right);
		m_tableWidget->setRangeSelected( selected, true);
	}
	for( int ii = 0; ii < m_tableWidget->columnCount(); ii++)
	{
		m_tableWidget->resizeColumnToContents( ii);
	}
	m_tableWidget->adjustSize();
}

QVariant WidgetVisitorState_QTableWidget::getState() const
{
	QList<QVariant> rt;
	foreach (const QTableWidgetSelectionRange& selected, m_tableWidget->selectedRanges())
	{
		int top = selected.topRow();
		int left = selected.leftColumn();
		int bottom = selected.bottomRow();
		int right = selected.rightColumn();
		QLine range( QPoint( top, left), QPoint( bottom, right));
		rt.push_back( QVariant( range));
	}
	return QVariant(rt);
}


