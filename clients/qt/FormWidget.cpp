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

#include "FormWidget.hpp"
#include "FormCall.hpp"
#include "WidgetMessageDispatcher.hpp"
#include "global.hpp"

#include <QDebug>
#include <QApplication>
#include <QPushButton>
#include <QHBoxLayout>
#include <QFrame>
#include <QList>

FormWidget::FormWidget( FormLoader *_formLoader, DataLoader *_dataLoader, QUiLoader *_uiLoader, QWidget *_parent, bool _debug )
	: QWidget( _parent ), m_form( ),
	  m_uiLoader( _uiLoader ), m_formLoader( _formLoader ),
	  m_dataLoader( _dataLoader ), m_ui( 0 ), m_dataHandler( 0 ),
	  m_locale( DEFAULT_LOCALE ), m_layout( 0 ), m_forms( ),
	  m_globals( 0 ), m_props( 0 ), m_debug( _debug ), m_modal( false )
{
	initialize( );	
}

void FormWidget::initialize( )
{
// maps data between constructed widgets from .ui and the data loader
	m_dataHandler = new DataHandler( m_dataLoader, this, m_debug );

// the global map to pass variables between forms
	m_globals = new QHash< QString, QString >( );

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
	connect( m_dataLoader, SIGNAL( answer( const QByteArray&, const QByteArray& ) ),
		this, SLOT( gotAnswer( const QByteArray&, const QByteArray& ) ) );
	connect( m_dataLoader, SIGNAL( error( const QByteArray&, const QByteArray& ) ),
		this, SLOT( gotError( const QByteArray&, const QByteArray& ) ) );

// signal dispatcher for form buttons
	m_signalMapper = new QSignalMapper( this );
	
// the form must be switched after 'action' has been taken in the current form
	connect( m_signalMapper, SIGNAL( mapped( QWidget * ) ),
		this, SLOT( switchForm( QWidget * ) ) );
}

void FormWidget::formListLoaded( QStringList forms )
{
// remember list of forms, so we can connect buttons to them
	m_forms = forms;
}

void FormWidget::switchForm( QWidget *actionwidget )
{
	WidgetVisitor visitor( actionwidget);

	QVariant doctype = visitor.property( "doctype");
	if (doctype.isValid())
	{
		WidgetMessageDispatcher dispatcher( visitor);
		WidgetMessageDispatcher::Request request = dispatcher.getFormActionRequest( m_debug);
		m_dataLoader->datarequest( request.tag, request.content);
	}
	
	// switch form now, formLoaded will inform parent and others
	QVariant formlink = visitor.property( "form");
	doFormCloseInititalizations( actionwidget);
	if (formlink.isValid())
	{
		QString nextForm = formlink.toString();
		if( m_modal && nextForm == "_CLOSE_" ) {
			emit closed( );
		} else {
			loadForm( nextForm );
		}
	}
}

void FormWidget::reload( )
{
	loadForm( m_form );
}

FormWidget::~FormWidget( )
{
	if( m_ui ) delete m_ui;
	if( m_dataHandler ) delete m_dataHandler;
	if( !m_modal && m_globals ) delete m_globals;
}

QHash<QString, QString> *FormWidget::globals( ) const
{
	return m_globals;
}

void FormWidget::setGlobals( QHash<QString, QString> *_globals )
{
	delete m_globals;
	m_globals = _globals;
}

void FormWidget::setForm( const QString &_form )
{
	loadForm( _form );
}

QString FormWidget::form( ) const
{
	return m_form;
}

QIcon FormWidget::getWindowIcon( ) const
{
	return m_ui->windowIcon( );
}	

void FormWidget::loadForm( QString name, bool modal )
{
	if( !m_formLoader ) return;
	
	m_previousForm = m_form;
	m_form = name;
	m_modal = modal;

	qDebug( ) << "Initiating form load for " << m_form << m_modal;
	
	m_formLoader->initiateFormLoad( m_form );
}	

void FormWidget::setLocale( QLocale locale )
{
	m_locale = locale;
}

void FormWidget::setLanguage( QString language )
{
	setLocale( QLocale( language ) );
}

void FormWidget::changeEvent( QEvent* _event )
{
	if( _event ) {
		switch( _event->type( ) ) {			
			case QEvent::LanguageChange:
				m_ui->update( );
				break;
		}
	}

	QWidget::changeEvent( _event );
}

