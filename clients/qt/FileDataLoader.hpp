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

			virtual void initiateDataCreate( QString form_name, QByteArray data );
			virtual void initiateDataRead( QString form_name );
			virtual void initiateDataUpdate( QString form_name, QByteArray data );
			virtual void initiateDataDelete( QString form_name );
			
			virtual void initiateDomainDataLoad( QString form_name, QString widget_name );
		
		private:
			QString m_dir;
	};

#endif // _FILE_DATA_LOADER_INCLUDED
//
// FileDataLoader.hpp
//
