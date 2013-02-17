#include <QDialog>
#include <QMessageBox>
#include <QtGui>
#include <cassert>

#include "manageServersDialog.hpp"
#include "ui_manageServersDialog.h"

#include "serverDefinitionDialog.hpp"

ManageServersDialog::ManageServersDialog( QVector<ConnectionParameters>& params,
					  QWidget *parent ) :
	QDialog( parent ), ui( new Ui::ManageServersDialog ),
	m_globalParams( params )
{
	m_localParams = m_globalParams;
	ui->setupUi( this );
	ui->connectionList->sortItems();
	ui->connectionList->setSelectionMode( QAbstractItemView::SingleSelection );
	for ( QVector<ConnectionParameters>::const_iterator it = m_localParams.begin();
							it != m_localParams.end(); it++ )
		ui->connectionList->addItem( it->name );
	connect( ui->newConnBttn, SIGNAL( clicked() ), this, SLOT( newConnection() ));
	connect( ui->editConBttn, SIGNAL( clicked() ), this, SLOT( editConnection() ));
	connect( ui->delConnBttn, SIGNAL( clicked() ), this, SLOT( delConnection() ));
	connect( ui->connectionList, SIGNAL( itemSelectionChanged() ), this, SLOT( updateButtons() ));
	connect( ui->connectionList, SIGNAL( itemSelectionChanged() ), this, SLOT( connectionBrief() ));

	updateButtons();
}

ManageServersDialog::~ManageServersDialog()
{
	delete ui;
}

void ManageServersDialog::done( int retCode )
{
	if ( retCode == QDialog::Accepted )	{
		m_globalParams.clear();
		m_globalParams = m_localParams;
		QDialog::done( retCode );
	}
	else
		QDialog::done( retCode );
}

void ManageServersDialog::newConnection()
{
	ConnectionParameters conn;
	ServerDefinitionDialog* newConn = new ServerDefinitionDialog( conn, this );
	newConn->windowTitle() = tr( "New connection parameters" );

	bool duplicate;
	do	{
		duplicate = false;
		if ( newConn->exec() )	{
			for ( int i = 0; i < m_localParams.size(); i++ )	{
				if ( m_localParams[ i ].name.compare( conn.name, Qt::CaseInsensitive ) == 0 )	{
					duplicate = true;
					QString msg = QString( "A definition named '%1' already exists" ).arg( m_localParams[ i ].name );
					QMessageBox::critical( this, tr( "New definition" ), msg );
					break;
				}
			}
			if ( ! duplicate )	{
				m_localParams.append( conn );
				ui->connectionList->clear();
				for ( int i = 0; i < m_localParams.size(); i++ )
					ui->connectionList->addItem( m_localParams[ i ].name );
			}
		}
	} while( duplicate );

	delete newConn;
}

void ManageServersDialog::editConnection()
{
	QList< QListWidgetItem* > items = ui->connectionList->selectedItems();
	assert( items.size() == 1 );

	int	pos;
	ConnectionParameters conn;
	QString name = items.first()->data( 0 ).toString();
	for ( pos = 0; pos < m_localParams.size(); pos++ )	{
		if ( m_localParams[ pos ].name.compare( name, Qt::CaseInsensitive ) == 0 )	{
			conn = m_localParams[ pos ];
			break;
		}
	}
	assert( pos >= 0 && pos < m_localParams.size() );

	ServerDefinitionDialog* editConn = new ServerDefinitionDialog( conn, this );
	editConn->windowTitle() = tr( "Edit connection parameters" );

	bool duplicate;
	do	{
		duplicate = false;
		if ( editConn->exec() )	{
			for ( int i = 0; i < m_localParams.size(); i++ )	{
				if ( m_localParams[ i ].name.compare( conn.name, Qt::CaseInsensitive ) == 0 && i != pos )	{
					duplicate = true;
					QString msg = QString( "A definition named '%1' already exists" ).arg( m_localParams[ i ].name );
					QMessageBox::critical( this, tr( "Edit definition" ), msg );
					break;
				}
			}
			if ( ! duplicate )	{
				m_localParams[ pos ] = conn;
				ui->connectionList->clear();
				for ( int i = 0; i < m_localParams.size(); i++ )
					ui->connectionList->addItem( m_localParams[ i ].name );
			}
		}
	} while( duplicate );

	delete editConn;
}

void ManageServersDialog::delConnection()
{
	QList< QListWidgetItem* > items = ui->connectionList->selectedItems();
	assert( items.size() == 1 );

	QString conn = items.first()->data( 0 ).toString();
	for ( int i = 0; i < m_localParams.size(); i++ )	{
		if ( m_localParams[ i ].name.compare( conn, Qt::CaseInsensitive ) == 0 )	{
			QString msg = QString( "Delete the definition for '%1' ?" ).arg( m_localParams[ i ].name );
			QMessageBox::StandardButton ret = QMessageBox::question( this, tr( "Delete definition" ),
										 msg, QMessageBox::Ok | QMessageBox::Cancel );
			if ( ret == QMessageBox::Ok )	{
				m_localParams.remove( i );
				ui->connectionList->clear();
				for ( int i = 0; i < m_localParams.size(); i++ )
					ui->connectionList->addItem( m_localParams[ i ].name );
			}
			break;
		}
	}
}

void ManageServersDialog::updateButtons()
{
	QList< QListWidgetItem* > items = ui->connectionList->selectedItems();
	if ( items.empty() )	{
		ui->newConnBttn->setEnabled( true );
		ui->editConBttn->setEnabled( false );
		ui->delConnBttn->setEnabled( false );
	}
	else	{
		ui->newConnBttn->setEnabled( true );
		ui->editConBttn->setEnabled( true );
		ui->delConnBttn->setEnabled( true );
	}
}

void ManageServersDialog::connectionBrief() const
{
	if ( ui->connectionList->currentItem() )	{
		QString name = ui->connectionList->currentItem()->text();
		for ( QVector< ConnectionParameters >::const_iterator it = m_localParams.begin();
									it != m_localParams.end(); it++ )	{
			if ( it->name.compare( name, Qt::CaseInsensitive ) == 0 )	{
				ui->parametersLbl->setText( it->toString() );
				break;
			}
		}
	}
	else
		ui->parametersLbl->setText( tr( "N/A" ) );
}
