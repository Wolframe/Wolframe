//
// SqliteFormLoader.hpp
//

#ifndef _SQLITE_FORM_LOADER_HPP_INCLUDED
#define _SQLITE_FORM_LOADER_HPP_INCLUDED

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QSqlDatabase>

#include "FormLoader.hpp"

#define VERSION_MAJOR 0
#define VERSION_MINOR 0

namespace _Wolframe {
	namespace QtClient {

	class Version
	{
		public:
			Version( )
				: m_major( -1 ), m_minor( -1 )
			{
			}
			
			Version( int major, int minor )
				: m_major( major ), m_minor( minor )
			{
			}
			
			bool valid( )
			{
				return m_major != -1 && m_minor != -1;
			}
			
			static Version invalidVersion( )
			{
				return Version( );
			}
			
			QString toString( )
			{
				if( valid( ) ) {
					return QString( "%1.%2" ).arg( m_major ).arg( m_minor );
				} else {
					return QString( "<invalid version>" );
				}
			}
			
		private:
			int m_major;
			int m_minor;
			
			int major( ) { return m_major; }
			int minor( ) { return m_minor; }			
	};

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
