//
// ManageStorageDialog.cpp
//

#include "ManageStorageDialog.hpp"
#include "Preferences.hpp"
#include "LoadMode.hpp"

#include <QFormLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QHBoxLayout>


ManageStorageDialog::ManageStorageDialog( QWidget *_parent ) :
	QDialog( _parent )
{
	initialize( );
	
	setWindowTitle( tr( "Preferences" ) );
	setModal( true );
}

void ManageStorageDialog::initialize( )
{
	QHBoxLayout *l = new QHBoxLayout( );
	
	m_leftStorage = new StorageWidget( this );
	l->addWidget( m_leftStorage );
	
	QVBoxLayout *lb = new QVBoxLayout( );
	m_left = new QPushButton( tr( "<--" ), this );
	lb->addWidget( m_left );
	m_right = new QPushButton( tr( "-->" ), this );
	lb->addWidget( m_right );
	m_syncMode = new QComboBox( this );
	m_syncMode->addItem( tr( "Left to right" ) );
	m_syncMode->addItem( tr( "Right to left" ) );
	m_syncMode->addItem( tr( "Force left to right" ) );
	m_syncMode->addItem( tr( "Force right to left" ) );
	lb->addWidget( m_syncMode );
	m_synchronize = new QPushButton( tr( "&Synchronize" ), this );
	lb->addWidget( m_synchronize );
	m_delete = new QPushButton( tr( "&Delete" ), this );
	lb->addWidget( m_delete );
	m_exit = new QPushButton( tr( "&Exit" ), this );
	lb->addWidget( m_exit );
	l->addLayout( lb );

	m_rightStorage = new StorageWidget( this );
	l->addWidget( m_rightStorage );
		
	setLayout( l );

	connect( m_exit, SIGNAL( clicked( ) ), this, SLOT( terminate( ) ) );	
}

void ManageStorageDialog::terminate( )
{
	close( );
}

