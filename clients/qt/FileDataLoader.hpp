//
// FileDataLoader.hpp
//

#ifndef _FILE_DATA_LOADER_INCLUDED
#define _FILE_DATA_LOADER_INCLUDED

#include "DataLoader.hpp"

class FileDataLoader : public DataLoader
{	
	// intentionally omitting Q_OBJECT here, is done in DataLoader!
	
	public:
		FileDataLoader( QString dir );
		virtual ~FileDataLoader( ) {};

		virtual void request( QString windowName, QString formName, QString widgetName, QByteArray xml, QHash<QString, QString> *props );

	private:
		void handleCreate( QString name, QByteArray data, QHash<QString, QString> *props );
		void handleRead( QString name, QHash<QString, QString> *props );
		void handleUpdate( QString name, QByteArray data, QHash<QString, QString> *props );
		void handleDelete( QString name, QHash<QString, QString> *props );
		void handleDomainDataLoad( QString formName, QString widgetName, QHash<QString, QString> *props );
		
	private:
		QString m_dir;
};

#endif // _FILE_DATA_LOADER_INCLUDED
//
// FileDataLoader.hpp
//
