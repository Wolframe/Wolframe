/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
 All rights reserved.

 This file is part of Project Wolframe.

 Commercial Usage
    Licensees holding valid Project Wolframe Commercial licenses may
    use this file in accordance with the Project Wolframe
    Commercial License Agreement provided with the Software or,
    alternatively, in accordance with the terms contained
    in a written agreement between the licensee and Project Wolframe.

 GNU General Public License Usage
    Alternatively, you can redistribute this file and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Wolframe is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/

#include "FileFormLoader.hpp"
#include "FormCall.hpp"
#include "global.hpp"

#include <QDir>
#include <QFile>
#include <QResource>

#include <QDebug>


FileFormLoader::FileFormLoader( QString formDir, QString localeDir, QString resourcesDir )
	: m_formDir( formDir ), m_localeDir( localeDir ), m_resourcesDir( resourcesDir )
{
	initialize( );
}

void FileFormLoader::initialize( )
{
	// register all resources in the resources form dir to the application
	QDir resourcesDir( m_resourcesDir );
	QStringList nameFilters;
	nameFilters << "*.rcc";
	resourcesDir.setNameFilters( nameFilters );
	QFileInfoList resources = resourcesDir.entryInfoList( QDir::Files | QDir::NoDotAndDotDot, QDir::Name );
	foreach( QFileInfo resource, resources ) {
		QString filename = resource.filePath( );
		qDebug( ) << "Loading resource file" << filename;
		if( !QResource::registerResource( filename ) ) {
			qWarning( ) << "Unable to open resource file" << filename;
		}
	}
}

void FileFormLoader::initiateListLoad( )
{
	QDir formsDir( m_formDir );
	QStringList forms = formsDir.entryList( QDir::Files | QDir::NoDotAndDotDot, QDir::Name )
		.replaceInStrings( ".ui", "" );
	emit formListLoaded( forms );
}

QByteArray FileFormLoader::readFile( QString name )
{
	qDebug( ) << "Reading file " << name;
	QFile file( name );
	file.open( QFile::ReadOnly );
	QByteArray data = file.readAll( );
	file.close( );
	return data;
}

void FileFormLoader::initiateFormLocalizationLoad( QString &name, QLocale locale )
{
	QByteArray localization = readFile( m_localeDir + "/" + FormCall::name(name) + "." + locale.name( ) + ".qm" );
	
	emit formLocalizationLoaded( name, localization );
}

void FileFormLoader::initiateFormLoad( QString &name )
{
// read directly here and stuff data into the signal
	QByteArray form = readFile( m_formDir + "/" + FormCall::name( name) + ".ui" );

	emit formLoaded( name, form );
}


void FileFormLoader::initiateGetLanguageCodes( )
{
	QStringList languageCodes;
	languageCodes.push_back( DEFAULT_LOCALE ); // default locale, always around
	
// read list of supported languages for all forms based on their qm files available
	QDir translationDir( m_localeDir );
	translationDir.setFilter( QDir::Files | QDir::NoDotAndDotDot );
	translationDir.setSorting( QDir::Name );
	QStringList filters;
	filters << "*.qm";
	translationDir.setNameFilters( filters );
	QStringList localeFiles = translationDir.entryList( );
	QMutableStringListIterator it( localeFiles );
	while( it.hasNext( ) ) {
		it.next( );
		QStringList parts = it.value( ).split( "." );
		languageCodes.push_back( parts[1] );		
	}
	
	languageCodes.removeDuplicates( );

	emit languageCodesLoaded( languageCodes );
}

void FileFormLoader::initiateFormSave( QString name, QByteArray form )
{
}

void FileFormLoader::initiateFormLocalizationSave( QString name, QLocale locale, QByteArray localizationSrc, QByteArray localizationBin )
{
}
			
void FileFormLoader::initiateFormDelete( QString name )
{
}

void FileFormLoader::initiateFormLocalizationDelete( QString name, QLocale locale )
{
}

