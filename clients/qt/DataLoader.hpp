//
// DataLoader.hpp
//

#ifndef _DATA_LOADER_INCLUDED
#define _DATA_LOADER_INCLUDED

#include <QObject>
#include <QString>
#include <QByteArray>


	class DataLoader : public QObject
	{	
		Q_OBJECT
		
		public:
			virtual ~DataLoader( ) {};
			virtual void initiateDataCreate( QString form_name, QByteArray data ) = 0;
			virtual void initiateDataRead( QString form_name ) = 0;
			virtual void initiateDataUpdate( QString form_name, QByteArray data ) = 0;
			virtual void initiateDataDelete( QString form_name ) = 0;
			
			virtual void initiateDomainDataLoad( QString form_name, QString widget_name ) = 0;
			
		signals:
			void dataCreated( QString form_name );
			void dataRead( QString form_name, QByteArray data );
			void dataUpdated( QString form_name );
			void dataDeleted( QString form_name );
			
			void domainDataLoaded( QString form_name, QString widget_name, QByteArray data );
	};

#endif // _DATA_LOADER_INCLUDED
