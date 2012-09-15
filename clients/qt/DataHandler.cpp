//
// DataHandler.cpp
//

#include "DataHandler.hpp"

#include <QXmlStreamReader>
#include <QWidget>
#include <QDebug>

#include <QLineEdit>
#include <QDateEdit>
#include <QComboBox>

namespace _Wolframe {
	namespace QtClient {

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
							QLineEdit *lineEdit = dynamic_cast<QLineEdit *>( widget );
							lineEdit->setText( text );
						} else if( clazz == "QDateEdit" ) {
							QDateEdit *dateEdit = dynamic_cast<QDateEdit *>( widget );
							dateEdit->setDate( QDate::fromString( text, Qt::ISODate ) );
						} else if( clazz == "QComboBox" ) {
							QComboBox *comboBox = dynamic_cast<QComboBox *>( widget );
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
