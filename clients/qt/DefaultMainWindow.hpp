//
// DefaultMainWindow.hpp
//

#ifndef _DEFAULT_MAIN_WINDOW_HPP_INCLUDED
#define _DEFAULT_MAIN_WINDOW_HPP_INCLUDED

#include <QMainWindow>

#include "MainWindowUi.hpp"

class DefaultMainWindow : public QMainWindow
{
	Q_OBJECT

	private:
		Ui::MainWindowUi *m_ui;

	public:
		DefaultMainWindow( QWidget *_parent = 0 )
		{
			m_ui = new Ui::MainWindowUi( );
			m_ui->setupUi( this );
			setParent( _parent );
		}

		virtual ~DefaultMainWindow( )
		{
			delete m_ui;
		}
	};

#endif // _DEFAULT_MAIN_WINDOW_HPP_INCLUDED
