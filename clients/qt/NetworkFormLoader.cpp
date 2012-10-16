//
// NetworkFormLoader.cpp
//

#include "NetworkFormLoader.hpp"

#include <QDir>
#include <QFile>

#include <QDebug>

namespace _Wolframe {
	namespace QtClient {

NetworkFormLoader::NetworkFormLoader( WolframeClient *_wolframeClient )
	: m_wolframeClient( _wolframeClient )
{
}

void NetworkFormLoader::initiateListLoad( )
{
	QStringList forms;

	emit formListLoaded( forms );
	
//~ // read list of forms directory on demand
	//~ QDir formsDir( QLatin1String( "forms" ) );
	//~ QStringList forms = formsDir.entryList( QDir::Files | QDir::NoDotAndDotDot, QDir::Name )
		//~ .replaceInStrings( ".ui", "" );
}

void NetworkFormLoader::initiateFormLocalizationLoad( QString &name, QLocale locale )
{
	//~ QByteArray localization = readFile( m_localeDir + "/" + name + "." + locale.name( ) + ".qm" );
	//~ 
	//~ emit formLocalizationLoaded( name, localization );
}

void NetworkFormLoader::initiateFormLoad( QString &name )
{
	QString s = m_wolframeClient->syncRun( "getForm " + name );
	
	emit formLoaded( name, s.toUtf8( ) );
}

void NetworkFormLoader::initiateGetLanguageCodes( )
{
	QStringList languageCodes;
	languageCodes.push_back( "en_US" ); // default locale, always around
	
	QString s = m_wolframeClient->syncRun( "getFormLanguages" );
	if( s.isNull( ) ) {
		qWarning( ) << "Trouble getting form languages";
	} else {
		QStringList lines = s.split( "\n", QString::SkipEmptyParts );
		languageCodes.append( lines );
		languageCodes.removeDuplicates( );
		qDebug( ) << s << ": " << lines << ": " << languageCodes;
	}
	
	emit languageCodesLoaded( languageCodes );
}

} // namespace QtClient
} // namespace _Wolframe
