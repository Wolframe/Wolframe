//
// DataHandler.hpp
//

#ifndef _DATA_HANDLER_INCLUDED
#define _DATA_HANDLER_INCLUDED

#include <QWidget>
#include <QString>
#include <QByteArray>

namespace _Wolframe {
	namespace QtClient {

	class DataHandler
	{	
		public:
			DataHandler( ) {};
			virtual ~DataHandler( ) {};
			void readFormData( QString name, QWidget *form, QByteArray &data );
	};
} // namespace QtClient
} // namespace _Wolframe

#endif // _DATA_HANDLER_INCLUDED
