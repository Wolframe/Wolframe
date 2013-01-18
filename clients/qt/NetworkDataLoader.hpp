//
// NetworkDataLoader.hpp
//

#ifndef _NETWORK_DATA_LOADER_INCLUDED
#define _NETWORK_DATA_LOADER_INCLUDED

#include "DataLoader.hpp"
#include "WolframeClient.hpp"

#include <QHash>
#include <QPair>
#include <QScopedPointer>
	
class NetworkDataLoader : public DataLoader
{	
	// intentionally omitting Q_OBJECT here, is done in DataLoader!
	
	public:
		NetworkDataLoader( WolframeClient *_wolframeClient, bool _debug = false );
		virtual ~NetworkDataLoader( ) {};

		virtual void request( QString windowName, QString formName, QString widgetName, QByteArray xml, QHash<QString, QString> *props );

	private:
		void handleCreate( QString windowName, QString name, QByteArray data, QHash<QString, QString> *props );
		void handleRead( QString windowName, QString name, QHash<QString, QString> *props );
		void handleUpdate( QString windowName, QString name, QByteArray data, QHash<QString, QString> *props );
		void handleDelete( QString windowName, QString name, QHash<QString, QString> *props );
		void handleDomainDataLoad( QString windowName, QString formName, QString widgetName, QHash<QString, QString> *props );
		QString mapAction( QString action );
		QString mapDoctype( QString action, bool domain, QString docType );
		
	public slots:
		virtual void gotAnswer( QStringList params, QString content );
		virtual void gotError( QString error );
			
	private:
		WolframeClient *m_wolframeClient;
		QScopedPointer<QHash<QString, QPair<QString, QString> > > m_map;
		bool m_debug;
};

#endif // _NETWORK_DATA_LOADER_INCLUDED
//
// NetworkDataLoader.hpp
//
