//
// MainWindow.hpp
//

#ifndef _MAIN_WINDOW_HPP_INCLUDED
#define _MAIN_WINDOW_HPP_INCLUDED

#include <QWidget>

namespace _SMERP {
	namespace QtClient {

	class MainWindow : public QWidget
	{
	public:
		MainWindow( QWidget *_parent = 0 );
		
	private:
		void initialize( );
	};
} // namespace QtClient
} // namespace _SMERP

#endif // _MAIN_WINDOW_HPP_INCLUDED
