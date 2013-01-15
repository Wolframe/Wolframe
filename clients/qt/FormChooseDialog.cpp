//
// FormChooseDialog.cpp
//

#include "FormChooseDialog.hpp"
#include "Preferences.hpp"
#include "PreferencesDialog.hpp"

#include <QFormLayout>
#include <QApplication>


FormChooseDialog::FormChooseDialog( const QStringList _formNames, QWidget *_parent ) :
	QDialog( _parent ), m_formNames( _formNames )
{
	initialize( );
	setWindowTitle( tr( "Pick a form.." ) );
	setModal( true );
}

void FormChooseDialog::initialize( )
{
	QFormLayout *formLayout = new QFormLayout( );
	
	m_forms = new QComboBox( this );
	formLayout->addRow( tr( "&Available forms:" ), m_forms );
	m_forms->addItems( m_formNames );
	
	m_buttons = new QDialogButtonBox( this );
	m_buttons->addButton( QDialogButtonBox::Ok );
	m_buttons->addButton( QDialogButtonBox::Cancel );
	m_buttons->button( QDialogButtonBox::Ok )->setText( tr( "Open" ) );
	m_buttons->button( QDialogButtonBox::Cancel )->setText( tr( "Cancel" ) );
	formLayout->addRow( m_buttons );
	setLayout( formLayout );
	
	connect( m_buttons->button( QDialogButtonBox::Cancel ), SIGNAL( clicked( ) ),
		this, SLOT( cancel( ) ) );
		
	connect( m_buttons->button( QDialogButtonBox::Ok ), SIGNAL( clicked( ) ),
		this, SLOT( ok( ) ) );	
}

void FormChooseDialog::keyPressEvent( QKeyEvent *_event )
{
	switch( _event->key( ) ) {
		case Qt::Key_Escape:
			_event->ignore( );
			break;
			
		case Qt::Key_Enter:
		case Qt::Key_Return:
			if( m_forms->hasFocus( ) ) {
				m_buttons->button( QDialogButtonBox::Ok )->click( );
			}
			break;
		
		default:
			QDialog::keyPressEvent( _event );
	}
}

void FormChooseDialog::ok( )
{
	m_form = m_forms->itemText( m_forms->currentIndex( ) );
	accept( );
}

void FormChooseDialog::cancel( )
{
	close( );
}