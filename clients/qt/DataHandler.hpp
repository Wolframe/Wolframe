//
// DataHandler.hpp
//

#ifndef _DATA_HANDLER_INCLUDED
#define _DATA_HANDLER_INCLUDED

#include <QWidget>
#include <QString>
#include <QByteArray>
#include <QHash>

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
		void loadFormDomain( QString form_name, QString widget_name, QWidget *form, QByteArray &data );
		void loadFormDomains( QString name, QWidget *form );
		QString readFormVariable( QString variable, QWidget *form );
	
	private:
		DataLoader *m_dataLoader;
};

#endif // _DATA_HANDLER_INCLUDED
