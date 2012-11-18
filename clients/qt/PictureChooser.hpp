//
// PictureChooser.hpp
//

#ifndef _PICTURECHOOSER_HPP_INCLUDED
#define _PICTURECHOOSER_HPP_INCLUDED

#include "FileChooser.hpp"

#include <QObject>
#include <QWidget>
#include <QString>
#include <QByteArray>
#include <QLabel>

#include <QDesignerExportWidget>

class QDESIGNER_WIDGET_EXPORT PictureChooser : public QWidget
{
	Q_OBJECT
	Q_PROPERTY( QString fileName READ fileName WRITE setFileName )
		
	public:
		PictureChooser( QWidget *_parent = 0 );
		QString fileName( ) const;
		QByteArray picture( ) const;

	public slots:
		void setFileName( const QString &_filename );
		void setPicture( const QByteArray &_data );

	private:
		void initialize( );
				
	private slots:
		void updatePicture( );
		void updatePicture( QString _fileName );
		void updatePicture( QByteArray _data );
		
	private:
		QLabel *m_label;
		QByteArray m_data;
		FileChooser *m_fileChooser;
};

#endif // _PICTURECHOOSER_HPP_INCLUDED
