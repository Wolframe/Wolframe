//
// FileFormLoader.hpp
//

#ifndef _FILE_FORM_LOADER_HPP_INCLUDED
#define _FILE_FORM_LOADER_HPP_INCLUDED

#include <QObject>
#include <QString>
#include <QByteArray>

#include "FormLoader.hpp"


	class FileFormLoader : public FormLoader
	{
		// intentionally omitting Q_OBJECT here, is done in FormLoader!
		
		public:
			FileFormLoader( QString formDir, QString localeDir, QString resourcesDir );
			virtual ~FileFormLoader( ) {};
			virtual void initiateListLoad( );
			virtual void initiateFormLoad( QString &name );
			virtual void initiateFormLocalizationLoad( QString &name, QLocale locale );
			virtual void initiateGetLanguageCodes( );

			virtual void initiateFormSave( QString name, QByteArray form );
			virtual void initiateFormLocalizationSave( QString name, QLocale locale, QByteArray localizationSrc, QByteArray localizationBin );
			
			virtual void initiateFormDelete( QString name );
			virtual void initiateFormLocalizationDelete( QString name, QLocale locale );
		
		private:
			void initialize( );
			
		private:
			QString m_formDir;
			QString m_localeDir;
			QString m_resourcesDir;
			
			QByteArray readFile( QString fileName );

	};

#endif // _FILE_FORM_LOADER_HPP_INCLUDED
