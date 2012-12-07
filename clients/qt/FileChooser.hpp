//
// FileChooser.hpp
//

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
		
	public:
		enum SelectionMode {
			SelectExistingFile,
			SelectExistingFiles,
			SelectExistingDir
		};
		
		FileChooser( SelectionMode _selectionMode, QWidget *_parent = 0 );
		FileChooser( SelectionMode _selectionMode, const QString title, QWidget *_parent = 0 );
		
		QString fileName( ) const;
		QStringList fileNames( ) const;
		SelectionMode selectionMode( ) const;

	signals:
		void fileNameChanged( const QString _fileName );
		void fileNamesChanged( const QStringList _fileNames );
		
	public slots:
		void setFileName( const QString &_filename );
		void setFileNames( const QStringList &_filenames );
		void setSelectionMode( const SelectionMode _mode );
	
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
