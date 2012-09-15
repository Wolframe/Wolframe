//
// FileDataLoader.hpp
//

#ifndef _FILE_DATA_LOADER_INCLUDED
#define _FILE_DATA_LOADER_INCLUDED

#include "DataLoader.hpp"

namespace _Wolframe {
	namespace QtClient {

	class FileDataLoader : public DataLoader
	{	
		// intentionally omitting Q_OBJECT here, is done in DataLoader!
		
		public:
			FileDataLoader( QString dir );
			virtual ~FileDataLoader( ) {};

			virtual void initiateDataLoad( QString form_name );
			virtual void initiateDataSave( QString form_name, QByteArray data );
		
		private:
			QString m_dir;
	};
} // namespace QtClient
} // namespace _Wolframe

#endif // _FILE_DATA_LOADER_INCLUDED
//
// FileDataLoader.hpp
//
