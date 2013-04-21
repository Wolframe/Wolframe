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
#include "WidgetListener.hpp"
#include "WidgetListener.hpp"
#include "WidgetEnabler.hpp"
#include <QDebug>
#include <QLabel>
#include <QBuffer>
#include <QHeaderView>
#include <QSignalMapper>
#include <QWidget>

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
		if (item)
		{
			m_rowheaders[ item->text().toLatin1()] = rr;
		}
	}
	m_columncount = m_tableWidget->columnCount();
	for (int cc=0; cc<m_columncount; ++cc)
	{
		QTableWidgetItem* item = m_tableWidget->horizontalHeaderItem( cc);
		if (item)
		{
			m_colheaders[ item->text().toLatin1()] = cc;
		}
	}
}

WidgetVisitorState_QTableWidget::~WidgetVisitorState_QTableWidget()
{
	foreach (QWidget* wdg, m_cellwidgets) if (wdg) delete wdg;
}

void WidgetVisitorState_QTableWidget::clear()
{
	m_tableWidget->clearContents();
	for( int i = m_tableWidget->rowCount( ) - 1; i >= 0; i-- ) {
		m_tableWidget->removeRow( i );
	}
	m_mode = Init;
	m_row = -1;
	m_column = -1;
	m_rowcount = m_tableWidget->rowCount();
	m_columncount = m_tableWidget->columnCount();
}

bool WidgetVisitorState_QTableWidget::enter( const QString& name, bool writemode)
{
	switch (m_mode)
	{
		case Init:
		{
			if (name == "row")
			{
				if (writemode)
				{
					m_row = m_rowcount; //... default insertion at bottom of table
				}
				else
				{
					if (++m_row >= m_rowcount) return false;
				}
				m_mode = Row;
				return true;
			}
			if (name == "column")
			{
				if (writemode)
				{
					m_column = m_columncount; //... default insertion at end of table
				}
				else
				{
					if (++m_column >= m_columncount) return false;
				}
				m_mode = Column;
				return true;
			}
			return false;
		}
		case Row:
		{
			QHash<QString,int>::const_iterator itr = m_colheaders.find( name);
			if (itr == m_colheaders.end()) return false;
			m_column = itr.value();
			if (writemode)
			{
				while (m_column >= m_items.size()) m_items.push_back( QVariant());
				while (m_column >= m_cellwidgets.size()) m_cellwidgets.push_back( 0);
			}
			m_mode = RowData;
			return true;
		}
		case Column:
		{
			QHash<QString,int>::const_iterator itr = m_rowheaders.find( name);
			if (itr == m_rowheaders.end()) return false;
			m_row = itr.value();
			if (writemode)
			{
				while (m_row >= m_items.size()) m_items.push_back( QVariant());
				while (m_row >= m_cellwidgets.size()) m_cellwidgets.push_back( 0);
			}
			m_mode = ColumnData;
			return true;
		}
		case RowData:
		case ColumnData:
			return false;
	}
	return false;
}

void WidgetVisitorState_QTableWidget::fill_cell( int row, int col, int itemidx)
{
	QTableWidgetItem* item = m_tableWidget->item( row, col);
	if (!item)
	{
		item = new QTableWidgetItem( m_items.at( itemidx).toString());
		item->setFlags( item->flags() ^ Qt::ItemIsEditable);
		if (m_items.at( itemidx).isValid())
		{
			item->setData( Qt::UserRole, m_items.at( itemidx));
		}
		m_tableWidget->setItem( row, col, item);
	}
	if (m_cellwidgets.at( itemidx))
	{
		m_tableWidget->setCellWidget( row, col, m_cellwidgets.at( itemidx));
		m_cellwidgets[ itemidx] = 0;
	}
}

bool WidgetVisitorState_QTableWidget::leave( bool writemode)
{
	switch (m_mode)
	{
		case Init:
			return false;
		case Row:
			if (writemode)
			{
				if (m_row == m_rowcount)
				{
					m_tableWidget->insertRow( m_rowcount++);
				}
				for (int col=0; col<m_items.size(); ++col)
				{
					fill_cell( m_row, col, col);
				}
				m_items.clear();
				m_cellwidgets.clear();
			}
			m_row = -1;
			m_column = -1;
			m_mode = Init;
			return true;
		case Column:
			if (writemode)
			{
				if (m_column == m_columncount)
				{
					m_tableWidget->insertColumn( m_columncount++);
				}
				for (int row=0; row<m_items.size(); ++row)
				{
					fill_cell( row, m_column, row);
				}
				m_items.clear();
				m_cellwidgets.clear();
			}
			m_row = -1;
			m_column = -1;
			m_mode = Init;
			return true;
		case RowData:
			m_mode = Row;
			return true;
		case ColumnData:
			m_mode = Column;
			return true;
	}
	return false;
}

bool WidgetVisitorState_QTableWidget::isArrayElement( const QString& name)
{
	if (m_mode == Init && (name == "row" || name == "column")) return true;
	return false;
}

