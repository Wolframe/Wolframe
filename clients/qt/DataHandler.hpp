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

class DataHandler : public QObject
{
	Q_OBJECT
	
	public:
		DataHandler( DataLoader *_dataLoader );
		virtual ~DataHandler( ) {};
		void readFormData( QString name, QWidget *form, QByteArray &data, QHash<QString, QString> *props );
		void writeFormData( QString name, QWidget *form, QByteArray *data, QHash<QString, QString> *props );
		void resetFormData( QWidget *form );
		void loadFormDomain( QString form_name, QString widget_name, QWidget *form, QByteArray &data, QHash<QString, QString> *props );
		void loadFormDomains( QString name, QWidget *form );
		QString readFormVariable( QString variable, QWidget *form );

	private:
		void writeWidgets( QWidget *_parent, QXmlStreamWriter &xml, QHash<QString, QString> *props, QSet<QWidget *> *set );
	
	private:
		DataLoader *m_dataLoader;
};

#endif // _DATA_HANDLER_INCLUDED
