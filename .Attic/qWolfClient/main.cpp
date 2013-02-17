#include <QApplication>

#include "mainWindow.hpp"

int main( int argc, char *argv[] )
{
	Q_INIT_RESOURCE( qWolfClient );

	QApplication app( argc, argv );
	MainWindow mainWin;
	mainWin.setWindowIcon( QIcon( ":/images/wolfClient.png" ));
	mainWin.show();
	return app.exec();
}
