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

static const char* getPropertyIdentifier( char* buf, int bufsize, const QString& name)
{
	int ii,namelen = name.length();
	if (namelen >= bufsize) return NULL;
	//... properties with length bigger than MaxIdentifierSize are not visible by visitors

	const QChar* nameptr = name.constData();
	for (; ii<namelen; ++ii)
	{
		if (nameptr[ii] > 127 || nameptr[ii] <= 0) return NULL;
		//... properties with non ascii characters are not visible by visitors
		buf[ ii] = (char)nameptr[ii];
	}
	buf[ii] = 0;
	return buf;
}

static const char* getPropertyIdentifier( char* buf, int bufsize, const char* name, int namelen)
{
	int ii;
	if (namelen >= bufsize) return NULL;
	//... properties with length bigger than MaxIdentifierSize are not visible by visitors

	for (; ii<namelen; ++ii)
	{
		if (name[ii] > 127 || name[ii] <= 0) return NULL;
		//... properties with non ascii characters are not visible by visitors
		buf[ ii] = (char)name[ii];
	}
	buf[ii] = 0;
	return buf;
}

WidgetVisitor::WidgetVisitor( QWidget* root, QHash<QString,QString>* globals_)
	:m_globals(globals_)
{
	m_stk.push( root);
}

bool WidgetVisitor::enter( const QString& name)
{
	char name_[MaxIdentifierSize+1];
	const char* id = getPropertyIdentifier( name_, sizeof(name_), name);
	return id?enter( id):false;
}

bool WidgetVisitor::enter( const char* name)
{
}

void WidgetVisitor::leave()
{
	if (m_stk.empty()) return;
	m_stk.pop();
}

static bool clearWidgetProperty( QWidget* widget, const char* name, QVariant data)
{
	QString clazz = widget->metaObject()->className();

	if( clazz == "QLineEdit") {
		QLineEdit *lineEdit = qobject_cast<QLineEdit *>( widget);
		lineEdit->clear();
	} else if( clazz == "QDateEdit") {
		//~ QDateEdit *dateEdit = qobject_cast<QDateEdit *>( widget);
		// TODO
	} else if( clazz == "QTimeEdit") {
		//~ QTimeEdit *timeEdit = qobject_cast<QTimeEdit *>( widget);
		// TODO
	} else if( clazz == "QDateTimeEdit") {
		//~ QDateTimeEdit *dateTimeEdit = qobject_cast<QDateTimeEdit *>( widget);
	} else if( clazz == "QComboBox") {
		QComboBox *comboBox = qobject_cast<QComboBox *>( widget);
		comboBox->clear();
	} else if( clazz == "QSpinBox") {
		//~ QSpinBox *spinBox = qobject_cast<QSpinBox *>( widget);
		// TODO
	} else if( clazz == "QDoubleSpinBox") {
		//~ QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox *>( widget);
		// TODO
	} else if( clazz == "QSlider") {
		//~ QSlider *slider = qobject_cast<QSlider *>( widget);
		// TODO
	} else if( clazz == "QPlainTextEdit") {
		QPlainTextEdit *plainTextEdit = qobject_cast<QPlainTextEdit *>( widget);
		plainTextEdit->clear();
	} else if( clazz == "QTextEdit") {
		QTextEdit *textEdit = qobject_cast<QTextEdit *>( widget);
		textEdit->clear();
	} else if( clazz == "QCheckBox") {
		QCheckBox *checkBox = qobject_cast<QCheckBox *>( widget);
		checkBox->setChecked( false);
	} else if( clazz == "QRadioButton") {
		QRadioButton *radioButton = qobject_cast<QRadioButton *>( widget);
		radioButton->setChecked( false);
	} else if( clazz == "QListWidget") {
		QListWidget *listWidget = qobject_cast<QListWidget *>( widget);
		listWidget->clear();
	} else if( clazz == "QTreeWidget") {
		QTreeWidget *treeWidget = qobject_cast<QTreeWidget *>( widget);
		treeWidget->clear();
	} else if( clazz == "QTableWidget") {
		QTableWidget *tableWidget = qobject_cast<QTableWidget *>( widget);
		tableWidget->clearContents();
		for( int i = tableWidget->rowCount() - 1; i >= 0; i--) {
			tableWidget->removeRow( i);
		}
	} else if( clazz == "FileChooser") {
		FileChooser *fileChooser = qobject_cast<FileChooser *>( widget);
		fileChooser->setFileName( "");
	} else if( clazz == "PictureChooser") {
		PictureChooser *pictureChooser = qobject_cast<PictureChooser *>( widget);
		pictureChooser->setFileName( "");
	} else if( clazz == "QPushButton") {
		// skip, ok, buttons can't be reset
	} else if( clazz == "QGroupBox") {
		// skip, ok, grouboxes can't be reset
	} else if( clazz == "QWidget") {
		// skip, generic widget, don't possibly know how to reset it
	} else {
		qWarning() << "Clear for unknown class" << clazz << "of widget" << widget << "(" << name << ")";
	}
	qDebug() << "Clearing " << clazz << name;
}

static bool setWidgetProperty( QWidget* widget, const char* name, QVariant data)
{
	QString clazz = widget->metaObject()->className();
	if( clazz == "QComboBox")
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
	else if( clazz == "QListWidget")
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


static QVariant getWidgetProperty( QWidget* widget, const char* name)
{
	QString clazz = widget->metaObject()->className();
	if( clazz == "QComboBox")
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
	else if( clazz == "QListWidget")
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
	char name_[MaxIdentifierSize+1];
	const char* id = getPropertyIdentifier( name_, sizeof(name_), name);
	return id?property( id):QVariant()/*invalid*/;
}

QVariant property( const char* name, int level)
{
	QVariant invalid;
	if (m_stk.empty()) return QVariant()/*invalid*/;

	QVariant rt = m_stk.top()->property( name);
	if (rt.isValid()) return rt;
	rt = getWidgetProperty( m_stk.top(), name);
	if (rt.isValid()) return rt;

	const char* follow = strchr( name, '.');
	if (follow)
	{
		int chldlen = follow-name;
		char chldbuf_[MaxIdentifierSize+1];

		if (memcmp( name, "global", chldlen) == 0)
		{
			return QVariant( m_globals[ QString(follow+1)]);
		}
		const char* chld = getPropertyIdentifier( chldbuf_, sizeof(chldbuf_), name, chldlen);
		if (m_stk.enter( chld))
		{
			QVariant rt = property( follow+1, level+1);
			m_stk.leave();
			return rt;
		}
		if (level == 0 && m_stk.at(0)->objectName() == chld)
		{
			WidgetVisitor rootvisit( m_stk.at(0), m_globals);
			return rootvisit.property( follow+1, 1);
		}
	}
	return rt;
}

QVariant WidgetVisitor::property( const char* name)
{
	return property( name, 0);
}

void WidgetVisitor::setProperty( const char* name, const QVariant& value)
{

}

void WidgetVisitor::setProperty( const QString& name, const QVariant& value)
{

}


