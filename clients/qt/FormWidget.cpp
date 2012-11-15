//
// FormWidget.cpp
//

#include "FormWidget.hpp"
#include "global.hpp"

#include <QDebug>
#include <QTranslator>
#include <QApplication>
#include <QPushButton>
#include <QHBoxLayout>
#include <QFrame>

FormWidget::FormWidget( FormLoader *_formLoader, DataLoader *_dataLoader, QUiLoader *_uiLoader, QWidget *_parent )
	: QWidget( _parent ), m_mode( RunMode ), m_form( ),
	  m_uiLoader( _uiLoader ), m_formLoader( _formLoader ),
	  m_dataLoader( _dataLoader ), m_ui( 0 ), m_dataHandler( 0 ),
	  m_locale( DEFAULT_LOCALE ), m_layout( 0 ), m_forms( )
{
	initializeNormal( );	
}

FormWidget::FormWidget( FormWidgetMode _mode, QWidget *_parent )
	: QWidget( _parent ), m_mode( _mode ), m_form( ),
	  m_uiLoader( 0 ), m_formLoader( 0 ), m_dataLoader( 0 ),
	  m_ui( 0 ), m_dataHandler( 0 ),
	  m_locale( DEFAULT_LOCALE ), m_layout( 0 ), m_forms( )
{
	initializeDesigner( );
}

void FormWidget::initializeDesigner( )
{
	m_layout = new QHBoxLayout( this );
	m_layout->setMargin( 0 );
	
	m_text = new QLabel( m_form.isNull( ) ? tr( "<no name>" ) : m_form );
	m_text->setFrameStyle( QFrame::Box );
	
	m_layout->addWidget( m_text );
}

void FormWidget::initializeNormal( )
{
// maps data between constructed widgets from .ui and the data loader
	m_dataHandler = new DataHandler( m_dataLoader );	

	if( !m_layout ) {
		m_layout = new QHBoxLayout( this );
	}
	
// link the form loader for form loader notifications
	connect( m_formLoader, SIGNAL( formLoaded( QString, QByteArray ) ),
		this, SLOT( formLoaded( QString, QByteArray ) ) );	
	connect( m_formLoader, SIGNAL( formLocalizationLoaded( QString, QByteArray ) ),
		this, SLOT( formLocalizationLoaded( QString, QByteArray ) ) );	
	connect( m_formLoader, SIGNAL( formListLoaded( QStringList ) ),
		this, SLOT( formListLoaded( QStringList ) ) );

// link the data loader to our form widget
	connect( m_dataLoader, SIGNAL( dataCreated( QString ) ),
		this, SLOT( slotDataCreated( QString ) ) );
	connect( m_dataLoader, SIGNAL( dataRead( QString, QByteArray ) ),
		this, SLOT( slotDataRead( QString, QByteArray ) ) );
	connect( m_dataLoader, SIGNAL( dataUpdated( QString ) ),
		this, SLOT( slotDataUpdated( QString ) ) );
	connect( m_dataLoader, SIGNAL( dataDeleted( QString ) ),
		this, SLOT( slotDataDeleted( QString ) ) );

// link the data loader to the data handler
	connect( m_dataLoader, SIGNAL( domainDataLoaded( QString, QString, QByteArray ) ),
		this, SLOT( formDomainLoaded( QString, QString, QByteArray ) ) );

// signal dispatcher for form buttons
	m_signalMapper = new QSignalMapper( this );
	
// the form must be switched after 'action' has been taken in the current form
	connect( m_signalMapper, SIGNAL( mapped( QObject * ) ),
		this, SLOT( switchForm( QObject * ) ) );
}

void FormWidget::formListLoaded( QStringList forms )
{
// remember list of forms, so we can connect buttons to them
	m_forms = forms;
}

