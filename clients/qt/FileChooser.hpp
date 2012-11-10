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

namespace _Wolframe {
	namespace QtClient {

	class FileChooser : public QWidget
	{
	Q_OBJECT
	Q_PROPERTY( QString m_fileName READ fileName WRITE setFileName )
	Q_ENUMS( SelectionMode )
		
	public:
		enum SelectionMode {
			SelectExistingFile,
			SelectExistingDir
		};
		
		FileChooser( SelectionMode _selectionMode, QWidget *_parent = 0 );
		
		QString fileName( ) const;

	public slots:
		void setFileName( const QString &fn );
	
	private:
		void initialize( );
		
	private slots:
		void chooseFile( );
		
	private:
		SelectionMode m_selectionMode;
		QLineEdit *m_lineEdit;
		QPushButton *m_button;	
	};

} // namespace QtClient
} // namespace _Wolframe

#endif // _FILECHOOSER_HPP_INCLUDED
