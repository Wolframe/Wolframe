//
// DataLoader.hpp
//

#ifndef _DATA_LOADER_INCLUDED
#define _DATA_LOADER_INCLUDED

#include <QObject>
#include <QString>
#include <QByteArray>

namespace _Wolframe {
	namespace QtClient {

	class DataLoader : public QObject
	{	
		Q_OBJECT
		
		public:
			virtual ~DataLoader( ) {};
			virtual void initiateDataLoad( QString form_name ) = 0;
			virtual void initiateDataSave( QString form_name, QByteArray data ) = 0;
			virtual void initiateDomainDataLoad( QString form_name, QString widget_name ) = 0;

		signals:
			void dataLoaded( QString form_name, QByteArray data );
			void dataSaved( QString form_name );		
			void domainDataLoaded( QString form_name, QString widget_name, QByteArray data );
	};
} // namespace QtClient
} // namespace _Wolframe

#endif // _DATA_LOADER_INCLUDED
