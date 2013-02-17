#ifndef _SETTINGS_HPP_INCLUDED
#define _SETTINGS_HPP_INCLUDED

#include <QString>
#include <QVector>
#include <QPoint>
#include <QSize>

#include "connection.hpp"

struct ApplicationSettings
{
	QPoint		mainWindowPos;		///< main window position
	QSize		mainWindowSize;		///< main window size

	bool		developEnabled;		///< development menu enabled ?

	bool		saveUsername;		///< persistent username ?
	QString		lastUsername;		///< last username in case of persisting it
	QString		lastConnection;		///< name of the last used connection

	QVector< ConnectionParameters >	connectionParams; ///< defined connections parameters

public:
	ApplicationSettings();

	void write( const QString& organization, const QString& application );
	void read( const QString& organization, const QString& application );
};

#endif // _SETTINGS_HPP_INCLUDED
