//
// DataLoader.hpp
//

#ifndef _DATA_LOADER_INCLUDED
#define _DATA_LOADER_INCLUDED

#include <QByteArray>
#include <QString>
#include <QObject>
#include <QHash>

class DataLoader : public QObject
{	
	Q_OBJECT
	
	public:
		virtual ~DataLoader( ) {};
		
		virtual void request( QString formName, QString widgetName, QByteArray xml, QHash<QString, QString> *props ) = 0;
	
	Q_SIGNALS:
		void answer( QString formName, QString widgetName, QByteArray xml );
};

#endif // _DATA_LOADER_INCLUDED
