//
// DataHandler.cpp
//

#include "DataHandler.hpp"

#include <QWidget>
#include <QDebug>

#include <QLineEdit>
#include <QDateEdit>
#include <QTimeEdit>
#include <QDateTimeEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QSlider>
#include <QPlainTextEdit>
#include <QRadioButton>
#include <QGroupBox>
#include <QListWidget>
#include <QTreeWidget>
#include <QFile>
#include <QTreeWidgetItemIterator>
#include <QSet>
#include <QLabel>
#include <QPixmap>
#include <QTableWidget>

#include "FileChooser.hpp"
#include "PictureChooser.hpp"
#include "FormWidget.hpp"

DataHandler::DataHandler( DataLoader *_dataLoader, FormWidget *_formWidget, bool _debug )
	: m_dataLoader( _dataLoader ), m_formWidget( _formWidget ), m_debug( _debug )
{
}

void DataHandler::writeFormData( QString form_name, QWidget *form, QByteArray *data, QHash<QString, QString> *props )
{
	QSet<QWidget *> seen;
	QXmlStreamWriter xml( data );

	// pretty-printing only in debug mode (because of superfluous
	// white spaces sent to server)
	if( m_debug ) {
		xml.setAutoFormatting( true );
		xml.setAutoFormattingIndent( 2 );
	}
	
	xml.writeStartDocument( );
	if( props->contains( "rootelement" ) && props->contains( "doctype" ) ) {
		xml.writeDTD( QString( "<!DOCTYPE %1 SYSTEM '%2'>" )
			.arg( props->value( "rootelement" ) )
			.arg( props->value( "doctype" ) ) );
	}
	if( props->contains( "rootelement" ) ) {
		xml.writeStartElement( props->value( "rootelement" ) );
		foreach( QString key, props->keys( ) ) {
	// skip _q_ dynamic properties, they are used by the Qt stylesheet engine
			if( key.startsWith( "_q_" ) ) continue;
	// skip globals
			if( key.startsWith( "global." ) ) continue;
	// ignore our own actions
			if( key == "doctype" || key == "rootelement" || key == "action" || key == "initAction" ) continue;
			xml.writeAttribute( key, props->value( key ) );
		}		
	} else {
		xml.writeStartElement( form_name );
	}
	
	writeWidgets( form, xml, props, &seen );

	xml.writeEndElement( );
	xml.writeEndDocument( );
}

