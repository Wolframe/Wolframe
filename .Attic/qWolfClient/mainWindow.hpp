#ifndef _MAIN_WINDOW_HPP_INCLUDED
#define _MAIN_WINDOW_HPP_INCLUDED

#include <QMainWindow>
#include <QLabel>

#include "settings.hpp"

class MdiChild;
QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();

protected:
	void closeEvent( QCloseEvent *event );

private slots:
	// Connection menu actions
	void login();
	void logout();
	void print();
	void manageServers();
	void preferences();
	// Edit menu actions
	void undo();
	void redo();
	void cut();
	void copy();
	void paste();
	void del();
	void selectAll();
	// Window menu
	void reload();
	// Development menu
	void manageInterface();
	// Help menu
	void helpContents();
	void helpIndex();
	void contextHelp();
	void technicalSupport();
	void reportBug();
	void checkUpdates();
	void about();

	void updateMenus();
    void updateStatusBar();
    void updateWindowMenu();
    MdiChild *createMdiChild();
    void setActiveSubWindow(QWidget *window);

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();

    MdiChild *activeMdiChild();
    QMdiSubWindow *findMdiChild(const QString &fileName);

	QMdiArea	*m_mdiArea;
    QSignalMapper *windowMapper;

	QMenu		*connectMenu;
	QMenu		*editMenu;
	QMenu		*windowMenu;
	QMenu		*developMenu;
	QMenu		*helpMenu;

	// Toolbars
	QToolBar	*loginToolBar;
	QToolBar	*editToolBar;
	QToolBar	*windowToolBar;

	// Connection menu
	QAction		*loginAction;
	QAction		*logoutAction;
	QAction		*printAction;
	QAction		*manageServersAction;
	QAction		*preferencesAction;
	QAction		*exitAction;

	// Development menu
	QAction		*manageInterfaceAction;

	// Edit menu
	QAction *undoAction;
	QAction *redoAction;
	QAction *cutAction;
	QAction *copyAction;
	QAction *pasteAction;
	QAction		*delAction;
	QAction *selectAllAction;

	// Window menu
	QAction *reloadAction;
	QAction *closeAction;
	QAction *closeAllAction;
	QAction *tileAction;
	QAction *cascadeAction;
	QAction *nextAction;
	QAction *previousAction;
	QAction *wndMenuSeparatorAction;	// conditional separator

	// Help menu
	QAction *helpContentsAction;
	QAction *helpIndexAction;
	QAction *contextHelpAction;
	QAction *technicalSupportAction;
	QAction *reportBugAction;
	QAction *checkUpdtAction;
	QAction *aboutAction;
	QAction *aboutQtAction;

	// Status bar indicators
	QLabel *m_statusBarConn;
	QLabel *m_statusBarSSL;

	// Application settings
	ApplicationSettings	settings;
};

#endif // _MAIN_WINDOW_HPP_INCLUDED
