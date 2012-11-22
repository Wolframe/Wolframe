//
// PictureChooser.cpp
//

#include "PictureChooser.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFile>
#include <QPixmap>

#include <algorithm>

PictureChooser::PictureChooser( QWidget *_parent )
	: QWidget( _parent ), m_label( 0 ), m_data( 0 ), m_fileChooser( 0 )
{
	initialize( );
}

void PictureChooser::initialize( )
{	
	QVBoxLayout *l = new QVBoxLayout( this );
	
	m_label = new QLabel( this );
	m_label->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
	l->addWidget( m_label );

	m_fileChooser = new FileChooser( FileChooser::SelectExistingFile, false, this );
	l->addWidget( m_fileChooser );
        	
	connect( m_fileChooser, SIGNAL( fileNameChanged( QString ) ),
		this, SLOT( updatePicture( QString ) ) );
}

void PictureChooser::updatePicture( QString _fileName )
{
	QFile file( _fileName );
	if( !file.open( QFile::ReadOnly ) ) return;	
	m_data = file.readAll( );
	file.close( );	
	
	updatePicture( );
}

void PictureChooser::updatePicture( QByteArray _data )
{
	m_data = _data;
	
	updatePicture( );
}

void PictureChooser::updatePicture( )
{
	QPixmap p;
	p.loadFromData( m_data );	
	if( p.isNull( ) ) return;
		
	int w = std::min( m_label->width( ), p.width( ) );
	int h = std::min( m_label->height( ), p.height( ) );
	
	m_label->setPixmap( p.scaled( QSize( w, h ), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );

	m_label->adjustSize( );	
}

void PictureChooser::setFileName( const QString &_fileName )
{
	m_fileChooser->setFileName( _fileName );
}

QString PictureChooser::fileName( ) const
{
	return m_fileChooser->fileName( );
}

void PictureChooser::setPicture( const QByteArray &_data )
{
	updatePicture( _data );
}

QByteArray PictureChooser::picture( ) const
{
	return m_data;
}
