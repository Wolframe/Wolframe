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

#include <QCloseEvent>
#include <QMessageBox>

#include "loginDialog.hpp"
#include "ui_loginDialog.h"

#include "manageServersDialog.hpp"

LoginDialog::LoginDialog( const QString& username,
			      const QString& connName, QVector< ConnectionParameters >& connParams,
			      QWidget *parent ) :
	QDialog( parent ), m_connParams( connParams ), ui( new Ui::LoginDialog )
{
	ui->setupUi( this );
	ui->userInput->setText( username );

	for ( int i = 0; i < m_connParams.size(); i++ )
		ui->serverCombo->addItem( m_connParams[ i ].name );
	int index = ui->serverCombo->findText( connName );
	ui->serverCombo->setCurrentIndex( index );

	connect( ui->serverManageButton, SIGNAL( clicked() ), this, SLOT( manageConnections() ));

	if ( m_connParams.size() == 0 )	{
		QMessageBox::information( this, tr( "Login" ),
					  tr( "There are no servers defined.\nPlease define a server." ));
		manageConnections();
	}
}

LoginDialog::~LoginDialog()
{
	delete ui;
}

void LoginDialog::manageConnections()
{
	ManageServersDialog* manageDlg = new ManageServersDialog( m_connParams );
	if ( manageDlg->exec() )	{
		QString conn = ui->serverCombo->currentText();
		ui->serverCombo->clear();
		for ( int i = 0; i < m_connParams.size(); i++ )
			ui->serverCombo->addItem( m_connParams[ i ].name );
		if ( ! conn.isEmpty() )	{
			int index = ui->serverCombo->findText( conn );
			ui->serverCombo->setCurrentIndex( index );
		}
	}

	delete manageDlg;
}

ConnectionParameters LoginDialog::selectedConnection( )
{
	return m_connParams[ ui->serverCombo->currentIndex( ) ];
}

QString LoginDialog::username( )
{
	return ui->userInput->text( );
}
