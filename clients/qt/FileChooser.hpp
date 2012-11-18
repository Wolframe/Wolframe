//
// FileChooser.hpp
//

#ifndef _FILECHOOSER_HPP_INCLUDED
#define _FILECHOOSER_HPP_INCLUDED

#include <QObject>
#include <QWidget>
#include <QString>
#include <QLineEdit>
#include <QPushButton>

#include <QDesignerExportWidget>

class QDESIGNER_WIDGET_EXPORT FileChooser : public QWidget
{
	Q_OBJECT
	Q_ENUMS( SelectionMode )
	Q_PROPERTY( QString fileName READ fileName WRITE setFileName )
	Q_PROPERTY( SelectionMode selectionMode READ selectionMode WRITE setSelectionMode )
		
	public:
		enum SelectionMode {
			SelectExistingFile,
			SelectExistingDir
		};
		
		FileChooser( SelectionMode _selectionMode, QWidget *_parent = 0 );
		
		QString fileName( ) const;
		SelectionMode selectionMode( ) const;

	signals:
		void fileNameChanged( const QString _fileName );
		
	public slots:
		void setFileName( const QString &_filename );
		void setSelectionMode( const SelectionMode _mode );
	
	private:
		void initialize( );
		
	private slots:
		void chooseFile( );
		
	private:
		SelectionMode m_selectionMode;
		QLineEdit *m_lineEdit;
		QPushButton *m_button;	
};

#endif // _FILECHOOSER_HPP_INCLUDED