void DataHandler::writeWidgets( QWidget *_from, QXmlStreamWriter &xml, QHash<QString, QString> *props, QSet<QWidget *> *seen )
{
	QList<QWidget *> widgets = _from->findChildren<QWidget *>( );
	foreach( QWidget *widget, widgets ) {
		QString clazz = widget->metaObject( )->className( ); 
		QString name = widget->objectName( );

// already dumped		
		if( seen->contains( widget ) ) continue;
		seen->insert( widget );
		
// ignore internal elements
		if( name == "" || name.startsWith( "qt_" ) ||
			clazz == "QLabel" ||
			!widget->isEnabled( ) ) {
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
		} else if( clazz == "QTimeEdit" ) {
			QTimeEdit *timeEdit = qobject_cast<QTimeEdit *>( widget );
			QString text = timeEdit->time( ).toString( Qt::ISODate );
			xml.writeTextElement( "", name, text );
		} else if( clazz == "QDateTimeEdit" ) {
			QDateTimeEdit *dateTimeEdit = qobject_cast<QDateTimeEdit *>( widget );
			QString text = dateTimeEdit->dateTime( ).toString( Qt::ISODate );
			xml.writeTextElement( "", name, text );
		} else if( clazz == "QComboBox" ) {
			QComboBox *comboBox = qobject_cast<QComboBox *>( widget );
			QString text = comboBox->itemText( comboBox->currentIndex( ) );
			xml.writeTextElement( "", name, text );
		} else if( clazz == "QSpinBox" ) {
			QSpinBox *spinBox = qobject_cast<QSpinBox *>( widget );
			QString text = QString::number( spinBox->value( ) );
			xml.writeTextElement( "", name, text );
		} else if( clazz == "QDoubleSpinBox" ) {
			QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox *>( widget );
			QString text = QString::number( spinBox->value( ) );
			xml.writeTextElement( "", name, text );
		} else if( clazz == "QSlider" ) {
			QSlider *slider = qobject_cast<QSlider *>( widget );
			QString text = QString::number( slider->value( ) );
			xml.writeTextElement( "", name, text );
		} else if( clazz == "QPlainTextEdit" ) {
			QPlainTextEdit *plainTextEdit = qobject_cast<QPlainTextEdit *>( widget );
			QString text = plainTextEdit->toPlainText( );
			xml.writeTextElement( "", name, text );
		} else if( clazz == "QTextEdit" ) {
			QTextEdit *textEdit = qobject_cast<QTextEdit *>( widget );
			QString html = textEdit->toHtml( );
			xml.writeTextElement( "", name, html );
		} else if( clazz == "QCheckBox" ) {
			QCheckBox *checkBox = qobject_cast<QCheckBox *>( widget );
			QObject *_parent = widget->parent( );
			QString clazzParent = _parent->metaObject( )->className( ); 
			if( clazzParent == "QGroupBox" ) {
				QString groupName = _parent->objectName( );
				if( checkBox->isChecked( ) ) {
					xml.writeTextElement( "", groupName, name );
				}
			} else {
				xml.writeTextElement( "", name, checkBox->isChecked( ) ? "true" : "false" );
			}
		} else if( clazz == "QRadioButton" ) {
			QRadioButton *radioButton = qobject_cast<QRadioButton *>( widget );
			QObject *_parent = widget->parent( );
			QString clazzParent = _parent->metaObject( )->className( ); 
			if( clazzParent == "QGroupBox" ) {
				QString groupName = _parent->objectName( );
				if( radioButton->isChecked( ) ) {
					xml.writeTextElement( "", groupName, name );
				}
			} else {
				xml.writeTextElement( "", name, radioButton->isChecked( ) ? "true" : "false" );
			}
		} else if( clazz == "QListWidget" ) {
			QListWidget *listWidget = qobject_cast<QListWidget *>( widget );
			QList<QListWidgetItem *> items = listWidget->selectedItems( );
			foreach( QListWidgetItem *item, items ) {
				xml.writeTextElement( "", name, item->data( Qt::DisplayRole ).toString( ) );
			}
		} else if( clazz == "QTableWidget" ) {
			QTableWidget *tableWidget = qobject_cast<QTableWidget *>( widget );
			QList<QTableWidgetItem *> items = tableWidget->selectedItems( );
			QSet<QString> seen;
			foreach( QTableWidgetItem *item, items ) {
				// hard-coded! should be key/value as user attributes!
				QString id = item->data( Qt::UserRole ).toString( );
				if( !id.isNull( ) ) {
					// HACK: only first row, otherwise we get duplicates
					if( !seen.contains( id ) ) {
						xml.writeStartElement( "", name );
						xml.writeAttribute( "id", id );
						xml.writeCharacters( item->data( Qt::UserRole ).toString( ) );
						xml.writeEndElement( );
						seen.insert( id );
					}
				}
			}
		} else if( clazz == "QTreeWidget" ) {
			QTreeWidget *treeWidget = qobject_cast<QTreeWidget *>( widget );
			QList<QTreeWidgetItem *> items = treeWidget->selectedItems( );
			foreach( QTreeWidgetItem *item, items ) {
				// hard-coded! should be key/value as user attributes!
				xml.writeStartElement( "", name );
				QString id = item->data( 0, Qt::UserRole ).toString( );
				if( !id.isNull( ) ) {
					xml.writeAttribute( "id", id );
				}
				xml.writeCharacters( item->data( 0, Qt::DisplayRole ).toString( ) );
				xml.writeEndElement( );
			}
		} else if( clazz == "QGroupBox" ) {
			QGroupBox *groupBox = qobject_cast<QGroupBox *>( widget );
			QString boxName = groupBox->objectName( );
			xml.writeStartElement( boxName );

			// this is actually for all elements, get hidden properties and attach
			// them again as XML attributes of the current element
			QHash<QString, QString> p;
			FormWidget::readDynamicStringProperties( &p, widget );
			foreach( QString key, p.keys( ) ) {
				// skip Qt internal ones
				if( key.startsWith( "_q_" ) ) continue;
				// skip globals
				if( key.startsWith( "global." ) ) continue;
				xml.writeAttribute( key, p.value( key ) );
			}
			writeWidgets( groupBox, xml, props, seen );
			xml.writeEndElement( );
		} else if( clazz == "FileChooser" ) {
			FileChooser *fileChooser = qobject_cast<FileChooser *>( widget );
			QStringList fileNames = fileChooser->fileNames( );
			xml.writeStartElement( name );
			foreach( QString fileName, fileNames ) {
				if( fileName.isEmpty( ) ) continue;			
				xml.writeStartElement( "file" );
				xml.writeAttribute( "filename", fileName );
				QFile file( fileName );
				file.open( QFile::ReadOnly );
				QByteArray fileContent = file.readAll( );
				xml.writeAttribute( "size", QString::number( fileContent.length( ) ) );
				QString encoded = QString( fileContent.toBase64( ) );
				file.close( );	
				xml.writeCharacters( encoded );
				xml.writeEndElement( );
			}
			xml.writeEndElement( );
		} else if( clazz == "PictureChooser" ) {
			PictureChooser *pictureChooser = qobject_cast<PictureChooser *>( widget );
			QString fileName = pictureChooser->fileName( );
			xml.writeStartElement( name );
			xml.writeAttribute( "filename", fileName );
			QByteArray fileContent = pictureChooser->picture( );
			xml.writeAttribute( "size", QString::number( fileContent.length( ) ) );
			QString encoded = QString( fileContent.toBase64( ) );
			xml.writeCharacters( encoded );
			xml.writeEndElement( );
		} else if( clazz == "QPushButton" ) {
			// skip, ok, buttons can't be reset
		} else if( clazz == "QWidget" ) {
			// skip, generic widget, don't possibly know how to reset it
		} else {
			qWarning( ) << "Write for unknown class" << clazz << "of widget" << widget << "(" << name << ")";
		}
		
		qDebug( ) << "Wrote " << clazz << name;
	}
}

