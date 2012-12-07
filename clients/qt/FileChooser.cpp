//
// FileChooser.cpp
//

#include "FileChooser.hpp"

#include <QHBoxLayout>
#include <QFileDialog>
#include <QFileInfo>

FileChooser::FileChooser( SelectionMode _selectionMode, QWidget *_parent )
	: QWidget( _parent ), m_selectionMode( _selectionMode ),
	  m_title( ), m_lineEdit( 0 ), m_comboBox( 0 ),
	  m_button( 0 )
{
	initialize( );
}

FileChooser::FileChooser( SelectionMode _selectionMode, const QString _title, QWidget *_parent )
	: QWidget( _parent ), m_selectionMode( _selectionMode ),
	  m_title( _title ), m_lineEdit( 0 ), m_comboBox( 0 ),
	  m_button( 0 )
{
	initialize( );
}

void FileChooser::initialize( )
{	
	QHBoxLayout *l = new QHBoxLayout( this );
          
	m_lineEdit = new QLineEdit( this );
        
        m_comboBox = new QComboBox( this );
        
	m_stackedWidget = new QStackedWidget( this );
	m_stackedWidget->addWidget( m_lineEdit );
	m_stackedWidget->addWidget( m_comboBox );
	l->addWidget( m_stackedWidget );
        
        m_button = new QPushButton( "...", this );
        m_button->setFixedWidth( m_button->fontMetrics( ).width( "..." ) * 2 );
	l->addWidget( m_button );
	
	connect( m_button, SIGNAL( clicked( ) ), this, SLOT( chooseFile( ) ) );
	
// focus goes to line edit, not to the browser button
	setFocusProxy( m_lineEdit );
	setFocusPolicy( Qt::StrongFocus );

// show the right element
	switchStack( );
}

void FileChooser::switchStack( )
{
	switch( m_selectionMode ) {
		case SelectExistingFile:
		case SelectExistingDir:
			m_stackedWidget->setCurrentIndex( 0 );
			break;

		case SelectExistingFiles:		
			m_stackedWidget->setCurrentIndex( 1 );
			break;		
	}
}

void FileChooser::setFileName( const QString &_fileName )
{
	switch( m_selectionMode ) {
		case SelectExistingFile:
		case SelectExistingDir:
			m_lineEdit->setText( _fileName );
			emit fileNameChanged( _fileName );
			break;

		case SelectExistingFiles:
			m_comboBox->clear( );
			m_comboBox->addItem( _fileName );
			QStringList _fileNames;
			_fileNames << _fileName;
			emit fileNamesChanged( _fileNames );
			break;		
	}
}

void FileChooser::setFileNames( const QStringList &_fileNames )
{
	switch( m_selectionMode ) {
		case SelectExistingFile:
		case SelectExistingDir:
			m_lineEdit->setText( _fileNames[0] );
			emit fileNameChanged( _fileNames[0] );
			break;

		case SelectExistingFiles:
			m_comboBox->clear( );
			m_comboBox->addItems( _fileNames );
			break;		
	}
}

QString FileChooser::fileName( ) const
{
	switch( m_selectionMode ) {
		case SelectExistingFile:
		case SelectExistingDir:
			return m_lineEdit->text( );

		case SelectExistingFiles:
			return m_comboBox->itemText( m_comboBox->currentIndex( ) );
	}

	return QString( );
}

QStringList FileChooser::fileNames( ) const
{
	QStringList files;
	
	switch( m_selectionMode ) {
		case SelectExistingFile:
		case SelectExistingDir:
			files << m_lineEdit->text( );
			break;
			
		case SelectExistingFiles:
			for( int i = 0; i < m_comboBox->count( ); i++ ) {
				files << m_comboBox->itemText( i );
			}
			break;
	}
	
	return files;
}

void FileChooser::setSelectionMode( const FileChooser::SelectionMode _mode )
{
	m_selectionMode = _mode;
	switchStack( );
}

FileChooser::SelectionMode FileChooser::selectionMode( ) const
{
	return m_selectionMode;
}

void FileChooser::chooseFile( )
{
	QStringList files;
	QString f;
	QFileInfo finfo( m_lineEdit->text( ) );
	
	switch( m_selectionMode ) {
		case SelectExistingFile:
			f = QFileDialog::getOpenFileName( this,
				m_title.isEmpty( ) ? tr( "Choose a file" ) : m_title,
				finfo.dir( ).absolutePath( ), QString::null /* filter */ );
			break;
		
		case SelectExistingFiles:
			files = QFileDialog::getOpenFileNames( this,
				m_title.isEmpty( ) ? tr( "Choose files" ) : m_title,
				m_lineEdit->text( ), QString::null /* filter */ );
			break;
		
		case SelectExistingDir:
			f = QFileDialog::getExistingDirectory( this,
				m_title.isEmpty( ) ? tr( "Choose a directory" ) : m_title,
				m_lineEdit->text( ), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
			break;
	}
			
	if( !f.isEmpty( ) ) {
		m_lineEdit->setText( f );
		emit fileNameChanged( f );
	}
	
	if( !files.isEmpty( ) ) {
		m_comboBox->clear( );
		m_comboBox->addItems( files );
		emit fileNamesChanged( files );
	}
}
