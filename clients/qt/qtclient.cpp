#include <QApplication>

#include "global.hpp"
#include "MainWindow.hpp"

using namespace _Wolframe::QtClient;

int main( int argc, char* argv[] ) {
	QCoreApplication::setOrganizationName( ORGANIZATION_NAME );
	QCoreApplication::setOrganizationDomain( ORGANIZATION_DOMAIN );
	QCoreApplication::setApplicationName( APPLICATION_NAME );
	
	QApplication app( argc, argv );
	
	MainWindow mainWindow;
#ifdef Q_OS_ANDROID
	mainWindow.setOrientation(MainWindow::ScreenOrientationAuto);
	mainWindow.showExpanded();
#endif
	mainWindow.show( );

	return app.exec( );
}