void DataHandler::resetFormData( QWidget *form )
{
	QList<QWidget *> widgets = form->findChildren<QWidget *>( );
	foreach( QWidget *widget, widgets ) {
		QString clazz = widget->metaObject( )->className( ); 
		QString name = widget->objectName( );
// ignore internal elements
		if( name == "" || name.startsWith( "qt_" ) ||
			clazz == "QLabel" ||
			!widget->isEnabled( ) ) {
			continue;
		}

// get dynamic properties of the widget (used for 'initialFocus' and 'state' currently)
		QHash<QString, QString> *props = new QHash<QString, QString>( );
		FormWidget::readDynamicStringProperties( props, widget );
		m_formWidget->restoreFromGlobals( props );
		
		if( clazz == "QLineEdit" ) {
			QLineEdit *lineEdit = qobject_cast<QLineEdit *>( widget );
			lineEdit->clear( );
			if( props->contains( "state" ) ) {
				lineEdit->setText( props->value( "state" ) );
			}
		} else if( clazz == "QDateEdit" ) {
			//~ QDateEdit *dateEdit = qobject_cast<QDateEdit *>( widget );
			// TODO
		} else if( clazz == "QTimeEdit" ) {
			//~ QTimeEdit *timeEdit = qobject_cast<QTimeEdit *>( widget );
			// TODO
		} else if( clazz == "QDateTimeEdit" ) {
			//~ QDateTimeEdit *dateTimeEdit = qobject_cast<QDateTimeEdit *>( widget );
		} else if( clazz == "QComboBox" ) {
			QComboBox *comboBox = qobject_cast<QComboBox *>( widget );
			comboBox->clear( );
		} else if( clazz == "QSpinBox" ) {
			//~ QSpinBox *spinBox = qobject_cast<QSpinBox *>( widget );
			// TODO
		} else if( clazz == "QDoubleSpinBox" ) {
			//~ QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox *>( widget );
			// TODO
		} else if( clazz == "QSlider" ) {
			//~ QSlider *slider = qobject_cast<QSlider *>( widget );
			// TODO
		} else if( clazz == "QPlainTextEdit" ) {
			QPlainTextEdit *plainTextEdit = qobject_cast<QPlainTextEdit *>( widget );
			plainTextEdit->clear( );
		} else if( clazz == "QTextEdit" ) {
			QTextEdit *textEdit = qobject_cast<QTextEdit *>( widget );
			textEdit->clear( );
		} else if( clazz == "QCheckBox" ) {
			QCheckBox *checkBox = qobject_cast<QCheckBox *>( widget );
			checkBox->setChecked( false );
		} else if( clazz == "QRadioButton" ) {
			QRadioButton *radioButton = qobject_cast<QRadioButton *>( widget );
			radioButton->setChecked( false );
		} else if( clazz == "QListWidget" ) {
			QListWidget *listWidget = qobject_cast<QListWidget *>( widget );
//			listWidget->clear( );
			for( int i = 0; i < listWidget->count( ); i++ ) {
				QListWidgetItem *item = listWidget->item( i );
				item->setSelected( false );
			}
		} else if( clazz == "QTreeWidget" ) {
			QTreeWidget *treeWidget = qobject_cast<QTreeWidget *>( widget );
//			treeWidget->clear( );
			QTreeWidgetItemIterator it( treeWidget );
			while( *it ) {
				(*it)->setSelected( false );
				(*it)->setExpanded( false );
				it++;
			}
		} else if( clazz == "QTableWidget" ) {
			QTableWidget *tableWidget = qobject_cast<QTableWidget *>( widget );
//			tableWidget->clearContents( );			
			for( int row = 0; row < tableWidget->rowCount( ); row++ ) {
				for( int col = 0; col < tableWidget->columnCount( ); col++ ) {
					QTableWidgetItem *item = tableWidget->item( row, col );
					if( item ) item->setSelected( false );
				}
			}
		} else if( clazz == "FileChooser" ) {
			FileChooser *fileChooser = qobject_cast<FileChooser *>( widget );
			fileChooser->setFileName( "" );
		} else if( clazz == "PictureChooser" ) {
			PictureChooser *pictureChooser = qobject_cast<PictureChooser *>( widget );
			pictureChooser->setFileName( "" );
		} else if( clazz == "QPushButton" ) {
			// skip, ok, buttons can't be reset
		} else if( clazz == "QGroupBox" ) {
			// skip, ok, grouboxes can't be reset
		} else if( clazz == "QWidget" ) {
			// skip, generic widget, don't possibly know how to reset it
		} else {
			qWarning( ) << "Reset for unknown class" << clazz << "of widget" << widget << "(" << name << ")";
		}
		
		if( 	props->contains( "initialFocus" ) &&
			props->value( "initialFocus" ) == "true" ) {
			qDebug( ) << "Setting focus of widget" << name;
			widget->setFocus( );
		}
		
		qDebug( ) << "Reset " << clazz << name;
	}
}

