#include <QApplication>

#include "MainWindow.hpp"

using namespace _Wolframe::QtClient;

int main( int argc, char* argv[] ) {
	QApplication app( argc, argv );
	
	MainWindow mainWindow;
#ifdef Q_OS_ANDROID
	mainWindow.setOrientation(MainWindow::ScreenOrientationAuto);
	mainWindow.showExpanded();
#endif
	mainWindow.show( );

	return app.exec( );
}
