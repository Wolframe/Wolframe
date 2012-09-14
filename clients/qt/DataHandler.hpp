//
// DataHandler.hpp
//

#ifndef _DATA_HANLDER_INCLUDED
#define _DATA_HANLDER_INCLUDED

#include <QObject>
#include <QString>
#include <QStringList>
#include <QByteArray>

namespace _Wolframe {
	namespace QtClient {

	class DataHandler : public QObject
	{	
		Q_OBJECT
		
		public:
			virtual ~DataHandler( ) {};
			virtual void initiateDataLoad( QString form_name ) = 0;
			virtual void initiateDataSave( QString form_name, QByteArray data ) = 0;

		Q_SIGNALS:
			void dataLoaded( QString form_name, QByteArray data );
			void dataSaved( QString form_name );
	};
} // namespace QtClient
} // namespace _Wolframe

#endif // _DATA_HANLDER_INCLUDED