void DataHandler::loadFormDomains( QString form_name, QWidget *form )
{
	QList<QWidget *> widgets = form->findChildren<QWidget *>( );
	foreach( QWidget *widget, widgets ) {
		QString clazz = widget->metaObject( )->className( ); 
		QString name = widget->objectName( );
// ignore internal elements
		if( name == "" || name.startsWith( "qt_" ) ||
			clazz == "QLabel" ||
			!widget->isEnabled( ) ) {
			continue;
		}
		
		// TODO: widgets can also have custom properties for the domain handling
		QHash<QString, QString> *props = new QHash<QString, QString>( );
		FormWidget::readDynamicStringProperties( props, widget );
		m_formWidget->restoreFromGlobals( props );
		props->insert( "action", "read" );
		if( clazz == "QComboBox" ) {
			m_dataLoader->request( form_name, name, QByteArray( ), props );
		} else if( clazz == "QListWidget" ) {
			m_dataLoader->request( form_name, name, QByteArray( ), props );
		} else if( clazz == "QTreeWidget" ) {
			m_dataLoader->request( form_name, name, QByteArray( ), props );
		} else if( clazz == "QTableWidget" ) {
			m_dataLoader->request( form_name, name, QByteArray( ), props );
		} else {
			// all other classes don't load domains, but we want to keep
			// the calling code generic..
		}
		
		qDebug( ) << "Domain load in " << clazz << name;
	}
}

