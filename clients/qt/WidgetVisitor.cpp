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
#include "WidgetVisitor.hpp"

#include <QDebug>
#include <QByteArray>
#include <QXmlStreamWriter>

WidgetVisitor::WidgetVisitor( QWidget* root, QHash<QByteArray,QString>* globals_)
	:m_globals(globals_)
{
	m_stk.push( QPair<QWidget*,WidgetState>( root, 0));
}

static WidgetState widgetInternalEnter( QWidget* widget, WidgetState state, const QByteArray& name)
{
	QString clazz = widget->metaObject()->className();
	if (clazz == "QTreeWidget")
	{

	}
	else
	{
		return 0;
	}

}

static WidgetState widgetInternalLeave( QWidget* widget, WidgetState state)
{
	return 0;
}

static QWidget* widgetEnter( QWidget* widget, const QByteArray& name)
{
	QString clazz = widget->metaObject()->className();

	if (clazz == "QTreeWidget")
	{

	}
	else
	{
		return 0;
	}

}


bool WidgetVisitor::enter( const QString& name)
{
	return enter( name.toAscii());
}

bool WidgetVisitor::enter( const QByteArray& name)
{
	if (m_stk.empty()) return false;
	WidgetState state;
	if ((state = widgetInternalEnter( m_stk.top().first, m_stk.top().second, name)) != 0)
	{
		m_stk.top().second = state;
		return true;
	}
	QWidget* wdg;
	if ((wdg = widgetEnter( m_stk.top().first, name)) != 0)
	{
		m_stk.push( QPair<QWidget*,WidgetState>( wdg, 0));
		return true;
	}
	return false;
}

void WidgetVisitor::leave()
{
	if (m_stk.empty()) return;
	WidgetState state;
	if ((state = widgetInternalLeave( m_stk.top().first, m_stk.top().second)) != NULL)
	{
		m_stk.top().second = state;
	}
	else
	{
		m_stk.pop();
	}
}

static bool clearWidgetProperty( QWidget* widget, const QByteArray& name, QVariant data)
{
	QString clazz = widget->metaObject()->className();

	if (clazz == "QLineEdit") {
		QLineEdit *lineEdit = qobject_cast<QLineEdit *>( widget);
		lineEdit->clear();
	} else if (clazz == "QDateEdit") {
	} else if (clazz == "QTimeEdit") {
	} else if (clazz == "QDateTimeEdit") {
	} else if (clazz == "QComboBox") {
		QComboBox *comboBox = qobject_cast<QComboBox *>( widget);
		comboBox->clear();
	} else if (clazz == "QSpinBox") {
	} else if (clazz == "QDoubleSpinBox") {
	} else if (clazz == "QSlider") {
	} else if (clazz == "QPlainTextEdit") {
		QPlainTextEdit *plainTextEdit = qobject_cast<QPlainTextEdit *>( widget);
		plainTextEdit->clear();
	} else if (clazz == "QTextEdit") {
		QTextEdit *textEdit = qobject_cast<QTextEdit *>( widget);
		textEdit->clear();
	} else if (clazz == "QCheckBox") {
		QCheckBox *checkBox = qobject_cast<QCheckBox *>( widget);
		checkBox->setChecked( false);
	} else if (clazz == "QRadioButton") {
		QRadioButton *radioButton = qobject_cast<QRadioButton *>( widget);
		radioButton->setChecked( false);
	} else if (clazz == "QListWidget") {
		QListWidget *listWidget = qobject_cast<QListWidget *>( widget);
		listWidget->clear();
	} else if (clazz == "QTreeWidget") {
		QTreeWidget *treeWidget = qobject_cast<QTreeWidget *>( widget);
		treeWidget->clear();
	} else if (clazz == "QTableWidget") {
		QTableWidget *tableWidget = qobject_cast<QTableWidget *>( widget);
		tableWidget->clearContents();
		for( int i = tableWidget->rowCount() - 1; i >= 0; i--) {
			tableWidget->removeRow( i);
		}
	} else if (clazz == "FileChooser") {
		FileChooser *fileChooser = qobject_cast<FileChooser *>( widget);
		fileChooser->setFileName( "");
	} else if (clazz == "PictureChooser") {
		PictureChooser *pictureChooser = qobject_cast<PictureChooser *>( widget);
		pictureChooser->setFileName( "");
	} else if (clazz == "QPushButton") {
		// skip, ok, buttons can't be reset
	} else if (clazz == "QGroupBox") {
		// skip, ok, grouboxes can't be reset
	} else if (clazz == "QWidget") {
		// skip, generic widget, don't possibly know how to reset it
	} else {
		qWarning() << "Clear for unknown class" << clazz << "of widget" << widget << "(" << name << ")";
	}
	qDebug() << "Clearing " << clazz << name;
}

static bool setWidgetProperty( QWidget* widget, WidgetState state, const QByteArray& name, QVariant data)
{
	QString clazz = widget->metaObject()->className();
	if (clazz == "QComboBox")
	{
		QComboBox *comboBox = qobject_cast<QComboBox *>( widget);
		if (strcmp( name, "value") == 0)
		{
			comboBox->addItem( data.toString());
			return true;
		}
		else if (strcmp( name, "value.id") == 0)
		{
			if (comboBox->currentIndex() < 0) return false;
			comboBox->setItemData( comboBox->currentIndex(), data, Qt::UserRole);
			return true;
		}
		else if (strcmp( name, "select") == 0)
		{
			int idx = comboBox->findText( data.toString(), Qt::UserRole, Qt::MatchExactly);
			if (idx < 0) return false;
			comboBox->currentIndexChanged( idx);
			return true;
		}
		else if (strcmp( name, "select.id") == 0)
		{
			int idx = comboBox->findData( data, Qt::UserRole, Qt::MatchExactly);
			if (idx < 0) return false;
			comboBox->currentIndexChanged( idx);
			return true;
		}
	}
	else if (clazz == "QListWidget")
	{
		QListWidget *listWidget = qobject_cast<QListWidget *>( widget);
		if (strcmp( name, "value") == 0)
		{
			listWidget->addItem( text);
			return true;
		}
		if (strcmp( name, "select") == 0)
		{
			QList<QListWidgetItem *> items = listWidget->findItems( data.toString(), Qt::MatchExactly);
			foreach( QListWidgetItem *item, items)
			{
				item->setSelected( true);
			}
		}
		else
		{
			QList<QListWidgetItem *> items = listWidget->findItems( name, Qt::MatchExactly);
			foreach( QListWidgetItem *item, items)
			{
				item->setSelected( data.toBool());
			}
		}
	}
}


