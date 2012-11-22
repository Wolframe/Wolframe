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
	Q_PROPERTY( bool multipleSelection READ multipleSelection WRITE setMultipleSelection )
	Q_PROPERTY( QString fileName READ fileName WRITE setFileName )
	Q_PROPERTY( SelectionMode selectionMode READ selectionMode WRITE setSelectionMode )
		
	public:
		enum SelectionMode {
			SelectExistingFile,
			SelectExistingDir
		};
		
		FileChooser( SelectionMode _selectionMode, bool _multipleSelection, QWidget *_parent = 0 );
		
		bool multipleSelection( ) const;
		QString fileName( ) const;
		SelectionMode selectionMode( ) const;

	signals:
		void fileNameChanged( const QString _fileName );
		
	public slots:
		void setMultipleSelection( const bool _multipleSelection );
		void setFileName( const QString &_filename );
		void setSelectionMode( const SelectionMode _mode );
	
	private:
		void initialize( );
		
	private slots:
		void chooseFile( );
		
	private:
		SelectionMode m_selectionMode;
		bool m_multipleSelection;
		QLineEdit *m_lineEdit;
		QPushButton *m_button;	
};

#endif // _FILECHOOSER_HPP_INCLUDED
