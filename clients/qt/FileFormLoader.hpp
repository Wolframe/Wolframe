//
// FileFormLoader.hpp
//

#ifndef _FILE_FORM_LOADER_HPP_INCLUDED
#define _FILE_FORM_LOADER_HPP_INCLUDED

#include <QObject>
#include <QString>
#include <QByteArray>

#include "FormLoader.hpp"

namespace _Wolframe {
	namespace QtClient {

	class FileFormLoader : public FormLoader
	{
		// intentionally omitting Q_OBJECT here, is done in FormLoader!
		
		public:
			FileFormLoader( QString formDir, QString localeDir );
			virtual ~FileFormLoader( ) {};
			virtual void initiateListLoad( );
			virtual void initiateFormLoad( QString &name, QLocale locale );
			virtual QStringList getFormNames( );
		
		private:
			QString m_formDir;
			QString m_localeDir;
			
			QByteArray readFile( QString fileName );

	};
} // namespace QtClient
} // namespace _Wolframe

#endif // _FILE_FORM_LOADER_HPP_INCLUDED