void WidgetVisitorState_QTableWidget::set_thumbnail( int row, int itemidx, const QVariant& data)
{
	QByteArray decoded = QByteArray::fromBase64( data.toByteArray());
	QPixmap pixmap;
	pixmap.loadFromData( decoded);
	QLabel *label = new QLabel();
	label->setPixmap( pixmap);
	label->setFixedSize( pixmap.size());
	if (m_cellwidgets.at( itemidx)) delete m_cellwidgets[ itemidx];
	m_cellwidgets[ itemidx] = label;
	// Aba, HACK: backport from configurator, force size of row to be at
	// least 50px
	m_tableWidget->setRowHeight( row, 50 );
}

QVariant WidgetVisitorState_QTableWidget::get_thumbnail( int row, int col) const
{
	const QWidget* cellwidget = m_tableWidget->cellWidget( row, col);
	if (!cellwidget) return QVariant();
	const QLabel* label = qobject_cast<const QLabel*>( cellwidget);
	if (!label) return QVariant();
	const QPixmap* pixmap = label->pixmap();
	if (!pixmap) return QVariant();
	QByteArray bytes;
	QBuffer buffer(&bytes);
	buffer.open( QIODevice::WriteOnly);
	pixmap->save( &buffer);
	return QVariant( bytes.toBase64());
}

QVariant WidgetVisitorState_QTableWidget::getDataValue( const char* propertyname, int idx) const
{
	QVariant values_p = m_tableWidget->property( propertyname);
	QList<QVariant> values = values_p.toList();
	if (idx < 0 || idx >= values.size()) return QVariant();
	return values.at( idx);
}

void WidgetVisitorState_QTableWidget::setDataValue( const char* propertyname, int idx, QVariant value)
{
	QVariant values_p = m_tableWidget->property( propertyname);
	QList<QVariant> values = values_p.toList();
	while (idx > values.size()) values.push_back( QVariant());
	if (idx == values.size())
	{
		values.push_back( value);
	}
	else
	{
		values[ idx] = value;
	}
	m_tableWidget->setProperty( propertyname, values);
}

QVariant WidgetVisitorState_QTableWidget::getRowValue( int row) const
{
	return getDataValue( "_w_rowvalue", row);
}

void WidgetVisitorState_QTableWidget::setRowValue( int row, QVariant value)
{
	setDataValue( "_w_rowvalue", row, value);
}

QVariant WidgetVisitorState_QTableWidget::getColumnValue( int col) const
{
	return getDataValue( "_w_columnvalue", col);
}

void WidgetVisitorState_QTableWidget::setColumnValue( int col, QVariant value)
{
	setDataValue( "_w_columnvalue", col, value);
}

int WidgetVisitorState_QTableWidget::findSelectedData( const char* propertyname, QVariant value)
{
	QVariant values_p = m_tableWidget->property( propertyname);
	if (!values_p.isValid()) return -1;
	return values_p.toList().indexOf( value);
}

int WidgetVisitorState_QTableWidget::findSelectedRow( QVariant value)
{
	return findSelectedData( "_w_rowvalue", value);
}

int WidgetVisitorState_QTableWidget::findSelectedColumn( QVariant value)
{
	return findSelectedData( "_w_columnvalue", value);
}

QVariant WidgetVisitorState_QTableWidget::getSelectedValue() const
{
	int row = m_tableWidget->currentRow();
	int col = m_tableWidget->currentColumn();
	if (m_tableWidget->property( "_w_rowvalue").isValid())
	{
		return getRowValue( row);
	}
	else if (m_tableWidget->property( "_w_columnvalue").isValid())
	{
		return getColumnValue( col);
	}
	else
	{
		return m_tableWidget->property( "_w_selected");
	}
}


QVariant WidgetVisitorState_QTableWidget::property( const QString& name)
{
	QTableWidgetItem* item;
	switch (m_mode)
	{
		case Init:
			if (name == "selected")
			{
				QVariant sel = getSelectedValue();
				return sel;
			}
			break;
		case Row:
			if (name == "title")
			{
				item = m_tableWidget->verticalHeaderItem( m_row);
				if (item)
				{
					return QVariant( item->text().toLatin1());
				}
			}
			else if (name == "id")
			{
				return getRowValue( m_row);
			}
			break;
		case Column:
			if (name == "title")
			{
				item = m_tableWidget->horizontalHeaderItem( m_column);
				if (item)
				{
					return QVariant( item->text().toLatin1());
				}
			}
			else if (name == "id")
			{
				return getColumnValue( m_column);
			}
			break;
		case RowData:
		case ColumnData:
			if (name.isEmpty())
			{
				item = m_tableWidget->item( m_row, m_column);
				if (item) return item->data( Qt::UserRole);
			}
			if (name == "thumbnail")
			{
				return get_thumbnail( m_row, m_column);
			}
	}
	return QVariant();
}

