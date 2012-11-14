//
// NetworkFormLoader.cpp
//

#include "NetworkFormLoader.hpp"
#include "global.hpp"

#include <QDir>
#include <QFile>

#include <QDebug>


NetworkFormLoader::NetworkFormLoader( WolframeClient *_wolframeClient )
	: m_wolframeClient( _wolframeClient )
{
}

void NetworkFormLoader::initiateListLoad( )
{
	QStringList forms;

	//~ QString s = m_wolframeClient->syncRun( "getForms" );
	//~ if( s.isNull( ) ) {
		//~ qWarning( ) << "Trouble getting list of forms";
	//~ } else {
		//~ QStringList lines = s.split( "\n", QString::SkipEmptyParts );
		//~ forms.append( lines );
		//~ forms.removeDuplicates( );
	//~ }

	emit formListLoaded( forms );
}

void NetworkFormLoader::initiateFormLocalizationLoad( QString &name, QLocale locale )
{
	//~ QString s = m_wolframeClient->syncRun( "getFormLocalization " + name + " " + locale.name( ) );
	//~ if( s.isNull( ) ) {
		//~ qWarning( ) << "Trouble loading localization for form" << name << "and locale" << locale;
	//~ } else {
		//~ // TODO: encoding of binary data? or transfered binary?
		//~ emit formLocalizationLoaded( name, s.toUtf8( ) );
	//~ }
}

void NetworkFormLoader::initiateFormLoad( QString &name )
{
	//~ QString s = m_wolframeClient->syncRun( "getForm " + name );
	//~ if( s.isNull( ) ) {
		//~ qWarning( ) << "Trouble loading form" << name;
	//~ } else {
		//~ emit formLoaded( name, s.toUtf8( ) );
	//~ }
}

void NetworkFormLoader::initiateGetLanguageCodes( )
{
	QStringList languageCodes;
	languageCodes.push_back( DEFAULT_LOCALE ); // default locale, always around
	
	//~ QString s = m_wolframeClient->syncRun( "getFormLanguages" );
	//~ if( s.isNull( ) ) {
		//~ qWarning( ) << "Trouble getting form languages";
	//~ } else {
		//~ QStringList lines = s.split( "\n", QString::SkipEmptyParts );
		//~ languageCodes.append( lines );
		//~ languageCodes.removeDuplicates( );
	//~ }
	
	emit languageCodesLoaded( languageCodes );
}

void NetworkFormLoader::initiateFormSave( QString name, QByteArray form )
{
}

void NetworkFormLoader::initiateFormLocalizationSave( QString name, QLocale locale, QByteArray localizationSrc, QByteArray localizationBin )
{
}
			
void NetworkFormLoader::initiateFormDelete( QString name )
{
}

void NetworkFormLoader::initiateFormLocalizationDelete( QString name, QLocale locale )
{
}

