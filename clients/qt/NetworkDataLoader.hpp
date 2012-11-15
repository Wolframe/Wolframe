//
// NetworkDataLoader.hpp
//

#ifndef _NETWORK_DATA_LOADER_INCLUDED
#define _NETWORK_DATA_LOADER_INCLUDED

#include "DataLoader.hpp"
#include "WolframeClient.hpp"


class NetworkDataLoader : public DataLoader
{	
		// intentionally omitting Q_OBJECT here, is done in DataLoader!
		
		public:
			NetworkDataLoader( WolframeClient *_wolframeClient );
			virtual ~NetworkDataLoader( ) {};

			virtual void request( QString formName, QString widgetName, QByteArray xml, QHash<QString, QString> *props );
					
		private:
			WolframeClient *m_wolframeClient;
};

#endif // _NETWORK_DATA_LOADER_INCLUDED
//
// NetworkDataLoader.hpp
//
