/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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

#ifndef _FILECHOOSER_HPP_INCLUDED
#define _FILECHOOSER_HPP_INCLUDED

#include <QObject>
#include <QWidget>
#include <QString>
#include <QStringList>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QStackedWidget>

#include <QDesignerExportWidget>

class QDESIGNER_WIDGET_EXPORT FileChooser : public QWidget
{
	Q_OBJECT
	Q_ENUMS( SelectionMode )
	Q_PROPERTY( SelectionMode selectionMode READ selectionMode WRITE setSelectionMode )
	Q_PROPERTY( QString title READ title WRITE setTitle )
		
	public:
		enum SelectionMode {
			SelectExistingFile,
			SelectExistingFiles,
			SelectExistingDir
		};
		
		FileChooser( QWidget *_parent = 0 );
		FileChooser( SelectionMode _selectionMode, QWidget *_parent = 0 );
		FileChooser( SelectionMode _selectionMode, const QString title, QWidget *_parent = 0 );
		
		QString fileName( ) const;
		QStringList fileNames( ) const;
		SelectionMode selectionMode( ) const;
		QString title( ) const;

	signals:
		void fileNameChanged( const QString _fileName );
		void fileNamesChanged( const QStringList _fileNames );
		
	public slots:
		void setFileName( const QString &_filename );
		void setFileNames( const QStringList &_filenames );
		void setSelectionMode( const SelectionMode _mode );
		void setTitle( const QString &_title );
	
	private:
		void initialize( );
		void switchStack( );
		
	private slots:
		void chooseFile( );
		
	private:
		SelectionMode m_selectionMode;
		QString m_title;
		QStackedWidget *m_stackedWidget;
		QLineEdit *m_lineEdit;
		QComboBox *m_comboBox;
		QPushButton *m_button;	
};

#endif // _FILECHOOSER_HPP_INCLUDED
