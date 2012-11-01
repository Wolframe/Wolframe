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

namespace _Wolframe {
	namespace QtClient {

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

		private:
			WolframeClient *m_wolframeClient;

	};
} // namespace QtClient
} // namespace _Wolframe

#endif // _NETWORK_FORM_LOADER_HPP_INCLUDED
