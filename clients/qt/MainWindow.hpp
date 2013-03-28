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

#ifndef _MAIN_WINDOW_HPP_INCLUDED
#define _MAIN_WINDOW_HPP_INCLUDED

#include <QMainWindow>
#include <QtUiTools>
#include <QTranslator>
#include <QEvent>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMap>
#include <QAction>
#include <QCloseEvent>

#include "global.hpp"
#include "FormLoader.hpp"
#include "WolframeClient.hpp"
#include "FormWidget.hpp"
#include "loginDialog.hpp"
#include "LoadMode.hpp"
#include "ui_MainWindow.h"
#include "settings.hpp"
#include "connection.hpp"
#include "DebugTerminal.hpp"

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		MainWindow( QWidget *_parent = 0 );
		virtual ~MainWindow( );		
			
	private:
		Ui::MainWindow m_ui;		// ui definition from designer
		QTranslator m_translatorApp;	// contains the translations for this application
		QTranslator m_translatorQt; 	// contains the translations for qt
		FormWidget *m_formWidget;	// current active form
		QUiLoader *m_uiLoader;		// the UI loader for all forms
		FormLoader *m_formLoader;	// form loader (visible form)
		DataLoader *m_dataLoader;	// load and saves data (data form)
		QHash<QString,QVariant> m_globals;// global form variables
		WolframeClient *m_wolframeClient; // the client protocol class
		QString m_settings;		// file to read settings from
		QStringList m_languages;	// available interface translations
		QString m_language;		// the current language of the interface
		QMdiArea *m_mdiArea;		// pointer to MDI workspace in the main window
		QStringList m_forms;		// list of available forms
		QActionGroup *m_subWinGroup;	// group holding list of currently opened MDI subwindows
		QMap<QAction *, QMdiSubWindow *> m_subWinMap; // maps actions to MDI subwindows
		QMap<QMdiSubWindow *, QAction *> m_revSubWinMap; // reverse map (the above is actually a bimap)
		ApplicationSettings settings;	// Application settings
		ConnectionParameters m_selectedConnection; // lastly selected connection
		bool m_terminating;
		QLabel *m_statusBarConn;
		QLabel *m_statusBarSSL;
		DebugTerminal *m_debugTerminal;
		QAction *m_debugTerminalAction;	
		QDialog *m_modalDialog;
		
	public slots:
		void readSettings( );
		void parseArgs( );
		void initialize( );
		void loadLanguages( );
		void loadForm( QString formName );
		void loadLanguage( QString language );
		
	protected:
		void changeEvent( QEvent *_event );
		void closeEvent( QCloseEvent *e );
	
	private:
		void switchTranslator( QTranslator &translator, const QString &filename, const QString &i18n );
		void CreateFormWidget( const QString &name );
		QMdiSubWindow *CreateMdiSubWindow( const QString &form );
		int nofSubWindows( ) const;
		void activateAction( const QString name, bool enabled );
		QString composeWindowListTitle( const int idx, const QString title );
		QKeySequence::StandardKey defaultKeySequenceFromString( const QString s );
		void updateActionShortcuts( );
		void addStatusBarIndicators( );
		void storeSettings( );
		void storeStateAndPositions( );
		void restoreStateAndPositions( );
		void addDeveloperMenu( );

	private slots:
// slots for the wolframe client		
		void wolframeError( QString error );
		void connected( );
		void disconnected( );
		void authOk( );
		void authFailed( );

// menu slots
		void languageSelected( QAction *action );

// form loader slots
		void languageCodesLoaded( QStringList languages );
		void formListLoaded( QStringList forms );
		void formLoaded( QString name );
		void formModal( QString name );
		void endModal( void );
		void formError( QString error );
		
// MDI slots
		void subWindowSelected( QAction *action );
		void subWindowChanged( QMdiSubWindow *w );
		void updateMdiMenusAndToolbars( );
		void updateWindowMenu( );

// developer slots
		void showDebugTerminal( bool checked );
		void removeDebugToggle( );

// generic updating of status in menus and toolbars
		void updateMenusAndToolbars( );

// auto-wired slots for the menu
		void on_actionRestart_triggered( );
		void on_actionExit_triggered( );
		void on_actionPreferences_triggered( );
		void on_actionAbout_triggered( );
		void on_actionAboutQt_triggered( );
		void on_actionOpenForm_triggered( );
		void on_actionOpenFormNewWindow_triggered( );	
		void on_actionReload_triggered( );
		void on_actionNextWindow_triggered( );
		void on_actionPreviousWindow_triggered( );
		void on_actionClose_triggered( );
		void on_actionCloseAll_triggered( );
		void on_actionLogin_triggered( );
		void on_actionLogout_triggered( );
		void on_actionManageServers_triggered( );
};

#endif // _MAIN_WINDOW_HPP_INCLUDED
