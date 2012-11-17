//
// FileChooser.cpp
//

#include "FileChooser.hpp"

#include <QHBoxLayout>
#include <QFileDialog>
#include <QDebug>

FileChooser::FileChooser( SelectionMode _selectionMode, QWidget *_parent )
	: QWidget( _parent ), m_selectionMode( _selectionMode ),
	  m_lineEdit( 0 ), m_button( 0 )
{
	qDebug( ) << "FileChooser" << this;
	initialize( );
}

void FileChooser::initialize( )
{
	qDebug( ) << "FileChooser initialize" << this;
	
	QHBoxLayout *l = new QHBoxLayout( this );
	l->setMargin( 0 );

	m_lineEdit = new QLineEdit( this );
        l->addWidget( m_lineEdit );
        
        m_button = new QPushButton( "...", this );
        m_button->setFixedWidth( m_button->fontMetrics( ).width( "..." ) * 2 );
	l->addWidget( m_button );
	
	connect( m_button, SIGNAL( clicked( ) ), this, SLOT( chooseFile( ) ) );
	
// focus goes to line edit, not to the browser button
	setFocusProxy( m_lineEdit );
}

void FileChooser::setFileName( const QString &_fileName )
{
	m_lineEdit->setText( _fileName );
}

QString FileChooser::fileName( ) const
{
	return m_lineEdit->text( );
}

void FileChooser::setSelectionMode( const FileChooser::SelectionMode _mode )
{
	m_selectionMode = _mode;
}

FileChooser::SelectionMode FileChooser::selectionMode( ) const
{
	return m_selectionMode;
}

void FileChooser::chooseFile( )
{
	QString f;
	
	switch( m_selectionMode ) {
		case SelectExistingFile:
			f = QFileDialog::getOpenFileName( this, tr( "Choose a file" ),
				m_lineEdit->text( ), QString::null /* filter */ );
			break;
		
		case SelectExistingDir:
			f = QFileDialog::getExistingDirectory( this, tr( "Choose a directory" ),
				m_lineEdit->text( ), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
			break;
	}
			
	if( !f.isEmpty( ) ) {
		m_lineEdit->setText( f );
	}
}
