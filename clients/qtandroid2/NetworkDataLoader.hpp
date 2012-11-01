//
// NetworkDataLoader.hpp
//

#ifndef _NETWORK_DATA_LOADER_INCLUDED
#define _NETWORK_DATA_LOADER_INCLUDED

#include "DataLoader.hpp"
#include "WolframeClient.hpp"

namespace _Wolframe {
	namespace QtClient {

	class NetworkDataLoader : public DataLoader
	{	
		// intentionally omitting Q_OBJECT here, is done in DataLoader!
		
		public:
			NetworkDataLoader( WolframeClient *_wolframeClient );
			virtual ~NetworkDataLoader( ) {};

			virtual void initiateDataLoad( QString form_name );
			virtual void initiateDataSave( QString form_name, QByteArray data );
			virtual void initiateDomainDataLoad( QString form_name, QString widget_name );
		
		private:
			WolframeClient *m_wolframeClient;
	};
} // namespace QtClient
} // namespace _Wolframe

#endif // _NETWORK_DATA_LOADER_INCLUDED
//
// NetworkDataLoader.hpp
//
