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
#include <QLabel>
#include <QHash>
#include <QCloseEvent>

class WidgetProperties : public QObject
{
	Q_OBJECT
	
	public:
		WidgetProperties( QHash<QString, QString> *_props = 0 ) : m_props( _props ) { }
	
		QHash<QString, QString> *props( ) { return m_props; }
		
	private:
		QHash<QString, QString> *m_props;
};               

class FormWidget : public QWidget
{
	Q_OBJECT
	Q_PROPERTY( QString form READ form WRITE setForm )

	public:
		enum FormWidgetMode {
			RunMode,		// when running in qtclient
			DesignerMode		// when running in designer
		};
		
		FormWidget( FormLoader *_formLoader, DataLoader *_dataLoader, QUiLoader *_uiLoader, QWidget *_parent = 0, bool _debug = false );
		FormWidget( FormWidgetMode _mode, QWidget *parent = 0 );
		virtual ~FormWidget( );
		void loadForm( QString name );
		void loadLanguage( QString language );
		void setFormLoader( FormLoader *_formLoader );
		void setDataLoader( DataLoader *_dataLoader );
		void setUiLoader( QUiLoader *_uiLoader );
		void loadDelayedForm( const QString &_form );
	
	public:
		QString form( ) const;

		void restoreFromGlobals( QHash<QString, QString> *props );
		
		static QString readDynamicStringProperty( QObject *o, const char *name );
		static void writeDynamicStringProperty( QObject *o, const char *name, const QString &value );
		static void readDynamicStringProperties( QHash<QString, QString> *props, QObject *obj );
		
	public slots:
		void setForm( const QString &_form );
		void reload( );
			
	private:
		FormWidgetMode m_mode;		// run mode of the widget
		QString m_form;			// name of the form
		QString m_previousForm;
		QUiLoader *m_uiLoader;		// UI loader to user for loading designer XML files
		FormLoader *m_formLoader;	// form loader (visible form)
		DataLoader *m_dataLoader;	// load and saves data (data form)
		QWidget *m_ui;			// the ui loaded from the designer XML file
		DataHandler *m_dataHandler;	// form/data handler
		QLocale m_locale;		// current locale to use for form translations
		QHBoxLayout *m_layout;		// main layout swallowing the form
		QSignalMapper *m_signalMapper;	// delegate for form push buttons pointing to forms
		QStringList m_forms;		// names of all currently loaded forms
		QLabel *m_text;			// placeholder of form name in designer mode
		QHash<QString, QString> *m_globals;
		QHash<QString, QString> *m_props; // HACK to communicate props between signal and slot
		bool m_debug;
		
	private:
		void initializeNormal( );
		void initializeDesigner( );
		void sendRequest( QHash<QString, QString> *props );
		void storeToGlobals( QHash<QString, QString> *props );
	
	signals:
		void formLoaded( QString name );
		void error( QString error );
		void closed( );
	
	protected:
		void closeEvent( QCloseEvent *e );

	private slots:
		void formLoaded( QString name, QByteArray form );
		void formLocalizationLoaded( QString name, QByteArray localization );
		void formListLoaded( QStringList forms );
		
		void gotAnswer( QString formName, QString widgetName, QByteArray xml );
		
		void switchForm( QObject *object );
};
	
#endif // _FORM_WIDGET_HPP_INCLUDED
