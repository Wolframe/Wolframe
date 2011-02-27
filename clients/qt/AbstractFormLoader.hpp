//
// AbstractFormLoader.hpp
//

#ifndef _ABSTRACT_FORM_LOADER_HPP_INCLUDED
#define _ABSTRACT_FORM_LOADER_HPP_INCLUDED

#include <QObject>
#include <QString>
#include <QStringList>
#include <QByteArray>

namespace _Wolframe {
	namespace QtClient {

	class AbstractFormLoader : public QObject
	{	
		Q_OBJECT
		
		public:
			virtual ~AbstractFormLoader( ) {};
			virtual void initiateListLoad( ) = 0;
			virtual void initiateFormLoad( QString &name ) = 0;
			virtual QStringList getFormNames( ) = 0;

		Q_SIGNALS:
			void formListLoaded( );
			void formLoaded( QString form_name, QByteArray form );
	};
} // namespace QtClient
} // namespace _Wolframe

#endif // _ABSTRACT_FORM_LOADER_HPP_INCLUDED
