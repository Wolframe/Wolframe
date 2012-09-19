#include <QApplication>

#include "MainWindow.hpp"

using namespace _Wolframe::QtClient;

int main( int argc, char* argv[] ) {
	QApplication app( argc, argv );
	
	MainWindow mainWindow( app );
	mainWindow.show( );

	return app.exec( );
}
