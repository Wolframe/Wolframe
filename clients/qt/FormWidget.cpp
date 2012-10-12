//
// FormWidget.cpp
//

#include "FormWidget.hpp"

#include "FileFormLoader.hpp"
#include "FileDataLoader.hpp"

#include <QDebug>
#include <QTranslator>
#include <QApplication>

namespace _Wolframe {
	namespace QtClient {

FormWidget::FormWidget( FormLoader *_formLoader, QUiLoader *_uiLoader, QWidget *_parent )
	: QWidget( _parent ), m_uiLoader( _uiLoader ), m_formLoader( _formLoader ), m_ui( 0 ), m_locale( "en_US" )
{
// for now a from file form and data loader (later wolframe protocol)
	m_dataLoader = new FileDataLoader( "data" );

// maps data between constructed widgets from .ui and the data loader
	m_dataHandler = new DataHandler( m_dataLoader );	

	m_layout = new QHBoxLayout( this );
	
	initialize( );	
}

void FormWidget::initialize( )
{
// link the form loader for form loader notifications
	connect( m_formLoader, SIGNAL( formLoaded( QString, QByteArray ) ),
		this, SLOT( formLoaded( QString, QByteArray ) ) );	
	connect( m_formLoader, SIGNAL( formLocalizationLoaded( QString, QByteArray ) ),
		this, SLOT( formLocalizationLoaded( QString, QByteArray ) ) );	

// link the data loader to our form widget
	connect( m_dataLoader, SIGNAL( dataLoaded( QString, QByteArray ) ),
		this, SLOT( dataLoaded( QString, QByteArray ) ) );
	connect( m_dataLoader, SIGNAL( dataSaved( QString ) ),
		this, SLOT( dataSaved( QString ) ) );

// link the data loader to the data handler
	connect( m_dataLoader, SIGNAL( domainDataLoaded( QString, QString, QByteArray ) ),
		this, SLOT( formDomainLoaded( QString, QString, QByteArray ) ) );
}

FormWidget::~FormWidget( )
{
	if( m_ui ) delete m_ui;
	delete m_dataHandler;
	delete m_dataLoader;
}

void FormWidget::loadForm( QString name )
{
// indicate busy state
	qApp->setOverrideCursor( Qt::BusyCursor );

	m_name = name;

	qDebug( ) << "Initiating form load for " << m_name;
	
	m_formLoader->initiateFormLoad( m_name );
}	

void FormWidget::loadLanguage( QString language )
{
// indicate busy state
	qApp->setOverrideCursor( Qt::BusyCursor );

	m_locale = QLocale( language );
	
	qDebug( ) << "Initiating form locatization load for " << m_name << " and locale "
		<< m_locale.name( );
		
	m_formLoader->initiateFormLocalizationLoad( m_name, m_locale );
}

void FormWidget::formLocalizationLoaded( QString name, QByteArray localization )
{
	qDebug( ) << "Form localization loaded for " << name
		<< ", locale " << m_locale.name( )
		<< ", size " << localization.length( );

// get list of all translators for this form and delete them
	const QList<QTranslator *> oldTranslators( m_ui->findChildren<QTranslator *>( ) );
	foreach( QTranslator *translator, oldTranslators ) {
		qDebug( ) << "Removing old translator " << translator;
		QCoreApplication::instance( )->removeTranslator( translator );
	}
	qDeleteAll( oldTranslators );
	
// install translation files for this form
	QTranslator *translator = new QTranslator( m_ui );
	if( !translator->load( (const uchar *)localization.constData( ), localization.length( ) ) ) {
		qDebug( ) << "Error while loading translations for form " <<
			name << " for locale " << m_locale.name( );
	}
	QCoreApplication::instance( )->installTranslator( translator );

// not busy anymore
	qApp->restoreOverrideCursor( );	
}

void FormWidget::formLoaded( QString name, QByteArray form )
{
	qDebug( ) << "Form " << name << " loaded";

// read the form and construct it from the UI file
	QWidget *oldUi = m_ui;
	QBuffer buf( &form );
	m_ui = m_uiLoader->load( &buf, this );
	buf.close( );

// add new form to layout (which covers the whole widget)
	m_layout->addWidget( m_ui );

	if( oldUi ) {
		m_ui->move( oldUi->pos( ) );
		oldUi->hide( );
		oldUi->deleteLater( );
		oldUi->setParent( 0 );
	}
	m_ui->show( );	

// set localization now
	m_formLoader->initiateFormLocalizationLoad( m_name, m_locale );

// initiate load of form data
	qDebug( ) << "Initiating loading of form data for form " << name;
	m_dataLoader->initiateDataLoad( name );

// connect standard form actions
	QMetaObject::connectSlotsByName( this );
	
// not busy anymore
	qApp->restoreOverrideCursor( );
	
// signal
	emit formLoaded( m_name );
}

void FormWidget::dataSaved( QString name )
{	
	qDebug( ) << "Saved data for form " << name;
}

void FormWidget::dataLoaded( QString name, QByteArray xml )
{
	qDebug( ) << "Loaded data for form " << name << ":\n"
		<< xml;
	
	m_dataHandler->readFormData( name, m_ui, xml );
}

void FormWidget::formDomainLoaded( QString form_name, QString widget_name, QByteArray data )
{
	m_dataHandler->loadFormDomain( form_name, widget_name, m_ui, data );
}

void FormWidget::on_buttons_accepted( )
{
	qDebug( ) << "Form" << m_name << " accepted";
	
	QByteArray xml;
	m_dataHandler->writeFormData( m_name, m_ui, &xml );
	
	m_dataLoader->initiateDataSave( m_name, xml );
}

void FormWidget::on_buttons_rejected( )
{
	qDebug( ) << "Form" << m_name << " rejected";
	
	m_dataLoader->initiateDataLoad( m_name );
}

} // namespace QtClient
} // namespace _Wolframe
