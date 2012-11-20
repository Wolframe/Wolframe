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

#include "FileChooser.hpp"
#include "PictureChooser.hpp"
#include "FormWidget.hpp"

DataHandler::DataHandler( DataLoader *_dataLoader ) : m_dataLoader( _dataLoader )
{
}

void DataHandler::writeFormData( QString form_name, QWidget *form, QByteArray *data, QHash<QString, QString> *props )
{
	QSet<QWidget *> seen;
	QXmlStreamWriter xml( data );
	xml.setAutoFormatting( true );
	xml.setAutoFormattingIndent( 2 );
	
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
		} else if( clazz == "QTreeWidget" ) {
			QTreeWidget *treeWidget = qobject_cast<QTreeWidget *>( widget );
			QList<QTreeWidgetItem *> items = treeWidget->selectedItems( );
			foreach( QTreeWidgetItem *item, items ) {
				// hard-coded! should be key/value as user attributes!
				QString id = item->data( 0, Qt::UserRole ).toString( );
				if( !id.isNull( ) ) {
					xml.writeAttribute( "id", id );
				}
//				xml.writeAttribute( key, props->value( key ) );
				xml.writeTextElement( "", name, item->data( 0, Qt::DisplayRole ).toString( ) );
			}
		} else if( clazz == "QGroupBox" ) {
			QGroupBox *groupBox = qobject_cast<QGroupBox *>( widget );
			QString boxName = groupBox->objectName( );
			xml.writeStartElement( boxName );
			writeWidgets( groupBox, xml, props, seen );
			xml.writeEndElement( );
		} else if( clazz == "FileChooser" ) {
			FileChooser *fileChooser = qobject_cast<FileChooser *>( widget );
			QString fileName = fileChooser->fileName( );
			xml.writeStartElement( name );
			xml.writeAttribute( "filename", fileName );
			QFile file( fileName );
			file.open( QFile::ReadOnly );
			QByteArray fileContent = file.readAll( );
			xml.writeAttribute( "size", QString::number( fileContent.length( ) ) );
			QString encoded = QString( fileContent.toBase64( ) );
			file.close( );	
			xml.writeCharacters( encoded );
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
			qDebug( ) << "Write for unknown class" << clazz << "of widget" << widget << "(" << name << ")";
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
		
		if( clazz == "QLineEdit" ) {
			QLineEdit *lineEdit = qobject_cast<QLineEdit *>( widget );
			lineEdit->clear( );
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
			listWidget->clear( );
		} else if( clazz == "QTreeWidget" ) {
			QTreeWidget *treeWidget = qobject_cast<QTreeWidget *>( widget );
			treeWidget->clear( );
		} else if( clazz == "FileChooser" ) {
			FileChooser *fileChooser = qobject_cast<FileChooser *>( widget );
			fileChooser->setFileName( "" );
		} else if( clazz == "PictureChooser" ) {
			PictureChooser *pictureChooser = qobject_cast<PictureChooser *>( widget );
			pictureChooser->setFileName( "" );
		} else if( clazz == "QPushButton" ) {
			// skip, ok, buttons can't be reset
		} else if( clazz == "QWidget" ) {
			// skip, generic widget, don't possibly know how to reset it
		} else {
			qDebug( ) << "Reset for unknown class" << clazz << "of widget" << widget << "(" << name << ")";
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
		props->insert( "action", "read" );
		if( clazz == "QComboBox" ) {
			m_dataLoader->request( form_name, name, QByteArray( ), props );
		} else if( clazz == "QListWidget" ) {
			m_dataLoader->request( form_name, name, QByteArray( ), props );
		} else if( clazz == "QTreeWidget" ) {
			m_dataLoader->request( form_name, name, QByteArray( ), props );
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
		while( !xml.atEnd( ) ) {
			xml.readNext( );
			if( xml.isStartElement( ) ) {
				if( xml.name( ) == "tree" ) {
					_parent = item;
				} else if( xml.name( ) == "item" ) {
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
		qDebug( ) << "Unknown domain for class " << clazz;
	}
	if( xml.hasError( ) ) {
		qDebug( ) << xml.errorString( );
	}
}

void DataHandler::readFormData( QString formName, QWidget *form, QByteArray &data, QHash<QString, QString> *props )
{
	QXmlStreamReader xml( data );
	QWidget *widget = 0;
	bool inForm = false;
	QString clazz;

// search for root element (new) or form name (old)
	QString name;
	if( props->contains( "rootelement" ) ) {
		name = props->value( "rootelement" );
	} else {
		name = formName;
	}
	
	resetFormData( form );
	loadFormDomains( formName, form );
	
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
							QList<QTreeWidgetItem *> items = treeWidget->findItems( text, Qt::MatchExactly | Qt::MatchRecursive, 0 );
							foreach( QTreeWidgetItem *item, items ) {
								item->setSelected( true );
								QTreeWidgetItem *parent = item->parent( );
								while( parent != 0 && parent != treeWidget->invisibleRootItem( ) ) {
									parent->setExpanded( true );
									parent = parent->parent( );
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
						} else {
							qDebug( ) << "Read for unknown class" << clazz << "of widget" << widget << "(" << name << ")";
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

// Note: this is over-simplistic and hard-coded. Should be a generic
// property introspection language similar to the variable system
// in QScript (Javascript), maybe later..
QString DataHandler::readFormVariable( QString variable, QWidget *form )
{
	QStringList parts = variable.split( "." );

	qDebug( ) << "Evaluating variable" << variable;
	
// expecting a widget name as first argument
	if( parts[0].isNull( ) ) {
		qDebug( ) << "Expecting a expression of the form <widget>.<property";
		return QString( );
	}
	QString name = parts[0];
	
// expecting a property identifier as second argument
	if( parts[1].isNull( ) ) {
		qDebug( ) << "Expecting a property name in variable" << variable;
		return QString( );
	}
	QString property = parts[1];
	
	QWidget *widget = qFindChild<QWidget *>( form, name );
// no widget found with that name
	if( !widget ) {
		qDebug( ) << "Unkown widget" << name << "in variable" << variable;
		return QString( );
	}
	
// properties differ depending on the class of the widget	
	QString clazz = widget->metaObject( )->className( );
	if( clazz == "QTreeWidget" ) {
		QTreeWidget *treeWidget = qobject_cast<QTreeWidget *>( widget );
		
// always return data of the selected item (assuming single select for now)
// the ID is currently hard-coded in user data
		if( property == "id" ) {
			QList<QTreeWidgetItem *> items = treeWidget->selectedItems( );
			if( items.empty( ) ) return QString( );
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
		}
	} else {
// non supported class
		qDebug( ) << "Unsupported class" << clazz << "in variable" << variable;
		return QString( );
	}
	
	return QString( );
}