bool WidgetVisitorState_QTableWidget::setProperty( const QString& name, const QVariant& data)
{
	QHash<QString,int>::const_iterator itr;
	switch (m_mode)
	{
		case Init:
			if (name == "selected")
			{
				m_tableWidget->setProperty( "_w_selected", data);
				endofDataFeed();
				return true;
			}
			break;
		case Row:
			if (name == "title")
			{
				itr = m_rowheaders.find( data.toByteArray());
				if (m_row >= 0 || itr == m_rowheaders.end()) return false;
				m_row = itr.value();
				return true;
			}
			else if (name == "id")
			{
				setRowValue( m_row, data);
				return true;
			}
			break;
		case Column:
			if (name == "title")
			{
				itr = m_colheaders.find( data.toByteArray());
				if (m_column >= 0 || itr == m_colheaders.end()) return false;
				m_column = itr.value();
				return true;
			}
			else if (name == "id")
			{
				setColumnValue( m_column, data);
				return true;
			}
			break;
		case RowData:
			if (name.isEmpty())
			{
				m_items[ m_column] = data;
				return true;
			}
			if (name == "thumbnail")
			{
				set_thumbnail( m_row, m_column, data);
				return true;
			}
			break;
		case ColumnData:
			if (name.isEmpty())
			{
				m_items[ m_row] = data;
				return true;
			}
			if (name == "thumbnail")
			{
				set_thumbnail( m_row, m_row, data);
				return true;
			}
			break;
	}
	return false;
}

void WidgetVisitorState_QTableWidget::setState( const QVariant& state)
{
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
	m_tableWidget->horizontalHeader()->setStretchLastSection(true);
#if QT_VERSION >= 0x050000
	m_tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
#else
	m_tableWidget->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
#endif
	for( int ii = 0; ii < m_tableWidget->columnCount(); ii++)
	{
		m_tableWidget->resizeColumnToContents( ii);
	}
	for( int ii = 0; ii < m_tableWidget->rowCount(); ii++)
	{
		m_tableWidget->resizeRowToContents( ii);
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

void WidgetVisitorState_QTableWidget::endofDataFeed()
{
	QVariant selected = m_tableWidget->property( "_w_selected");
	if (selected.isValid())
	{
		int row = findSelectedRow( selected);
		if (row >= 0) m_tableWidget->setCurrentCell( row, 0);
		int col = findSelectedColumn( selected);
		if (col >= 0) m_tableWidget->setCurrentCell( 0, col);
	}
}

void WidgetVisitorState_QTableWidget::connectDataSignals( WidgetVisitor::DataSignalType dt, WidgetListener& listener)
{
	switch (dt)
	{
		case WidgetVisitor::SigChanged: QObject::connect( m_tableWidget, SIGNAL( cellChanged( int,int)), &listener, SLOT( changed()), Qt::UniqueConnection); break;
		case WidgetVisitor::SigActivated: QObject::connect( m_tableWidget, SIGNAL( cellActivated( int,int)), &listener, SLOT( activated()), Qt::UniqueConnection); break;
		case WidgetVisitor::SigEntered: QObject::connect( m_tableWidget, SIGNAL( cellEntered( int,int)), &listener, SLOT( entered()), Qt::UniqueConnection); break;
		case WidgetVisitor::SigPressed: QObject::connect( m_tableWidget, SIGNAL( cellPressed( int,int)), &listener, SLOT( pressed()), Qt::UniqueConnection); break;
		case WidgetVisitor::SigClicked: QObject::connect( m_tableWidget, SIGNAL( cellClicked( int,int)), &listener, SLOT( clicked()), Qt::UniqueConnection); break;
		case WidgetVisitor::SigDoubleClicked: QObject::connect( m_tableWidget, SIGNAL( cellDoubleClicked( int,int)), &listener, SLOT( doubleclicked()), Qt::UniqueConnection);
	}
}

void WidgetVisitorState_QTableWidget::connectWidgetEnabler( WidgetEnabler& enabler)
{
	QObject::connect( m_tableWidget, SIGNAL( cellChanged( int,int)), &enabler, SLOT( changed()), Qt::UniqueConnection);
	QObject::connect( m_tableWidget, SIGNAL( cellActivated( int,int)), &enabler, SLOT( changed()), Qt::UniqueConnection);
	QObject::connect( m_tableWidget, SIGNAL( cellEntered( int,int)), &enabler, SLOT( changed()), Qt::UniqueConnection);
	QObject::connect( m_tableWidget, SIGNAL( cellPressed( int,int)), &enabler, SLOT( changed()), Qt::UniqueConnection);
	QObject::connect( m_tableWidget, SIGNAL( cellClicked( int,int)), &enabler, SLOT( changed()), Qt::UniqueConnection);
	QObject::connect( m_tableWidget, SIGNAL( cellDoubleClicked( int,int)), &enabler, SLOT( changed()), Qt::UniqueConnection);
}

