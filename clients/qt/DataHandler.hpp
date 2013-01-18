//
// DataHandler.hpp
//

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
		void loadFormDomains( QString form_name, QWidget *form, QString name );
		QString readFormVariable( QString variable, QWidget *form );

	private:
		void writeWidgets( QWidget *_parent, QXmlStreamWriter &xml, QHash<QString, QString> *props, QSet<QWidget *> *set );
		void resetWidgetData( QWidget *widget, QString name );
		void loadFormDomains( QString form_name, QWidget *form, QWidget *widget, QString name );
	
	private:
		DataLoader *m_dataLoader;
		FormWidget *m_formWidget;
		bool m_debug;
};

#endif // _DATA_HANDLER_INCLUDED