void FormWidget::switchForm( QObject *object )
{
	WidgetProperties *widgetProps = qobject_cast<WidgetProperties *>( object );
	QHash<QString, QString> *props = widgetProps->props( );
		
	if( props->contains( "action" ) ) {
		QString action = props->value( "action" );
		if( action == "create" ) {
			actionCreate( props );
		} else if( action == "read" ) {
			actionRead( props );
		} else if( action == "update" ) {
			actionUpdate( props );
		} else if( action == "delete" ) {
			actionDelete( props );
		} else {
			qDebug( ) << "Unknown action" << action;
		}
	}
	
// switch form now, formLoaded will inform parent and others
	if( props->contains( "form" ) ) {
		QString nextForm = props->value( "form" );
		loadForm( nextForm );
	}
}

FormWidget::~FormWidget( )
{
	if( m_ui ) delete m_ui;
	if( m_dataHandler ) delete m_dataHandler;
}

void FormWidget::setFormLoader( FormLoader *_formLoader )
{
	m_formLoader = _formLoader;
	
	loadDelayedForm( m_form );
}

void FormWidget::setDataLoader( DataLoader *_dataLoader )
{
	m_dataLoader = _dataLoader;

	loadDelayedForm( m_form );
}

void FormWidget::setUiLoader( QUiLoader *_uiLoader )
{
	m_uiLoader = _uiLoader;

	loadDelayedForm( m_form );
}

void FormWidget::loadDelayedForm( const QString &_form )
{
	if( !m_formLoader || !m_dataLoader || !m_uiLoader ) return;
	
	loadForm( _form );
}

void FormWidget::setForm( const QString &_form )
{
	switch( m_mode ) {
		case DesignerMode:
			m_form = _form;
			m_text->setText( _form );
			break;
		
		case RunMode:
			loadForm( _form );
			break;
	}
}

QString FormWidget::form( ) const
{
	return m_form;
}

void FormWidget::loadForm( QString name )
{
	if( !m_formLoader ) return;

	if( name == m_form ) return;
	
// indicate busy state
	qApp->setOverrideCursor( Qt::BusyCursor );

	m_form = name;

	qDebug( ) << "Initiating form load for " << m_form;
	
	m_formLoader->initiateFormLoad( m_form );
}	

