/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
 All rights reserved.

 This file is part of Project Wolframe.

 Commercial Usage
    Licensees holding valid Project Wolframe Commercial licenses may
    use this file in accordance with the Project Wolframe
    Commercial License Agreement provided with the Software or,
    alternatively, in accordance with the terms contained
    in a written agreement between the licensee and Project Wolframe.

 GNU General Public License Usage
    Alternatively, you can redistribute this file and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Wolframe is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/

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
