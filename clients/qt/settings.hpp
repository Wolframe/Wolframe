#ifndef _SETTINGS_HPP_INCLUDED
#define _SETTINGS_HPP_INCLUDED

#include <QString>
#include <QVector>
#include <QPoint>
#include <QSize>
#include <QSettings>

#include "connection.hpp"
#include "LoadMode.hpp"

struct ApplicationSettings
{
	QPoint		mainWindowPos;		///< main window position
	QSize		mainWindowSize;		///< main window size

	bool		developEnabled;		///< development menu enabled ?

	bool		saveUsername;		///< persistent username ?
	QString		lastUsername;		///< last username in case of persisting it
	QString		lastConnection;		///< name of the last used connection

	QVector< ConnectionParameters >	connectionParams; ///< defined connections parameters

	bool mdi;
	bool debug;
	QString locale;
	LoadMode uiLoadMode;
	LoadMode dataLoadMode;
	QString uiFormsDir;
	QString uiFormTranslationsDir;
	QString uiFormResourcesDir;
	QString dataLoaderDir;

public:
	ApplicationSettings();

	void write( const QString& organization, const QString& application );
	void read( const QString& organization, const QString& application );
	void write( const QString &fileName );
	void read( const QString &fileName );

private:
	void write( QSettings &settings );
	void read( QSettings &settings );
};

#endif // _SETTINGS_HPP_INCLUDED
