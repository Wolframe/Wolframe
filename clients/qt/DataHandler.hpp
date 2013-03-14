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

#ifndef _DATA_HANDLER_INCLUDED
#define _DATA_HANDLER_INCLUDED

#include <QWidget>
#include <QString>
#include <QByteArray>
#include <QHash>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QSet>

#include "DataLoader.hpp"

class FormWidget;

class DataHandler : public QObject
{
	Q_OBJECT
	
	public:
		DataHandler( DataLoader *_dataLoader, FormWidget *_formWidget, bool _debug = false );
		virtual ~DataHandler( ) {};
		void readFormData( QString name, QWidget *form, QByteArray &data, QHash<QString, QString> *props );
		void writeFormData( QString name, QWidget *form, QByteArray *data, QHash<QString, QString> *props );
		void resetFormData( QWidget *form );
		void resetFormData( QWidget *form, QString name );
		void clearFormData( QWidget *form, QString name );
		void clearWidgetData( QWidget *widget, QString name );
		void loadFormDomain( QString form_name, QString widget_name, QWidget *form, QByteArray &data, QHash<QString, QString> *props );
		void loadFormDomains( QString name, QWidget *form );
		QString readFormVariable( QString variable, QWidget *form );

	private:
		void writeWidgets( QWidget *_parent, QStringList *dataElements, QXmlStreamWriter &xml, QHash<QString, QString> *props, QSet<QWidget *> *set );
		void resetWidgetData( QWidget *widget, QString name );
	
	private:
		DataLoader *m_dataLoader;
		FormWidget *m_formWidget;
		bool m_debug;
};

#endif // _DATA_HANDLER_INCLUDED