void FormWidget::formLocalizationLoaded( QString name, QByteArray localization )
{
	qDebug( ) << "Form localization loaded for " << name
		<< ", locale " << m_locale.name( )
		<< ", size " << localization.length( );

	qApp->removeTranslator( &m_translator );
	
	if( m_locale.name( ) != DEFAULT_LOCALE ) {
		if( !m_translator.load( (const uchar *)localization.constData( ), localization.length( ) ) ) {
			qWarning( ) << "Error while loading translations for form " <<
				name << " for locale " << m_locale.name( );
			return;
		}
		qApp->installTranslator( &m_translator );
	}

	QEvent ev( QEvent::LanguageChange );
	qApp->sendEvent( qApp, &ev );
	
// signal completion of form loading
	qDebug( ) << "Done loading form" << name;
	emit formLoaded( m_form );
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

void FormWidget::writeDynamicStringProperty( QObject *o, const char *name, const QString &value )
{
	QVariant v( value );
	o->setProperty( name, v );
	qDebug( ) << "setting dynamic property" << name << ":" << value << "in" << o;
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
	FormCall formCall( name);

// read the form and construct it from the UI file
	QWidget *oldUi = m_ui;
	QBuffer buf( &formXml );
	m_ui = m_uiLoader->load( &buf, this );
	if( m_ui == 0 ) {
// something went wrong loading or constructing the form
		m_ui = oldUi;
		m_form = m_previousForm;
		emit error( tr( "Unable to load form '%1', does it exist?" ).arg( name ) );
		return;
	}
	buf.close( );
	qDebug( ) << "Constructed UI form XML for form" << name << m_modal;
	
// if we have a modal dialog, we must not replace our own form, but emit
// a signal, so the main window can rearange and load the form modal in
// a new window
	if( !m_modal && m_ui->isModal( ) ) {
		m_ui = oldUi;
		m_form = m_previousForm;
		emit formModal( name );
		return;
	}
// initialize the form variables given by form parameters
	WidgetVisitor visitor( m_ui);
	foreach (const FormCall::Parameter& param, formCall.parameter())
	{
		visitor.setProperty( param.first, param.second);
		qDebug( ) << "Set UI parameter" << param.first << "=" << param.second;
	}

// add new form to layout (which covers the whole widget)
	m_layout->addWidget( m_ui );

	qDebug( ) << "set window title" << m_ui->windowTitle( );
	setWindowTitle( m_ui->windowTitle( ) );

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

// connect push buttons with form names to loadForms
	QList<QWidget *> widgets = m_ui->findChildren<QWidget *>( );
	foreach( QWidget *widget, widgets ) {
		QString clazz = widget->metaObject( )->className( ); 
		
		if( clazz == "QPushButton" ) {
			QPushButton *pushButton = qobject_cast<QPushButton *>( widget );
			
			connect( pushButton, SIGNAL( clicked( ) ),
				m_signalMapper, SLOT( map( ) ) );

			m_signalMapper->setMapping( pushButton, widget );
		}
	}

// reset the form now, this also loads the domains
	doFormInitInititalizations( m_ui);
	//[+]m_dataHandler->resetFormData( m_ui );
	m_dataHandler->loadFormDomains( m_form, m_ui );
	m_dataHandler->loadActionReloadTriggerMap( m_ui);

	WidgetMessageDispatcher dispatcher( m_ui);
	foreach (const WidgetMessageDispatcher::Request& request, dispatcher.getDomainLoadRequests( m_debug))
	{
		m_dataLoader->datarequest( request.tag, request.content);
	}

// load localication of the form now
	qDebug( ) << "Initiating form locatization load for " << m_form << " and locale "
		<< m_locale.name( );		
	m_formLoader->initiateFormLocalizationLoad( m_form, m_locale );
}
		
void FormWidget::gotAnswer( const QByteArray& tag_, const QByteArray& data_)
{
	qDebug() << "got answer tag=" << tag_ << "data=" << data_;
	QByteArray actiontag = WidgetMessageDispatcher::getActionId( tag_);
	if (actiontag.isEmpty())
	{
		WidgetMessageDispatcher dispatcher( m_ui);
		dispatcher.feedResult( tag_, data_);
	}
	else
	{
		// find all forms with a domain load that is bound over a trigger to the action completed
		// and issue a domain load request for them:
		WidgetVisitor visitor( WidgetVisitor(m_ui).uirootwidget());
		foreach (const QByteArray& doctype, m_dataHandler->getTriggeredDoctypes( actiontag))
		{
			foreach (QWidget* wdg, visitor.findDoctypeWidgets( doctype))
			{
				WidgetMessageDispatcher dispatcher( m_ui);
				WidgetMessageDispatcher::Request request = dispatcher.getDomainLoadRequest( m_debug);
				m_dataLoader->datarequest( request.tag, request.content);
			}
		}
	}
}

void FormWidget::gotError( const QByteArray& tag_, const QByteArray& data_)
{
	qDebug() << "got error tag=" << tag_ << "data=" << data_;
	emit error( QString( data_));
}

void FormWidget::closeEvent( QCloseEvent *e )
{
	emit closed( );
	e->accept( );
}
