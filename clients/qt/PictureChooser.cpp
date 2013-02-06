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

	m_fileChooser = new FileChooser( FileChooser::SelectExistingFile, this );
	m_fileChooser->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );
	l->addWidget( m_fileChooser );
        	
	connect( m_fileChooser, SIGNAL( fileNameChanged( QString ) ),
		this, SLOT( updatePicture( QString ) ) );

// focus goes to file chooser, not to the label with image
	setFocusProxy( m_fileChooser );
	setFocusPolicy( Qt::StrongFocus );
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
