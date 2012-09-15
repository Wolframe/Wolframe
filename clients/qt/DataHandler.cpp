//
// DataHandler.cpp
//

#include "DataHandler.hpp"

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QWidget>
#include <QDebug>

#include <QLineEdit>
#include <QDateEdit>
#include <QComboBox>

namespace _Wolframe {
	namespace QtClient {

void DataHandler::writeFormData( QString form_name, QWidget *form, QByteArray *data )
{
	QXmlStreamWriter xml( data );
	xml.setAutoFormatting( true );
	xml.setAutoFormattingIndent( 2 );
	
	xml.writeStartDocument( );
	xml.writeStartElement( form_name );

	QList<QWidget *> widgets = form->findChildren<QWidget *>( );
	foreach( QWidget *widget, widgets ) {
		QString clazz = widget->metaObject( )->className( ); 
		QString name = widget->objectName( );
		
// ignore internal elements
		if( name == "" || name.startsWith( "qt_" ) ||
			clazz == "QLabel" ) {
			continue;
		}
		
		if( clazz == "QLineEdit" ) {
			QLineEdit *lineEdit = qobject_cast<QLineEdit *>( widget );
			QString text = lineEdit->text( );
			xml.writeTextElement( "", name, text );
		} else if( clazz == "QDateEdit" ) {
			QDateEdit *dateEdit = qobject_cast<QDateEdit *>( widget );
			QString text = dateEdit->date( ).toString( Qt::ISODate );
			xml.writeTextElement( "", name, text );
		} else if( clazz == "QComboBox" ) {
			QComboBox *comboBox = qobject_cast<QComboBox *>( widget );
			QString text = comboBox->itemText( comboBox->currentIndex( ) );
			xml.writeTextElement( "", name, text );
		}
		qDebug( ) << clazz << name;
	}
	
	xml.writeEndElement( );
	xml.writeEndDocument( );
}

void DataHandler::readFormData( QString name, QWidget *form, QByteArray &data )
{
	QXmlStreamReader xml( data );
	QWidget *widget = 0;
	bool inForm = false;
	QString clazz;
	
	while( !xml.atEnd( ) ) {
		xml.readNext( );
		if( xml.isStartElement( ) && xml.name( ) == name ) {
			inForm = true;
		} else if( xml.isEndElement( ) && xml.name( ) == name ) {
			inForm = false;
		} else {
			if( inForm ) {
				if( xml.isStartElement( ) ) {
					widget = qFindChild<QWidget *>( form, xml.name( ).toString( ) );
					if( widget ) {
						clazz = widget->metaObject( )->className( ); 
						QString text = xml.readElementText( QXmlStreamReader::ErrorOnUnexpectedElement );
						if( clazz == "QLineEdit" ) {
							QLineEdit *lineEdit = qobject_cast<QLineEdit *>( widget );
							lineEdit->setText( text );
						} else if( clazz == "QDateEdit" ) {
							QDateEdit *dateEdit = qobject_cast<QDateEdit *>( widget );
							dateEdit->setDate( QDate::fromString( text, Qt::ISODate ) );
						} else if( clazz == "QComboBox" ) {
							QComboBox *comboBox = qobject_cast<QComboBox *>( widget );
							for( int idx = 0; idx < comboBox->count( ); idx++ ) {
								if( comboBox->itemText( idx ) == text ) {
									comboBox->setCurrentIndex( idx );
									break;
								}
							}
						}
					}
				}
			}
		}
	}
	if( xml.hasError( ) ) {
		qDebug( ) << xml.errorString( );
	}
}

} // namespace QtClient
} // namespace _Wolframe
