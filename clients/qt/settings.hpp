#ifndef _SETTINGS_HPP_INCLUDED
#define _SETTINGS_HPP_INCLUDED

#include <QString>
#include <QVector>
#include <QPoint>
#include <QSize>
#include <QSettings>

#include "connection.hpp"
#include "LoadMode.hpp"

struct WinState
{
	QString form;				///< last visible form
	QPoint position;			///< last position (for MDI)
	QSize size;				///< last size (for MDI)
};

struct ApplicationSettings
{
	QPoint		mainWindowPos;		///< main window position
	QSize		mainWindowSize;		///< main window size

	bool		developEnabled;		///< development menu enabled ?

	bool		saveUsername;		///< persistent username ?
	QString		lastUsername;		///< last username in case of persisting it
	QString		lastConnection;		///< name of the last used connection

	QVector< ConnectionParameters >	connectionParams; ///< defined connections parameters

	bool mdi;				///< MDI mode with many forms
	bool debug;				///< debug output
	QString locale;				///< startup locale
	LoadMode uiLoadMode;			///< developer UI file load mode or network mode
	LoadMode dataLoadMode;			///< data forms to network or local stubs in XML files
	QString uiFormsDir;			///< UI forms
	QString uiFormTranslationsDir;		///< QM files (translations)
	QString uiFormResourcesDir;		///< resources (e.g. pictures)
	QString dataLoaderDir;			///< XML demo data simulating data forms

	bool saveRestoreState;
	QVector<WinState> states;		///< states of subwindow(s)

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
