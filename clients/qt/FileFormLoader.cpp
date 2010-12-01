#include "FileFormLoader.hpp"

#include <QDir>

namespace _SMERP {
	namespace QtClient {

FileFormLoader::FileFormLoader( QString dir ) : m_dir( dir ) 
{
}

void FileFormLoader::initiateListLoad( )
{
	// doing nothing here, deferred to getFormNames on demand
	emit formListLoaded( );
}

void FileFormLoader::initiateFormLoad( QString &name )
{
	// just remembering the name here, deferring to getFormRef
	emit formLoaded( name, 0 );
}

QStringList FileFormLoader::getFormNames( )
{
	QDir forms_dir( QLatin1String( "forms" ) );
	QStringList forms = forms_dir.entryList( QDir::Files | QDir::NoDotAndDotDot, QDir::Name );
	return forms;
}

QString FileFormLoader::getFormRef( QString &name )
{
}

} // namespace QtClient
} // namespace _SMERP