void DataHandler::loadFormDomain( QString form_name, QString widget_name, QWidget *form, QByteArray &data, QHash<QString, QString> *props )
{
	QWidget *widget = form->findChild<QWidget *>( widget_name );
	QString clazz = widget->metaObject( )->className( ); 

	qDebug( ) << "Loading domain data for load in " << form_name << widget_name << data.length( );

	QXmlStreamReader xml( data );
	if( clazz == "QComboBox" ) {
		QComboBox *comboBox = qobject_cast<QComboBox *>( widget );
		while( !xml.atEnd( ) ) {
			xml.readNext( );
							
			if( xml.isStartElement( ) && xml.name( ) == "value" ) {
				QString text = xml.readElementText( QXmlStreamReader::ErrorOnUnexpectedElement );
				comboBox->addItem( text );
			}
		}
	} else if( clazz == "QListWidget" ) {
		QListWidget *listWidget = qobject_cast<QListWidget *>( widget );
		while( !xml.atEnd( ) ) {
			xml.readNext( );
			if( xml.isStartElement( ) && xml.name( ) == "value" ) {
				QString text = xml.readElementText( QXmlStreamReader::ErrorOnUnexpectedElement );
				listWidget->addItem( text );
			}
		}
	} else if( clazz == "QTableWidget" ) {
		QTableWidget *tableWidget = qobject_cast<QTableWidget *>( widget );
		QStringList headers;
		for( int i = 0; i < tableWidget->columnCount( ); i++ ) {
			QTableWidgetItem *item = tableWidget->horizontalHeaderItem( i );
			QString headerText = item->data( Qt::DisplayRole ).toString( );
			headers << headerText;		
		}
		int row = 0;
		bool inData = false;
		QXmlStreamAttributes attributes;
		while( !xml.atEnd( ) ) {
			xml.readNext( );
			if( xml.isStartElement( ) ) {
				if( xml.name( ) == widget_name ) {
					inData = true;
					tableWidget->insertRow( row );
// HACK: set height of rows to thumbnail heigth					
					tableWidget->setRowHeight( row, 50 );
// HACK: map attributes of rows into cells, not happy with that one!					
					attributes = xml.attributes( );
				} else if( inData ) {
					int col = headers.indexOf( xml.name( ).toString( ) );
					if( col != -1 ) {
						QString text = xml.readElementText( QXmlStreamReader::ErrorOnUnexpectedElement );
// HACK: hard-coded, don't have types in the XML (like 'binary, base64 encoded')
						if( xml.name( ) == "thumbnail" ) {
							QByteArray encoded = text.toAscii( );
							QByteArray decoded = QByteArray::fromBase64( encoded );

							QPixmap pixmap;
							pixmap.loadFromData( decoded );

							QLabel *label = new QLabel( );
							label->setPixmap( pixmap );
// HACK: assume the thumbnail is small enough to fit
							label->setFixedSize( pixmap.size( ) );

							QTableWidgetItem *item = new QTableWidgetItem( QString( ) );
							item->setFlags( item->flags( ) ^ Qt::ItemIsEditable );
							tableWidget->setItem( row, col, item );
							
							tableWidget->setCellWidget( row, col, label );
						} else {
							QTableWidgetItem *item = new QTableWidgetItem( text );
							item->setFlags( item->flags( ) ^ Qt::ItemIsEditable );
							tableWidget->setItem( row, col, item );
						}
// attributes like id are mapped to cell user data
						foreach( QXmlStreamAttribute attr, attributes ) {
							QVariant v;
							v.setValue( attr.value( ).toString( ) );
							QTableWidgetItem *item = tableWidget->item( row, col );
							item->setData( Qt::UserRole, v );
						}
					}
				}
			} else if( xml.isEndElement( ) ) {
				if( xml.name( ) == widget_name ) {
					row++;
					inData = false;
				}
			}
		}

// HACK: make sure the thumbs are visible
		tableWidget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
		tableWidget->resizeColumnsToContents( );

// iterate again and check against saved table state
		if( props->contains( "state" ) ) {
			qDebug( ) << "Restoring table state for tree" << widget_name << props->value( "state" );
			QStringList stateList = props->value( "state" ).split( "|", QString::SkipEmptyParts );
			QSet<QString> states;
			foreach( QString state, stateList ) {
				if( state.left( 1 ) == "S" ) {
					states.insert( state.mid( 1, state.length( ) - 1 ) );
				}
				for( int row = 0; row < tableWidget->rowCount( ); row++ ) {
					QTableWidgetItem *item = tableWidget->item( row, 0 );
					if( !item ) continue;
					QString id = item->data( Qt::UserRole ).toString( );
					if( states.contains( id ) ) {
						tableWidget->selectRow( row );
						// scrolls automatically to selected row
					}
				}
			}
		}
	} else if( clazz == "QTreeWidget" ) {
		QTreeWidget *treeWidget = qobject_cast<QTreeWidget *>( widget );
		QTreeWidgetItem *header = treeWidget->headerItem( );
		QStringList headers;
		for( int i = 0; i < header->columnCount( ); i++ ) {
			QString headerText = header->data( i, Qt::DisplayRole ).toString( );
			headers << headerText;
		}
		QTreeWidgetItem *_parent = treeWidget->invisibleRootItem( );
		QTreeWidgetItem *item = treeWidget->invisibleRootItem( );
		bool first = true;
		while( !xml.atEnd( ) ) {
			xml.readNext( );
			if( xml.isStartElement( ) ) {
				if( xml.name( ) == "tree" ) {
					_parent = item;
				} else if( xml.name( ) == "item" ) {
					if( first ) {
						first = false;
						continue;
					}
					item = new QTreeWidgetItem( _parent );
// attributes like id are mapped to user data
					QXmlStreamAttributes attributes = xml.attributes( );
					foreach( QXmlStreamAttribute attr, attributes ) {
						QVariant v;
						v.setValue( attr.value( ).toString( ) );
						item->setData( 0, Qt::UserRole, v );
					}
				} else {
// all element in the item are mapped to the columns
					int col = headers.indexOf( xml.name( ).toString( ) );
					if( col != -1 ) {
						QString text = xml.readElementText( QXmlStreamReader::ErrorOnUnexpectedElement );
						item->setText( col, text );
					}
				}
			} else if( xml.isEndElement( ) ) {
				if( xml.name( ) == "tree" ) {
					item = _parent;
					if( _parent ) {
						_parent = _parent->parent( );
						if( !_parent ) {
							_parent = treeWidget->invisibleRootItem( );
						}
					}
				} else if( xml.name( ) == "item" ) {
					_parent->addChild( item );
				}
			}
		}

// iterate again and check against saved tree state
		if( props->contains( "state" ) ) {
			qDebug( ) << "Restoring tree state for tree" << widget_name;
			QStringList stateList = props->value( "state" ).split( "|", QString::SkipEmptyParts );
// expand tree first, otherwise parents get selected if we select a non-expanded subtree!
			{
				QSet<QString> states;
				foreach( QString state, stateList ) {
					if( state.left( 1 ) == "E" ) {
						states.insert( state.mid( 1, state.length( ) - 1 ) );
					}
				}
				QTreeWidgetItemIterator it( treeWidget );
				while( *it ) {
					QString id = (*it)->data( 0, Qt::UserRole ).toString( );
					if( states.contains( id ) ) {
						(*it)->setExpanded( true );
					}
					++it;
				}
			}
// twice, see above
			{
				QSet<QString> states;
				foreach( QString state, stateList ) {
					if( state.left( 1 ) == "S" ) {
						states.insert( state.mid( 1, state.length( ) - 1 ) );
					}
				}
				QTreeWidgetItemIterator it( treeWidget );
				while( *it ) {
					QString id = (*it)->data( 0, Qt::UserRole ).toString( );
					if( states.contains( id ) ) {
						(*it)->setSelected( true );
						// better than nothing, scroll to the position of the last selection (usually one)
						treeWidget->scrollToItem( *it );
					}
					++it;
				}
			}
		}
	} else {
		// all other classes don't have domain data
	}
	if( xml.hasError( ) ) {
		qCritical( ) << xml.errorString( );
	}
}

