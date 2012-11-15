//
// SqliteDataLoader.hpp
//

#ifndef _SQLITE_DATA_LOADER_INCLUDED
#define _SQLITE_DATA_LOADER_INCLUDED

#include "DataLoader.hpp"


	class SqliteDataLoader : public DataLoader
	{	
		// intentionally omitting Q_OBJECT here, is done in DataLoader!
		
		public:
			SqliteDataLoader( QString m_dbName );
			virtual ~SqliteDataLoader( ) {};

			virtual void initiateDataCreate( QString form_name, QByteArray data );
			virtual void initiateDataRead( QString form_name );
			virtual void initiateDataUpdate( QString form_name, QByteArray data );
			virtual void initiateDataDelete( QString form_name );
			
			virtual void initiateDomainDataLoad( QString form_name, QString widget_name );
		
		private:
			QString m_dbName;
	};

#endif // _SQLITE_DATA_LOADER_INCLUDED
//
// SqliteDataLoader.hpp
//
