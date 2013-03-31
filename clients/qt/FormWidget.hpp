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

#ifndef _FORM_WIDGET_HPP_INCLUDED
#define _FORM_WIDGET_HPP_INCLUDED

#include "DataLoader.hpp"
#include "FormLoader.hpp"
#include "WidgetListener.hpp"
#include <QWidget>
#include <QBuffer>
#include <QtUiTools>
#include <QHBoxLayout>
#include <QSignalMapper>
#include <QLabel>
#include <QHash>
#include <QCloseEvent>
#include <QTranslator>

class FormWidget : public QWidget
{
	Q_OBJECT

	public:		
		FormWidget( FormLoader *_formLoader, DataLoader *_dataLoader, QHash<QString,QVariant>* _globals, QUiLoader *_uiLoader, QWidget *_parent = 0, bool _debug = false );
		virtual ~FormWidget( );
		void loadForm( QString name, bool modal = false );
		void setLocale( QLocale locale );
		void setLanguage( QString language );
	
	public:
		QString form( ) const;
		QIcon getWindowIcon( ) const;

	public slots:
		void setForm( const QString &_form );
		void reload( );
			
	private:
		QString m_form;				// name of the form
		QString m_previousForm;
		QUiLoader *m_uiLoader;			// UI loader to user for loading designer XML files
		FormLoader *m_formLoader;		// form loader (visible form)
		DataLoader *m_dataLoader;		// load and saves data (data form)
		QHash<QString,QVariant>* m_globals;	// global variable assignments read in init form and written in switch/close form
		QWidget *m_ui;				// the ui loaded from the designer XML file
		QLocale m_locale;			// current locale to use for form translations
		QHBoxLayout *m_layout;			// main layout swallowing the form
		QSignalMapper *m_signalMapper;		// delegate for form push buttons pointing to forms
		QStringList m_forms;			// names of all currently loaded forms
		QTranslator m_translator;		// contains the translations for the current form
		QHash<QString,QList<WidgetListenerR> > m_listeners;// widget signal listeners for this form
		bool m_debug;
		bool m_modal;
		
	private:
		void initialize( );
	
	signals:
		void formLoaded( QString name );
		void formModal( QString m_form );
		void error( QString error );
		void closed( );
		void datarequest();

	protected:
		void changeEvent( QEvent *_event );
		void closeEvent( QCloseEvent *e );

	private slots:
		void formLoaded( QString name, QByteArray form );
		void formLocalizationLoaded( QString name, QByteArray localization );
		void formListLoaded( QStringList forms );
		
		void gotAnswer( const QString& tag_, const QByteArray& data_);
		void gotError( const QString& tag_, const QByteArray& data_);

		void switchForm( QWidget *object );
};
	
#endif // _FORM_WIDGET_HPP_INCLUDED
