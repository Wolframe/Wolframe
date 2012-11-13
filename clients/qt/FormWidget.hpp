//
// FormWidget.hpp
//

#ifndef _FORM_WIDGET_HPP_INCLUDED
#define _FORM_WIDGET_HPP_INCLUDED

#include "DataLoader.hpp"
#include "DataHandler.hpp"
#include "FormLoader.hpp"

#include <QWidget>
#include <QBuffer>
#include <QtUiTools>
#include <QHBoxLayout>
#include <QSignalMapper>

namespace _Wolframe {
	namespace QtClient {

	class WidgetProperties : public QObject
	{
	Q_OBJECT
	
	public:
		WidgetProperties( ) : m_action( ), m_nextForm( ) { }
		
		void setNextForm( const QString &_nextForm ) { m_nextForm = _nextForm; }
		QString nextForm( ) const { return m_nextForm; }
		void setAction( const QString &_action ) { m_action = _action; }
		QString action( ) const { return m_action; }

		QString toString( ) const
		{
			return QString( "[nextForm %1, action %2]" ).arg( m_nextForm ).arg( m_action );
		}
		
	private:
		QString m_action;		// action to execute
		QString m_nextForm;		// next form to show
	};
	
	class FormWidget : public QWidget
	{
	Q_OBJECT

	public:
		FormWidget( FormLoader *_formLoader, DataLoader *_dataLoader, QUiLoader *_uiLoader, QWidget *_parent );
		virtual ~FormWidget( );
		void loadForm( QString name );
		void loadLanguage( QString language );
			
	private:
		QString m_name;			// name of the form
		QUiLoader *m_uiLoader;		// UI loader to user for loading designer XML files
		FormLoader *m_formLoader;	// form loader (visible form)
		DataLoader *m_dataLoader;	// load and saves data (data form)
		QWidget *m_ui;			// the ui loaded from the designer XML file
		DataHandler *m_dataHandler;	// form/data handler
		QLocale m_locale;		// current locale to use for form translations
		QHBoxLayout *m_layout;		// main layout swallowing the form
		QSignalMapper *m_signalMapper;	// delegate for form push buttons pointing to forms
		QStringList m_forms;		// names of all currently loaded forms
		
	private:
		void initialize( );
		QString readDynamicStringProperty( QObject *o, const char *name );
	
	signals:
		void formLoaded( QString name );
		void error( QString error );
		
	private slots:
		void formLoaded( QString name, QByteArray form );
		void formLocalizationLoaded( QString name, QByteArray localization );
		void formListLoaded( QStringList forms );
		void dataLoaded( QString name, QByteArray xml );
		void dataSaved( QString name );
		void dataDeleted( QString name );
		void formDomainLoaded( QString form_name, QString widget_name, QByteArray data );

		void switchForm( QObject *object );
		void actionSend( );
		void actionGet( );
		void actionInit( );
		void actionDelete( );
	};
	
} // namespace QtClient
} // namespace _Wolframe

#endif // _FORM_WIDGET_HPP_INCLUDED
