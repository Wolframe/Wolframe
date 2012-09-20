//
// FormWidget.cpp
//

#include "FormWidget.hpp"

#include "FileFormLoader.hpp"
#include "FileDataLoader.hpp"

#include <QDebug>
#include <QHBoxLayout>
#include <QTranslator>
#include <QApplication>

namespace _Wolframe {
	namespace QtClient {

FormWidget::FormWidget( FormLoader *_formLoader, QWidget *_parent )
	: QWidget( _parent ), m_formLoader( _formLoader ), m_ui( 0 )
{
// Qt Designer UI loader, enable automatic language switch handling
	m_uiLoader = new QUiLoader( );
	m_uiLoader->setLanguageChangeEnabled ( true );

// for now a from file form and data loader (later wolframe protocol)
	m_dataLoader = new FileDataLoader( "data" );

// maps data between constructed widgets from .ui and the data loader
	m_dataHandler = new DataHandler( );	
	
	initialize( );	
}

void FormWidget::initialize( )
{
// link the form loader for form loader notifications
	QObject::connect( m_formLoader, SIGNAL( formLoaded( QString, QByteArray, QByteArray ) ),
		this, SLOT( formLoaded( QString, QByteArray, QByteArray ) ) );	

// link the data loader to our form widget
	QObject::connect( m_dataLoader, SIGNAL( dataLoaded( QString, QByteArray ) ),
		this, SLOT( dataLoaded( QString, QByteArray ) ) );
	QObject::connect( m_dataLoader, SIGNAL( dataSaved( QString ) ),
		this, SLOT( dataSaved( QString ) ) );
}

FormWidget::~FormWidget( )
{
	if( m_ui ) delete m_ui;
	delete m_dataHandler;
	delete m_dataLoader;
	delete m_uiLoader;
}

void FormWidget::loadForm( QString name, QLocale locale )
{
// indicate busy state
	qApp->setOverrideCursor( Qt::BusyCursor );

	qDebug( ) << "Initiating form load for " << name;
	m_formLoader->initiateFormLoad( name, locale );
	
	m_locale = locale;
}	

void FormWidget::formLoaded( QString name, QByteArray form, QByteArray localization )
{
	qDebug( ) << "Form " << name << " loaded";

// get list of all translators for this form and delete them
	const QList<QTranslator *> oldTranslators( m_ui->findChildren<QTranslator *>( ) );
	foreach( QTranslator *translator, oldTranslators ) {
		QCoreApplication::instance( )->removeTranslator( translator );
	}
	qDeleteAll( oldTranslators );

// read the form and construct it from the UI file
	QWidget *oldUi = m_ui;
	QBuffer buf( &form );
	m_ui = m_uiLoader->load( &buf, this );
	buf.close( );

// install translation files for this form
	QTranslator *translator = new QTranslator( m_ui );
	if( !translator->load( (const uchar *)localization.constData( ), localization.length( ) ) ) {
		qDebug( ) << "Error while loading translations for form " <<
			name << " for locale " << m_locale;
	}
	QCoreApplication::instance( )->installTranslator( translator );

	QHBoxLayout *_layout = new QHBoxLayout( this );
        _layout->addWidget( m_ui );

	if( oldUi ) {
		m_ui->move( oldUi->pos( ) );
		oldUi->hide( );
		oldUi->deleteLater( );
	}
	m_ui->show( );	

// initiate load of form data
	qDebug( ) << "Initiating loading of form data for form " << name;
	m_dataLoader->initiateDataLoad( name );

// connect standard form actions
	QMetaObject::connectSlotsByName( m_ui );
	
// not busy anymore
	qApp->restoreOverrideCursor( );	
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
