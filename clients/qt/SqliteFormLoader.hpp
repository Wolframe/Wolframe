//
// SqliteFormLoader.hpp
//

#ifndef _SQLITE_FORM_LOADER_HPP_INCLUDED
#define _SQLITE_FORM_LOADER_HPP_INCLUDED

#include <QObject>
#include <QString>

#include "FormLoader.hpp"
#include "Version.hpp"

#define VERSION_MAJOR 0
#define VERSION_MINOR 0

namespace _Wolframe {
	namespace QtClient {

	class SqliteFormLoader : public FormLoader
	{
		// intentionally omitting Q_OBJECT here, is done in FormLoader!
		
		public:
			SqliteFormLoader( QString dbName );
			virtual ~SqliteFormLoader( ) {};
			virtual void initiateListLoad( );
			virtual void initiateFormLoad( QString &name );
			virtual void initiateFormLocalizationLoad( QString &name, QLocale locale );
			virtual void initiateGetLanguageCodes( );
		
		protected:
			void initialize( );
			Version getSchemaVersion( );
		
		private:
			QString m_dbName;
	};
} // namespace QtClient
} // namespace _Wolframe

#endif // _SQLITE_FORM_LOADER_HPP_INCLUDED
