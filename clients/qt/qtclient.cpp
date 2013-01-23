#include <QApplication>
#include <QIcon>

#include "global.hpp"
#include "MainWindow.hpp"

int main( int argc, char* argv[] ) {
	QCoreApplication::setOrganizationName( ORGANIZATION_NAME );
	QCoreApplication::setOrganizationDomain( ORGANIZATION_DOMAIN );
	QCoreApplication::setApplicationName( APPLICATION_NAME );

	int code;
	do {
		Q_INIT_RESOURCE( qtclient );
		
		QApplication app( argc, argv );
		app.setWindowIcon( QIcon( QString( ":/images/wolfClient.png" ) ) );
		MainWindow mainWindow;		
		mainWindow.show( );
		code = app.exec( );
	} while( code == RESTART_CODE );
	

	return code;
}
