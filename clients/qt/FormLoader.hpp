//
// FormLoader.hpp
//

#ifndef _FORM_LOADER_HPP_INCLUDED
#define _FORM_LOADER_HPP_INCLUDED

#include <QObject>
#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QLocale>


	class FormLoader : public QObject
	{	
		Q_OBJECT
		
		public:
			virtual ~FormLoader( ) {};
			virtual void initiateListLoad( ) = 0;
			virtual void initiateFormLoad( QString &name ) = 0;
			virtual void initiateFormLocalizationLoad( QString &name, QLocale locale ) = 0;
			virtual void initiateGetLanguageCodes( ) = 0;
			
			virtual void initiateFormSave( QString name, QByteArray form ) = 0;
			virtual void initiateFormLocalizationSave( QString name, QLocale locale, QByteArray localizationSrc, QByteArray localizationBin ) = 0;
			
			virtual void initiateFormDelete( QString name ) = 0;
			virtual void initiateFormLocalizationDelete( QString name, QLocale locale ) = 0;

		Q_SIGNALS:
			void formListLoaded( QStringList forms );
			void formLoaded( QString form_name, QByteArray form );
			void formLocalizationLoaded( QString form_name, QByteArray localization );
			void languageCodesLoaded( QStringList languages );
			
			void formSaved( QString name );
			void formLocalizationSaved( QString name );
			
			void formDeleted( QString name );
			void formLocalizationDeleted( QString name );
	};

#endif // _FORM_LOADER_HPP_INCLUDED
