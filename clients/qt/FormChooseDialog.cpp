/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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

#include "FormChooseDialog.hpp"

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
	
	m_forms = new QListWidget( this );
	formLayout->addRow( tr( "&Available forms:" ), m_forms );
	m_forms->setSelectionMode( QAbstractItemView::SingleSelection );
	m_forms->addItems( m_formNames );
	
	connect( m_forms, SIGNAL( itemDoubleClicked( QListWidgetItem * ) ),
		this, SLOT( ok( ) ) );
	
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
	QList<QListWidgetItem *> items = m_forms->selectedItems( );
	m_form = items.at( 0 )->text( );
	
	accept( );
}

void FormChooseDialog::cancel( )
{
	close( );
}