static QVariant getWidgetProperty( QWidget* widget, WidgetState state, const QByteArray& name)
{
	QString clazz = widget->metaObject()->className();
	if (clazz == "QComboBox")
	{
		QComboBox *comboBox = qobject_cast<QComboBox *>( widget);
		if (strcmp( name, "select"))
		{
			return QVariant( comboBox->currentText());
		}
		else if (strcmp( name, "select.id"))
		{
			if (comboBox->currentIndex() < 0) return QVariant();
			return comboBox->itemData( comboBox->currentIndex(), Qt::UserRole);
		}
	}
	else if (clazz == "QListWidget")
	{
		if (strcmp( name, "select"))
		{
			QListWidget *listWidget = qobject_cast<QListWidget*>( widget);
			QList<QVariant> rt;
			foreach( QListWidgetItem *item, listWidget->selectedItems())
			{
				rt.append( *item);
			}
			return QVariant( rt);
		}
	}
}

QVariant WidgetVisitor::property( const QString& name)
{
	return property( name.toAscii());
}

QVariant property( const QByteArray& name, int level)
{
	if (m_stk.empty()) return QVariant()/*invalid*/;

	QVariant rt = getWidgetProperty( m_stk.top().first, m_stk.top().second, name);
	if (rt.isValid()) return rt;
	QVariant rt = m_stk.top().first->property( name);
	if (rt.isValid()) return rt;

	int followidx = name.indexOf( '.');
	if (followidx >= 0)
	{
		if (level == 0 && name.startsWith( "global."))
		{
			return QVariant( m_globals[ name.mid( 7, name.size()-7)]);
		}
		QByteArray prefix( name.mid( 0, followidx));
		QByteArray rest( name.mid( followidx+1, name.size()-followidx-1));
		if (m_stk.enter( prefix))
		{
			QVariant rt = property( rest, level+1);
			m_stk.leave();
			return rt;
		}
		if (level == 0 && m_stk.at(0)->objectName() == prefix)
		{
			WidgetVisitor rootvisit( m_stk.at(0).first, m_stk.at(0).second, m_globals);
			return rootvisit.property( rest, 1);
		}
		QList<QByteArray> props = m_stk.top().first->dynamicPropertyNames();
		QList<QByteArray>::const_iterator pi = props.begin(), pi = props.end();
		for (; pi != pe; ++pi)
		{
			if (pi->startsWith( "synonym:"))
			{
				if (name == pi->mid( 8, pi->size()-8))
				{
					QVariant synonym = m_stk.top().first->property( *pi);
					if (synonym.type() == QVariant::String)
					{
						return property( synonym.toString());
					}
					else if (synonym.type() == QVariant::ByteArray)
					{
						return property( synonym.toByteArray());
					}
				}
			}
		}
	}
	return rt;
}

QVariant WidgetVisitor::property( const QByteArray& name)
{
	return property( name, 0);
}

bool WidgetVisitor::setProperty( const QByteArray& name, const QVariant& value, int level)
{
	if (m_stk.empty()) return QVariant()/*invalid*/;

	if (setWidgetProperty( m_stk.top().first, m_stk.top().second, name, value)) return true;
	if (m_stk.top().first->setProperty( name, value)) return true;

	int followidx = name.indexOf( '.');
	if (followidx >= 0)
	{
		if (level == 0 && name.startsWith( "global."))
		{
			m_globals[ name.mid( 7, name.size()-7)] = value;
		}
		QByteArray prefix( name.mid( 0, followidx));
		QByteArray rest( name.mid( followidx+1, name.size()-followidx-1));
		if (m_stk.enter( prefix))
		{
			bool rt = setProperty( rest, value, level+1);
			m_stk.leave();
			return rt;
		}
		if (level == 0 && m_stk.at(0)->objectName() == prefix)
		{
			WidgetVisitor rootvisit( m_stk.at(0).first, m_stk.at(0).second, m_globals);
			return rootvisit.setProperty( rest, value, 1);
		}
		QList<QByteArray> props = m_stk.top().first->dynamicPropertyNames();
		QList<QByteArray>::const_iterator pi = props.begin(), pi = props.end();
		for (; pi != pe; ++pi)
		{
			if (pi->startsWith( "synonym:"))
			{
				if (name == pi->mid( 8, pi->size()-8))
				{
					QVariant synonym = m_stk.top().first->property( *pi);
					if (synonym.type() == QVariant::String)
					{
						return setProperty( synonym.toString(), value);
					}
					else if (synonym.type() == QVariant::ByteArray)
					{
						return setProperty( synonym.toByteArray(), value);
					}
				}
			}
		}
	}
	return rt;
}

bool WidgetVisitor::setProperty( const QString& name, const QVariant& value)
{
	return setProperty( name.toAscii(), value, 0);
}

bool WidgetVisitor::setProperty( const QByteArray& name, const QVariant& value)
{
	return setProperty( name, value, 0);
}

