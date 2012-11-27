//
// Preferences->cpp
//

#include "Preferences.hpp"
#include "global.hpp"

#include <QMetaEnum>
#include <QDebug>
#include <QLocale>
 

QScopedPointer<Preferences> Preferences::m_instance;
QString Preferences::m_fileName;

Preferences::Preferences( QString _organization, QString _application, QObject *_parent )
	: QObject( _parent ), m_organization( _organization ), m_application( _application )
{
	loadSettings( );
}

Preferences::~Preferences( )
{
}

bool Preferences::exists( )
{
	QSettings *s = createSettings( );
	bool e = s->contains( "wolframe/locale" );
	delete s;
	return e;
}

void Preferences::setFileName( const QString &_fileName )
{
	m_fileName = _fileName;
}

QSettings *Preferences::createSettings( )
{
	QSettings *s;
	if( m_fileName.isNull( ) ) {
		s = new QSettings( m_organization, m_application );
		qDebug( ) << "Using default settings file";
	} else {
		s = new QSettings( m_fileName, QSettings::IniFormat );
		qDebug( ) << "Using file " << m_fileName << "as settings file";
	}
	
	return s;
}

void Preferences::loadSettings( )
{
	QSettings *s = createSettings( );
	m_host = s->value( "wolframe/host", "localhost" ).toString( );
	m_port = s->value( "wolframe/port", 7661 ).toString( ).toUShort( );
	m_secure = s->value( "wolframe/secure", false ).toBool( );
	m_checkSSL = s->value( "wolframe/checkSSL", true ).toBool( );
	m_clientCertFile = s->value( "wolframe/client-cert-file", "./certs/client.crt" ).toString( );
	m_clientKeyFile = s->value( "wolframe/client-key-file", "./private/client.key" ) .toString( );
	m_CACertFile = s->value( "wolframe/ca-cert-file", "./certs/CAclient.cert.pem" ).toString( );
	const QMetaObject &mo = Preferences::staticMetaObject;
	int idx = mo.indexOfEnumerator( "LoadMode" );
	QMetaEnum metaEnum = mo.enumerator( idx );
	m_uiLoadMode = static_cast< LoadMode >( metaEnum.keyToValue( s->value( "wolframe/uiloadmode", "Network" ).toString( ).toStdString( ).c_str( ) ) );
	m_dataLoadMode = static_cast< LoadMode >( metaEnum.keyToValue( s->value( "wolframe/dataloadmode", "Network" ).toString( ).toStdString( ).c_str( ) ) );
	m_dbName = s->value( "wolframe/dbname", DEFAULT_SQLITE_FILENAME ).toString( );
	m_debug = s->value( "wolframe/debug", false ).toBool( );
	m_uiFormsDir = s->value( "wolframe/uiFormDir", DEFAULT_UI_FORMS_DIR ).toString( );
	m_uiFormTranslationsDir = s->value( "wolframe/uiFormTranslationsDir", DEFAULT_UI_FORM_TRANSLATIONS_DIR ).toString( );
	m_dataLoaderDir = s->value( "wolframe/dataLoaderDir", DEFAULT_DATA_LOADER_DIR ).toString( );
	m_locale = s->value( "wolframe/locale", DEFAULT_LOCALE ).toString( );
	delete s;
}

void Preferences::storeSettings( )
{
	QSettings *s = createSettings( );
	s->setValue( "wolframe/host", m_host );
	s->setValue( "wolframe/port", m_port );
	s->setValue( "wolframe/secure", m_secure );
	s->setValue( "wolframe/checkSSL", m_checkSSL );
	s->setValue( "wolframe/client-cert-file", m_clientCertFile );
	s->setValue( "wolframe/client-key-file", m_clientKeyFile );
	s->setValue( "wolframe/ca-cert-file", m_CACertFile );
	const QMetaObject &mo = Preferences::staticMetaObject;
	int idx = mo.indexOfEnumerator( "LoadMode" );
	QMetaEnum metaEnum = mo.enumerator( idx );
	s->setValue( "wolframe/uiloadmode", metaEnum.valueToKey( m_uiLoadMode ) );
	s->setValue( "wolframe/dataloadmode", metaEnum.valueToKey( m_dataLoadMode ) );
	s->setValue( "wolframe/dbname", m_dbName );
	s->setValue( "wolframe/debug", m_debug );
	s->setValue( "wolframe/uiFormDir", m_uiFormsDir );
	s->setValue( "wolframe/uiFormTranslationsDir", m_uiFormTranslationsDir );
	s->setValue( "wolframe/dataLoaderDir", m_dataLoaderDir );
	s->setValue( "wolframe/locale", m_locale );
	delete s;
}

Preferences *Preferences::instance( )
{
	if( !m_instance ) {
		m_instance.reset( new Preferences( ) );
	}
	
	return m_instance.data( );
}

