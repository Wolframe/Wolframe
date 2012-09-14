//
// FileDataHandler.hpp
//

#ifndef _FILE_DATA_HANLDER_INCLUDED
#define _FILE_DATA_HANLDER_INCLUDED

#include "DataHandler.hpp"

namespace _Wolframe {
	namespace QtClient {

	class FileDataHandler : public DataHandler
	{	
		// intentionally omitting Q_OBJECT here, is done in DataHandler!
		
		public:
			FileDataHandler( QString dir );
			virtual ~FileDataHandler( ) {};

			virtual void initiateDataLoad( QString form_name );
			virtual void initiateDataSave( QString form_name, QByteArray data );
		
		private:
			QString m_dir;
	};
} // namespace QtClient
} // namespace _Wolframe

#endif // _FILE_DATA_HANLDER_INCLUDED
//
// FileDataHandler.hpp
//