void FormWidget::loadLanguage( QString language )
{
// indicate busy state
	qApp->setOverrideCursor( Qt::BusyCursor );

	m_locale = QLocale( language );
	
	qDebug( ) << "Initiating form locatization load for " << m_form << " and locale "
		<< m_locale.name( );
		
	m_formLoader->initiateFormLocalizationLoad( m_form, m_locale );
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

QString FormWidget::readDynamicStringProperty( QObject *o, const char *name )
{
	QVariant v = o->property( name );
	if( !v.isValid( ) ) {
		return QString( );
	} else {
		return v.toString( );
	}
}

void FormWidget::readDynamicStringProperties( QHash<QString, QString> *props, QObject *o )
{
	foreach( QByteArray b, o->dynamicPropertyNames( ) ) {
		QString propName = QString::fromAscii( b.data( ) );
		QString propValue = readDynamicStringProperty( o, b.data( ) );
		props->insert( propName, propValue );
	}
}

void FormWidget::formLoaded( QString name, QByteArray formXml )
{
// that's not us
	if( name != m_form ) return;
	
	qDebug( ) << "Form " << name << " loaded";

// read the form and construct it from the UI file
	QWidget *oldUi = m_ui;
	QBuffer buf( &formXml );
	m_ui = m_uiLoader->load( &buf, this );
	if( m_ui == 0 ) {
// something went wrong loading or constructing the form
		m_ui = oldUi;
		emit error( tr( "Unable to create form %1, does it exist?" ).arg( name ) );
		return;
	}
	buf.close( );
	qDebug( ) << "Constructed UI form XML for form" << name;

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
	qDebug( ) << "Starting to load localization for form" << name;
	m_formLoader->initiateFormLocalizationLoad( m_form, m_locale );

// sub form widgets have to be propertly initialized with the data/form and
// ui loaders and their signals have to be wired to our form widget
	qDebug( ) << "Checking for subforms in form" << name << ", wire them as necessary";
	QList<FormWidget *> subforms = m_ui->findChildren<FormWidget *>( );
	foreach( FormWidget *subform, subforms ) {
		subform->setFormLoader( m_formLoader );
		subform->setDataLoader( m_dataLoader );
		subform->setUiLoader( m_uiLoader );
		subform->initializeNormal( );
		subform->m_text->hide( );		
	}

// connect actions and forms
// connect push buttons with form names to loadForms
	qDebug( ) << "Checking form" << name << "for dynamic properties 'form' and 'action'";
	QList<QWidget *> widgets = m_ui->findChildren<QWidget *>( );
	foreach( QWidget *widget, widgets ) {
		QString clazz = widget->metaObject( )->className( ); 
		QString _name = widget->objectName( );
		
		if( clazz == "QPushButton" ) {
			QHash<QString, QString> *props = new QHash<QString, QString>( );
			readDynamicStringProperties( props, widget );
						
			qDebug( ) << "connecting button" << _name << "to properties" << *props;

			QPushButton *pushButton = qobject_cast<QPushButton *>( widget );
			
			connect( pushButton, SIGNAL( clicked( ) ),
				m_signalMapper, SLOT( map( ) ) );

			WidgetProperties *widgetProps = new WidgetProperties( props );
			m_signalMapper->setMapping( pushButton, widgetProps );
		}
	}

// not busy anymore
	qApp->restoreOverrideCursor( );

// check for 'initAction'
	QHash<QString, QString> *props = new QHash<QString, QString>( );
	readDynamicStringProperties( props, m_ui );
	if( props->contains( "initAction" ) ) {
		QString initAction = props->value( "initAction" );
		if( initAction == "create" ) {
			actionCreate( props );
		} else if( initAction == "read" ) {
			actionRead( props );
		} else if( initAction == "update" ) {
			actionUpdate( props );
		} else if( initAction == "delete" ) {
			actionDelete( props );
		} else {
			qDebug( ) << "Unknown init action" << initAction;
		}
	}
		
// signal
	qDebug( ) << "Done loading form" << name;
	emit formLoaded( m_form );
}

void FormWidget::slotDataCreated( QString name )
{	
// that's not us
	if( name != m_form ) return;

	qDebug( ) << "Created data for form " << name;
}

void FormWidget::slotDataRead( QString name, QByteArray xml )
{
// that's not us
	if( name != m_form ) return;

	qDebug( ) << "Loaded data for form " << name << ":\n"
		<< xml;
	
	m_dataHandler->readFormData( name, m_ui, xml );
}

void FormWidget::slotDataUpdated( QString name )
{	
// that's not us
	if( name != m_form ) return;

	qDebug( ) << "Updated data for form " << name;
}

void FormWidget::slotDataDeleted( QString name )
{
// that's not us
	if( name != m_form ) return;

	qDebug( ) << "Deleted data of form " << name;
}

void FormWidget::formDomainLoaded( QString form_name, QString widget_name, QByteArray _data )
{
// that's not us
	if( form_name != m_form ) return;

	m_dataHandler->loadFormDomain( form_name, widget_name, m_ui, _data );
}

void FormWidget::actionCreate( QHash<QString, QString> *props )
{
	qDebug( ) << "Creating data of form " << m_form << "[" << *props << "]";
	
	QByteArray xml;
	m_dataHandler->writeFormData( m_form, m_ui, &xml );
	
	m_dataLoader->initiateDataUpdate( m_form, xml );
}

void FormWidget::actionUpdate( QHash<QString, QString> *props )
{
	qDebug( ) << "Updating data of form " << m_form << "[" << *props << "]";
	
	QByteArray xml;
	m_dataHandler->writeFormData( m_form, m_ui, &xml );
	
	m_dataLoader->initiateDataUpdate( m_form, xml );
}

void FormWidget::actionRead( QHash<QString, QString> *props )
{
	qDebug( ) << "Reading data for form " << m_form << "[" << *props << "]";
	
	m_dataLoader->initiateDataRead( m_form );
}

void FormWidget::actionDelete( QHash<QString, QString> *props )
{
	qDebug( ) << "Sending delete request for form " << m_form << "[" << *props << "]";
	
	// TODO: REQUEST with parameters
}
