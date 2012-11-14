//
// NetworkFormLoader.hpp
//

#ifndef _NETWORK_FORM_LOADER_HPP_INCLUDED
#define _NETWORK_FORM_LOADER_HPP_INCLUDED

#include <QObject>
#include <QString>
#include <QByteArray>

#include "FormLoader.hpp"
#include "WolframeClient.hpp"


	class NetworkFormLoader : public FormLoader
	{
		// intentionally omitting Q_OBJECT here, is done in FormLoader!

		public:
			NetworkFormLoader( WolframeClient *_client );
			virtual ~NetworkFormLoader( ) {}
			virtual void initiateListLoad( );
			virtual void initiateFormLoad( QString &name );
			virtual void initiateFormLocalizationLoad( QString &name, QLocale locale );
			virtual void initiateGetLanguageCodes( );

			virtual void initiateFormSave( QString name, QByteArray form );
			virtual void initiateFormLocalizationSave( QString name, QLocale locale, QByteArray localizationSrc, QByteArray localizationBin );
			
			virtual void initiateFormDelete( QString name );
			virtual void initiateFormLocalizationDelete( QString name, QLocale locale );

		private:
			WolframeClient *m_wolframeClient;

	};

#endif // _NETWORK_FORM_LOADER_HPP_INCLUDED