void DataHandler::readFormData( QString formName, QWidget *form, QByteArray &data, QHash<QString, QString> *props )
{
	QXmlStreamReader xml( data );
	QWidget *widget = 0;
	bool inForm = true /* false */;
	QString clazz;

// search for root element (new) or form name (old)
	QString name;
	if( props->contains( "rootelement" ) ) {
		name = props->value( "rootelement" );
	} else {
		name = formName;
	}
	
	resetFormData( form );
	//loadFormDomains( formName, form );
	
	while( !xml.atEnd( ) ) {
		xml.readNext( );
		if( xml.isStartElement( ) && xml.name( ) == name ) {
			inForm = true;
		} else if( xml.isEndElement( ) && xml.name( ) == name ) {
// HACK: in theory we should check for subforms, currently we can't prepare the
// data in the layers below as we want, so we do an everywhere match (global namespace
// for all widgets), which is quite dangerous!
			//inForm = false;
		} else {
			if( inForm ) {
				if( xml.isStartElement( ) ) {
					widget = qFindChild<QWidget *>( form, xml.name( ).toString( ) );
					qDebug( ) << "Reading from XML for" << xml.name( ) << "into" << widget;
					if( widget ) {
						clazz = widget->metaObject( )->className( ); 						
						QXmlStreamAttributes attributes = xml.attributes( );
						if( clazz == "QLineEdit" ) {
							QLineEdit *lineEdit = qobject_cast<QLineEdit *>( widget );
							QString text = xml.readElementText( QXmlStreamReader::ErrorOnUnexpectedElement );
							lineEdit->setText( text );
						} else if( clazz == "QDateEdit" ) {
							QDateEdit *dateEdit = qobject_cast<QDateEdit *>( widget );
							QString text = xml.readElementText( QXmlStreamReader::ErrorOnUnexpectedElement );
							dateEdit->setDate( QDate::fromString( text, Qt::ISODate ) );
						} else if( clazz == "QTimeEdit" ) {
							QTimeEdit *timeEdit = qobject_cast<QTimeEdit *>( widget );
							QString text = xml.readElementText( QXmlStreamReader::ErrorOnUnexpectedElement );
							timeEdit->setTime( QTime::fromString( text, Qt::ISODate ) );
						} else if( clazz == "QDateTimeEdit" ) {
							QDateTimeEdit *dateTimeEdit = qobject_cast<QDateTimeEdit *>( widget );
							QString text = xml.readElementText( QXmlStreamReader::ErrorOnUnexpectedElement );
							dateTimeEdit->setDateTime( QDateTime::fromString( text, Qt::ISODate ) );
						} else if( clazz == "QSpinBox" ) {
							QSpinBox *spinBox = qobject_cast<QSpinBox *>( widget );
							QString text = xml.readElementText( QXmlStreamReader::ErrorOnUnexpectedElement );
							spinBox->setValue( text.toInt( ) );
						} else if( clazz == "QDoubleSpinBox" ) {
							QDoubleSpinBox *doubleSpinBox = qobject_cast<QDoubleSpinBox *>( widget );
							QString text = xml.readElementText( QXmlStreamReader::ErrorOnUnexpectedElement );
							doubleSpinBox->setValue( text.toDouble( ) );
						} else if( clazz == "QComboBox" ) {
							QComboBox *comboBox = qobject_cast<QComboBox *>( widget );
							QString text = xml.readElementText( QXmlStreamReader::ErrorOnUnexpectedElement );
							for( int idx = 0; idx < comboBox->count( ); idx++ ) {
								if( comboBox->itemText( idx ) == text ) {
									comboBox->setCurrentIndex( idx );
									break;
								}
							}
						} else if( clazz == "QCheckBox" ) {
							QObject *parent = widget->parent( );
							QString clazzParent = parent->metaObject( )->className( ); 
							if( clazzParent != "QGroupBox" ) {
								QCheckBox *checkBox = qobject_cast<QCheckBox *>( widget );
								QString text = xml.readElementText( QXmlStreamReader::ErrorOnUnexpectedElement );
								if( text == "true"  ) {
									checkBox->setChecked( true );
								} else {
									checkBox->setChecked( false );
								}
							}
						} else if( clazz == "QRadioButton" ) {
							QObject *parent = widget->parent( );
							QString clazzParent = parent->metaObject( )->className( ); 
							if( clazzParent != "QGroupBox" ) {
								QRadioButton *radioButton = qobject_cast<QRadioButton *>( widget );
								QString text = xml.readElementText( QXmlStreamReader::ErrorOnUnexpectedElement );
								if( text == "true"  ) {
									radioButton->setChecked( true );
								} else {
									radioButton->setChecked( false );
								}
							}
						} else if( clazz == "QSlider" ) {
							QSlider *slider = qobject_cast<QSlider *>( widget );
							QString text = xml.readElementText( QXmlStreamReader::ErrorOnUnexpectedElement );
							slider->setValue( text.toInt( ) );
						} else if( clazz == "QPlainTextEdit" ) {
							QPlainTextEdit *plainTextEdit = qobject_cast<QPlainTextEdit *>( widget );
							QString text = xml.readElementText( QXmlStreamReader::ErrorOnUnexpectedElement );
							plainTextEdit->setPlainText( text );
						} else if( clazz == "QTextEdit" ) {
							QTextEdit *textEdit = qobject_cast<QTextEdit *>( widget );
							QString text = xml.readElementText( QXmlStreamReader::ErrorOnUnexpectedElement );
							textEdit->setHtml( text );
						} else if( clazz == "QGroupBox" ) {
							QList<QWidget *> children = widget->findChildren<QWidget *>( );
							foreach( QWidget *child, children ) {
								QString subClazz = child->metaObject( )->className( ); 
								QString subName = child->objectName( );
								if( subClazz == "QRadioButton" ) {
									QRadioButton *radioButton = qobject_cast<QRadioButton *>( child );
									QString subText = radioButton->text( );
									QString text = xml.readElementText( QXmlStreamReader::ErrorOnUnexpectedElement );
									qDebug( ) << "radio" << name << subText << subText << name << subName;
									radioButton->setChecked( text.compare( subName ) == 0 );
								} else if( subClazz == "QCheckBox" ) {
									QCheckBox *checkBox = qobject_cast<QCheckBox *>( child );
									QString subText = checkBox->text( );
									QString text = xml.readElementText( QXmlStreamReader::ErrorOnUnexpectedElement );
									qDebug( ) << "checkbox" << name << subText << subText << name << subName;
									if( text.compare( subName ) == 0 ) {
										checkBox->setChecked( true );
									}
								} else {
									// don't read text element, otherwise the parser tells us
									// "Expected character data.", a little bit annoying..
								}
							}								
							// handle attributes (e.g. picture 'id') here, should be
							// handled for all widget actually. Remember value in
							// a dynamic property in the widget
							QXmlStreamAttributes attributes = xml.attributes( );
							foreach( QXmlStreamAttribute attr, attributes ) {
								FormWidget::writeDynamicStringProperty( widget, attr.name( ).toString( ).toLatin1( ).data( ), attr.value( ).toString( ) );
							}
						} else if( clazz == "QListWidget" ) {
							QString text = xml.readElementText( QXmlStreamReader::ErrorOnUnexpectedElement );
							QListWidget *listWidget = qobject_cast<QListWidget *>( widget );
							QList<QListWidgetItem *> items = listWidget->findItems( text, Qt::MatchExactly );
							foreach( QListWidgetItem *item, items ) {
								item->setSelected( true );
							}
						} else if( clazz == "QTreeWidget" ) {
							QString text = xml.readElementText( QXmlStreamReader::ErrorOnUnexpectedElement );
							QTreeWidget *treeWidget = qobject_cast<QTreeWidget *>( widget );
							// TODO: select by name (text in first field), for backwards compatibility, should be done by id
							QList<QTreeWidgetItem *> items = treeWidget->findItems( text, Qt::MatchExactly | Qt::MatchRecursive | Qt::MatchWrap, 0 );
							foreach( QTreeWidgetItem *item, items ) {
								item->setSelected( true );
								QTreeWidgetItem *parent = item->parent( );
								while( parent != 0 && parent != treeWidget->invisibleRootItem( ) ) {
									parent->setExpanded( true );
									parent = parent->parent( );
								}
							}
						} else if( clazz == "QTableWidget" ) {
							// TODO: select by user data, hard-coded for now, assuming id in userdata
							QTableWidget *tableWidget = qobject_cast<QTableWidget *>( widget );
							QString id = attributes.value( "", "id" ).toString( );
							if( !id.isEmpty( ) ) {
								QAbstractItemModel *model = tableWidget->model( );
								QModelIndexList matches = model->match( model->index( 0, 0 ), Qt::UserRole, id );
								foreach( const QModelIndex &index, matches ) {
									tableWidget->selectRow( index.row( ) );
								}
							}
						} else if( clazz == "FileChooser" ) {
							// don't restore anything, this is an upload component only
						} else if( clazz == "PictureChooser" ) {
							PictureChooser *pictureChooser = qobject_cast<PictureChooser *>( widget );
							QString text = xml.readElementText( QXmlStreamReader::ErrorOnUnexpectedElement );
							QByteArray encoded = text.toAscii( );
							QByteArray decoded = QByteArray::fromBase64( encoded );
							pictureChooser->setPicture( decoded );
						} else if( clazz == "QPushButton" ) {
							// skip, ok, buttons can't be reset
						} else if( clazz == "QWidget" ) {
							// skip, generic widget, don't possibly know how to reset it
						} else if( clazz == "QLabel" ) {
							// labels can usually not be edited, at the moment we use them to show
							// images and text
							QLabel *label = qobject_cast<QLabel *>( widget );
							QString text = xml.readElementText( QXmlStreamReader::ErrorOnUnexpectedElement );
							if( xml.name( ) == "image" || xml.name( ) == "thumbnail" ) {
// HACK: no types -> detect pictures by name							
								QByteArray encoded = text.toAscii( );
								QByteArray decoded = QByteArray::fromBase64( encoded );
								QPixmap p;
								p.loadFromData( decoded );	
								if( !p.isNull( ) ) {							
									int w = std::min( label->width( ), p.width( ) );
									int h = std::min( label->height( ), p.height( ) );							
									label->setPixmap( p.scaled( QSize( w, h ), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
									label->adjustSize( );
								}
							} else {
								label->setText( text );
							}
						} else {
							qWarning( ) << "Read for unknown class" << clazz << "of widget" << widget << "(" << name << ")";
						}
					}
				}
			}
		}
	}
	if( xml.hasError( ) ) {
		qCritical( ) << xml.errorString( );
	}
}

// Note: this is over-simplistic and hard-coded. Should be a generic
// property introspection language similar to the variable system
// in QScript (Javascript), maybe later..
QString DataHandler::readFormVariable( QString variable, QWidget *form )
{
	QStringList parts = variable.split( "." );

	qDebug( ) << "Evaluating variable" << variable;
	
// expecting a widget name as first argument
	if( parts[0].isNull( ) ) {
		qWarning( ) << "Expecting a expression of the form <widget>.<property";
		return QString( );
	}
	QString name = parts[0];
	
// expecting a property identifier as second argument
	if( parts[1].isNull( ) ) {
		qWarning( ) << "Expecting a property name in variable" << variable;
		return QString( );
	}
	QString property = parts[1];
	
	QWidget *widget = qFindChild<QWidget *>( form, name );
// no widget found with that name
	if( !widget ) {
		qWarning( ) << "Unknown widget" << name << "in variable" << variable << "of form" << form;
		return QString( );
	}
	
// properties differ depending on the class of the widget	
	QString clazz = widget->metaObject( )->className( );

	if( clazz == "QLineEdit" ) {
		QLineEdit *lineEdit = qobject_cast<QLineEdit *>( widget );
		if( property == "text" ) {
			return lineEdit->text( );
		} else if( property == "state" ) {
			return lineEdit->text( );
		} else {
			qWarning( ) << "Unsupported property" << property << "for class" << clazz << "in variable" << variable;
		}
	} else if( clazz == "QTableWidget" ) {
		QTableWidget *tableWidget = qobject_cast<QTableWidget *>( widget );
// always return data of the selected row (assuming single select for now and
// row select only)
// the ID is currently hard-coded in user data
		if( property == "id" ) {
			QList<QTableWidgetItem *> items = tableWidget->selectedItems( );
			if( items.empty( ) ) {
// nothing is selected, return the id of the root element, which is always 1
				return QString( "1" );
			}
			foreach( QTableWidgetItem *item, items ) {
// HACK: return /picture@id, first user element, besser mapping can be found later
				return item->data( Qt::UserRole ).toString( );
			}
		} else if( property == "state" ) {
			QList<QTableWidgetItem *> items = tableWidget->selectedItems( );
			QString state = "";
			QSet<QString> seen;
			foreach( QTableWidgetItem *item, items ) {
				// hard-coded! should be key/value as user attributes!
				QString id = item->data( Qt::UserRole ).toString( );
				if( !id.isNull( ) ) {
					// HACK: only first row, otherwise we get duplicates
					// TODO: currently we only have row selection, later
					// remember all coordinates of the items
					if( !seen.contains( id ) ) {
						state.append( "S" );
						state.append( item->data( Qt::UserRole ).toString( ) );
						state.append( "|" );							
						seen.insert( id );
					}
				}
			}

			return state;
		} else {
			qWarning( ) << "Unsupported property" << property << "for class" << clazz << "in variable" << variable;
		}
	} else if( clazz == "QTreeWidget" ) {
		QTreeWidget *treeWidget = qobject_cast<QTreeWidget *>( widget );
		
// always return data of the selected item (assuming single select for now)
// the ID is currently hard-coded in user data
		if( property == "id" ) {
			QList<QTreeWidgetItem *> items = treeWidget->selectedItems( );
			if( items.empty( ) ) {
// nothing is selected, return the id of the root element, which is always 1
				return QString( "1" );
			}			
			return items[0]->data( 0, Qt::UserRole ).toString( );
// state is header sizes, expanded/collapse states, selection states, sorting order,
// scroll position, etc.
// all those get lost if we reload the form..
		} else if( property == "state" ) {
			QTreeWidgetItemIterator it( treeWidget );
			QString state = "";
			while( *it ) {
				if( (*it)->isExpanded( ) ) {
					state.append( "E" );
					state.append( (*it)->data( 0, Qt::UserRole ).toString( ) );
					state.append( "|" );
				}
				if( (*it)->isSelected( ) ) {
					state.append( "S" );
					state.append( (*it)->data( 0, Qt::UserRole ).toString( ) );
					state.append( "|" );
				}
				++it;
			}

			return state;
		} else {
			qWarning( ) << "Unsupported property" << property << "for class" << clazz << "in variable" << variable;
		}
	} else {
// non supported class
		qWarning( ) << "Unsupported class" << clazz << "in variable" << variable;
		return QString( );
	}
	
	return QString( );
}

