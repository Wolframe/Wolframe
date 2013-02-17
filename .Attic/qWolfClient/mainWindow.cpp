
#include <QtGui>

#include "mainWindow.hpp"
#include "mdiChild.hpp"
#include "settings.hpp"
#include "loginDialog.hpp"
#include "manageServersDialog.hpp"
#include "preferencesDialog.hpp"


MainWindow::MainWindow()
{
	// Set the MDI area
	m_mdiArea = new QMdiArea;
	m_mdiArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
	m_mdiArea->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
	setCentralWidget( m_mdiArea );

    connect(m_mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
	    this, SLOT(updateMenus()));
    windowMapper = new QSignalMapper(this);
    connect( windowMapper, SIGNAL(mapped(QWidget*)),
	    this, SLOT(setActiveSubWindow(QWidget*)));

	createActions();
	createMenus();
	createToolBars();
	createStatusBar();
	updateMenus();

	settings.read( "Wolframe", "Wolframe Client" );
	move( settings.mainWindowPos );
	resize( settings.mainWindowSize );

	setWindowTitle( tr( "Wolframe Client" ));
	setUnifiedTitleAndToolBarOnMac( true );
}

void MainWindow::closeEvent( QCloseEvent *event )
{
	m_mdiArea->closeAllSubWindows();
	if ( m_mdiArea->currentSubWindow())	{
		event->ignore();
	}
	else	{
		settings.mainWindowPos = pos();
		settings.mainWindowSize = size();
		settings.write( "Wolframe", "Wolframe Client" );
		event->accept();
	}
}

void MainWindow::login()
{
	QString	username;
//	QString	password;
	QString	connName;

	if ( settings.saveUsername )	{
		username = settings.lastUsername;
		connName = settings.lastConnection;
	}

	LoginDialog* loginDlg = new LoginDialog( username, connName,
						 settings.connectionParams );
	loginDlg->exec();

	delete loginDlg;
}

void MainWindow::logout()
{
	QMessageBox::information( this, tr( "Feature not implemented" ),
				  tr( "Disconnect\n\nThis feature has not been implemented yet" ));
}

void MainWindow::print()
{
	QMessageBox::information( this, tr( "Feature not implemented" ),
				  tr( "Print\n\nThis feature has not been implemented yet" ));
}

void MainWindow::manageServers()
{
	ManageServersDialog* serversDlg = new ManageServersDialog( settings.connectionParams );
	serversDlg->exec();

	delete serversDlg;
}

void MainWindow::preferences()
{
	PreferencesDialog* preferencesDlg = new PreferencesDialog();
	preferencesDlg->exec();

	delete preferencesDlg;
}

void MainWindow::undo()
{
	if ( activeMdiChild() )
		activeMdiChild()->undo();
}

void MainWindow::redo()
{
	if ( activeMdiChild() )
		activeMdiChild()->redo();
}

void MainWindow::cut()
{
	if ( activeMdiChild() )
		activeMdiChild()->cut();
}

void MainWindow::copy()
{
	if ( activeMdiChild() )
		activeMdiChild()->copy();
}

void MainWindow::paste()
{
	if ( activeMdiChild() )
		activeMdiChild()->paste();
}

void MainWindow::del()
{
	if ( activeMdiChild() )
		activeMdiChild()->cut();
}

void MainWindow::selectAll()
{
	if ( activeMdiChild() )
		activeMdiChild()->selectAll();
}

void MainWindow::reload()
{
	QMessageBox::information( this, tr( "Feature not implemented" ),
				  tr( "Reload window data\n\nThis feature has not been implemented yet" ));
}

void MainWindow::manageInterface()
{
	QMessageBox::information( this, tr( "Feature not implemented" ),
				  tr( "Manage Interface\n\nThis feature has not been implemented yet" ));
}

// Help menu
void MainWindow::helpContents()
{
	QMessageBox::information( this, tr( "Feature not implemented" ),
				  tr( "Help Contents\n\nThis feature has not been implemented yet" ));
}

void MainWindow::helpIndex()
{
	QMessageBox::information( this, tr( "Feature not implemented" ),
				  tr( "Help Index\n\nThis feature has not been implemented yet" ));
}

void MainWindow::contextHelp()
{
	QMessageBox::information( this, tr( "Feature not implemented" ),
				  tr( "Context Help\n\nThis feature has not been implemented yet" ));
}

void MainWindow::technicalSupport()
{
	QMessageBox::information( this, tr( "Feature not implemented" ),
				  tr( "Technical Support\n\nThis feature has not been implemented yet" ));
}

void MainWindow::reportBug()
{
	QMessageBox::information( this, tr( "Feature not implemented" ),
				  tr( "Report Bug\n\nThis feature has not been implemented yet" ));
}

void MainWindow::checkUpdates()
{
	QMessageBox::information( this, tr( "Feature not implemented" ),
				  tr( "Check for Updates\n\nThis feature has not been implemented yet" ));
}

void MainWindow::about()
{
	QMessageBox::about( this, tr( "About Wolframe Client" ),
			    tr( "About the <b>Wolframe Client</b>.") );
}

void MainWindow::updateMenus()
{
//	connectAction->setEnabled( !hasConnection );
//	disconnectAction->setEnabled( hasConnection );

	bool hasMdiChild = ( activeMdiChild() != 0 );
	pasteAction->setEnabled( hasMdiChild );

	reloadAction->setEnabled( hasMdiChild );
	closeAction->setEnabled( hasMdiChild );
	closeAllAction->setEnabled( hasMdiChild );
	tileAction->setEnabled( hasMdiChild );
	cascadeAction->setEnabled( hasMdiChild );
	nextAction->setEnabled( hasMdiChild );
	previousAction->setEnabled( hasMdiChild );
	wndMenuSeparatorAction->setVisible( hasMdiChild );

	bool hasSelection = ( hasMdiChild && activeMdiChild()->textCursor().hasSelection() );
	undoAction->setEnabled( hasMdiChild && activeMdiChild()->document()->availableUndoSteps() );
	redoAction->setEnabled( hasMdiChild && activeMdiChild()->document()->availableRedoSteps() );
	cutAction->setEnabled( hasSelection );
	copyAction->setEnabled( hasSelection );
	delAction->setEnabled( hasSelection );
	selectAllAction->setEnabled( hasMdiChild );
}

void MainWindow::updateWindowMenu()
{
	windowMenu->clear();
	windowMenu->addAction( reloadAction );
	windowMenu->addSeparator();
	windowMenu->addAction( closeAction );
	windowMenu->addAction( closeAllAction );
	windowMenu->addSeparator();
	windowMenu->addAction( tileAction );
	windowMenu->addAction( cascadeAction );
	windowMenu->addSeparator();
	windowMenu->addAction( nextAction );
	windowMenu->addAction( previousAction );
	windowMenu->addAction( wndMenuSeparatorAction );

	QList< QMdiSubWindow* > windows = m_mdiArea->subWindowList();
	wndMenuSeparatorAction->setVisible( !windows.isEmpty() );

	for ( int i = 0; i < windows.size(); ++i )	{
		MdiChild *child = qobject_cast< MdiChild *>( windows.at( i )->widget() );

		QString text;
		if ( i < 9 )	{
			text = tr( "&%1 %2" ).arg( i + 1 ).arg( child->userFriendlyCurrentFile() );
		}
		else	{
			text = tr( "%1 %2" ).arg( i + 1 ).arg( child->userFriendlyCurrentFile() );
		}
		QAction *action  = windowMenu->addAction( text );
		action->setCheckable( true );
		action->setChecked( child == activeMdiChild() );
		connect( action, SIGNAL( triggered() ), windowMapper, SLOT( map() ));
		windowMapper->setMapping( action, windows.at( i ) );
	}
}

MdiChild *MainWindow::createMdiChild()
{
	MdiChild *child = new MdiChild;
	m_mdiArea->addSubWindow( child );

	connect( child, SIGNAL( undoAvailable( bool )), undoAction, SLOT( setEnabled( bool ) ));
	connect( child, SIGNAL( redoAvailable( bool )), redoAction, SLOT( setEnabled( bool ) ));
	connect( child, SIGNAL( copyAvailable( bool )), cutAction, SLOT( setEnabled( bool ) ));
	connect( child, SIGNAL( copyAvailable( bool )), copyAction, SLOT( setEnabled( bool ) ));
	connect( child, SIGNAL( copyAvailable( bool )), delAction, SLOT( setEnabled( bool ) ));

	return child;
}

void MainWindow::createActions()
{
	// Connection menu actions
	loginAction = new QAction( QIcon( ":/images/login.png" ), tr( "&Login..." ), this );
	loginAction->setStatusTip( tr( "Login to a Wolframe server" ) );
	connect( loginAction, SIGNAL( triggered() ), this, SLOT( login() ));

	logoutAction = new QAction( QIcon( ":/images/logout.png" ), tr( "L&ogout" ), this );
	logoutAction->setStatusTip( tr( "Logout from the Wolframe server" ) );
	connect( logoutAction, SIGNAL( triggered() ), this, SLOT( logout() ));

	manageServersAction = new QAction( tr( "&Manage servers..." ), this );
	manageServersAction->setStatusTip( tr( "Manage the list of Wolframe servers" ) );
	connect( manageServersAction, SIGNAL( triggered() ), this, SLOT( manageServers() ));

	preferencesAction = new QAction( QIcon( ":/images/configure.png" ), tr( "P&references..." ), this );
	preferencesAction->setStatusTip( tr( "Configure application preferences" ));
	connect( preferencesAction, SIGNAL( triggered() ), this, SLOT( preferences() ));

	printAction = new QAction( QIcon( ":/images/print.png" ), tr( "&Print..." ), this );
	printAction->setShortcuts( QKeySequence::Print );
	printAction->setStatusTip( tr( "Print current document" ) );
	connect( printAction, SIGNAL( triggered() ), this, SLOT( print() ));

	exitAction = new QAction( QIcon( ":/images/exit.png" ), tr( "E&xit" ), this );
	exitAction->setShortcuts( QKeySequence::Quit );
	exitAction->setStatusTip( tr( "Exit the Wolframe client" ) );
	connect( exitAction, SIGNAL( triggered() ), qApp, SLOT( closeAllWindows() ));

	// Edit menu actions
	undoAction = new QAction( QIcon( ":/images/undo.png" ), tr( "&Undo" ), this );
	undoAction->setShortcuts( QKeySequence::Undo );
	undoAction->setStatusTip( tr( "Undo the last action" ));
	undoAction->setToolTip( tr( "Undo the last action" ));
	connect( undoAction, SIGNAL( triggered() ), this, SLOT( undo() ));

	redoAction = new QAction( QIcon( ":/images/redo.png" ), tr( "&Redo" ), this );
	redoAction->setShortcuts( QKeySequence::Redo );
	redoAction->setStatusTip( tr( "Redo the last undo" ));
	connect( redoAction, SIGNAL( triggered() ), this, SLOT( redo() ));

	cutAction = new QAction( QIcon( ":/images/cut.png" ), tr( "Cu&t" ), this );
	cutAction->setShortcuts( QKeySequence::Cut );
	cutAction->setStatusTip( tr( "Cut the current selection's contents to the clipboard" ));
	connect( cutAction, SIGNAL( triggered() ), this, SLOT( cut() ));

	copyAction = new QAction( QIcon( ":/images/copy.png" ), tr( "&Copy" ), this );
	copyAction->setShortcuts( QKeySequence::Copy );
	copyAction->setStatusTip( tr( "Copy the current selection's contents to the clipboard" ));
	connect( copyAction, SIGNAL( triggered() ), this, SLOT( copy() ));

	pasteAction = new QAction( QIcon( ":/images/paste.png" ), tr( "&Paste" ), this );
	pasteAction->setShortcuts( QKeySequence::Paste );
	pasteAction->setStatusTip( tr( "Paste the clipboard's contents into the current selection" ));
	connect( pasteAction, SIGNAL( triggered() ), this, SLOT( paste() ));

	delAction = new QAction( QIcon( ":/images/delete.png" ), tr( "Delete" ), this );
	delAction->setShortcuts( QKeySequence::Delete );
	delAction->setStatusTip( tr( "Delete the current selection's contents" ));
	connect( delAction, SIGNAL( triggered() ), this, SLOT( del() ));

	selectAllAction = new QAction( tr( "Select &All" ), this );
	selectAllAction->setShortcuts( QKeySequence::SelectAll );
	selectAllAction->setStatusTip( tr( "Select all items in the current window" ));
	connect( selectAllAction, SIGNAL( triggered() ), this, SLOT( selectAll() ));

	// Window menu actions
	reloadAction = new QAction( QIcon( ":/images/refresh.png" ), tr( "&Reload window" ), this );
	reloadAction->setShortcuts( QKeySequence::Refresh );
	reloadAction->setStatusTip( tr( "Reload the active window" ) );
	connect( reloadAction, SIGNAL( triggered() ), m_mdiArea, SLOT( closeActiveSubWindow() ));

	closeAction = new QAction( tr( "Cl&ose" ), this );
	closeAction->setStatusTip( tr( "Close the active window" ) );
	connect( closeAction, SIGNAL( triggered() ), m_mdiArea, SLOT( closeActiveSubWindow() ));

	closeAllAction = new QAction( tr( "Close &All" ), this );
	closeAllAction->setStatusTip( tr( "Close all the windows" ) );
	connect( closeAllAction, SIGNAL( triggered() ), m_mdiArea, SLOT( closeAllSubWindows() ));

	tileAction = new QAction( tr( "&Tile" ), this );
	tileAction->setStatusTip( tr( "Tile the windows" ));
	connect( tileAction, SIGNAL( triggered() ), m_mdiArea, SLOT( tileSubWindows() ));

	cascadeAction = new QAction( tr( "&Cascade" ), this );
	cascadeAction->setStatusTip( tr( "Cascade the windows" ) );
	connect( cascadeAction, SIGNAL( triggered() ), m_mdiArea, SLOT( cascadeSubWindows() ));

	nextAction = new QAction( QIcon( ":/images/next.png" ), tr( "Ne&xt" ), this );
	nextAction->setShortcuts( QKeySequence::NextChild );
	nextAction->setStatusTip( tr( "Move the focus to the next window" ) );
	connect( nextAction, SIGNAL( triggered() ), m_mdiArea, SLOT( activateNextSubWindow() ));

	previousAction = new QAction( QIcon( ":/images/previous.png" ), tr( "Pre&vious"), this );
	previousAction->setShortcuts( QKeySequence::PreviousChild );
	previousAction->setStatusTip( tr( "Move the focus to the previous window" ) );
	connect( previousAction, SIGNAL( triggered() ), m_mdiArea, SLOT( activatePreviousSubWindow() ));

	wndMenuSeparatorAction = new QAction( this );
	wndMenuSeparatorAction->setSeparator( true );

	// Development menu actions
	manageInterfaceAction = new QAction( tr( "Manage &interface(s)..." ), this );
	manageInterfaceAction->setStatusTip( tr( "Manage application interface(s)" ) );
	connect( manageInterfaceAction, SIGNAL( triggered() ), this, SLOT( manageInterface() ));

	// Help menu actions
	helpContentsAction = new QAction( tr( "Contents" ), this );
	helpContentsAction->setStatusTip( tr( "Help table of contents" ) );
	connect( helpContentsAction, SIGNAL( triggered() ), this, SLOT( helpContents() ));

	helpIndexAction = new QAction( tr( "Index" ), this );
	helpIndexAction->setStatusTip( tr( "Help index") );
	connect( helpIndexAction, SIGNAL( triggered() ), this, SLOT( helpIndex() ));

	contextHelpAction = new QAction( tr( "Context Help" ), this );
	contextHelpAction->setShortcuts( QKeySequence::HelpContents );
	contextHelpAction->setStatusTip( tr( "Show context sensitive help" ) );
	connect( contextHelpAction, SIGNAL( triggered() ), this, SLOT( contextHelp() ));

	technicalSupportAction = new QAction( tr( "Technical Support" ), this );
	technicalSupportAction->setStatusTip( tr( "Show technical support resources" ) );
	connect( technicalSupportAction, SIGNAL( triggered() ), this, SLOT( technicalSupport() ));

	reportBugAction = new QAction( tr( "Report Bug..." ), this );
	reportBugAction->setStatusTip( tr( "Report a bug about the application" ) );
	connect( reportBugAction, SIGNAL( triggered() ), this, SLOT( reportBug() ));

	checkUpdtAction = new QAction( tr( "Check for Updates..." ), this );
	checkUpdtAction->setStatusTip( tr( "Check for application updates" ) );
	connect( checkUpdtAction, SIGNAL( triggered() ), this, SLOT( checkUpdates() ));

	aboutAction = new QAction( tr( "&About Wolframe Client" ), this );
	aboutAction->setStatusTip( tr( "Show the application's About box" ) );
	connect( aboutAction, SIGNAL( triggered() ), this, SLOT( about() ));

	aboutQtAction = new QAction( tr( "About &Qt" ), this );
	aboutQtAction->setStatusTip( tr( "Show the Qt library's About box" ) );
	connect( aboutQtAction, SIGNAL( triggered() ), qApp, SLOT( aboutQt() ));
}

void MainWindow::createMenus()
{
	// Connect menu
	connectMenu = menuBar()->addMenu( tr( "&Connection" ) );
	connectMenu->addAction( loginAction );
	connectMenu->addAction( logoutAction );
	connectMenu->addSeparator();
	connectMenu->addAction( printAction );
	connectMenu->addSeparator();
	connectMenu->addAction( manageServersAction );
	connectMenu->addAction( preferencesAction );
	connectMenu->addSeparator();
	connectMenu->addAction( exitAction );

	// Edit menu
	editMenu = menuBar()->addMenu( tr( "&Edit" ) );
	editMenu->addAction( undoAction );
	editMenu->addAction( redoAction );
	editMenu->addSeparator();
	editMenu->addAction( cutAction );
	editMenu->addAction( copyAction );
	editMenu->addAction( pasteAction );
	editMenu->addAction( delAction );
	editMenu->addSeparator();
	editMenu->addAction( selectAllAction );

	// Window menu
	windowMenu = menuBar()->addMenu( tr( "&Window" ) );
	updateWindowMenu();
	connect( windowMenu, SIGNAL( aboutToShow() ), this, SLOT( updateWindowMenu() ));

	// Help menu
	menuBar()->addSeparator();
	helpMenu = menuBar()->addMenu( tr( "&Help" ) );
	helpMenu->addAction( helpContentsAction );
	helpMenu->addAction( helpIndexAction );
	helpMenu->addAction( contextHelpAction );
	helpMenu->addSeparator();
	helpMenu->addAction( technicalSupportAction );
	helpMenu->addAction( reportBugAction );
	helpMenu->addSeparator();
	helpMenu->addAction( checkUpdtAction );
	helpMenu->addSeparator();
	helpMenu->addAction( aboutAction );
	helpMenu->addAction( aboutQtAction );
}

void MainWindow::createToolBars()
{
	loginToolBar = addToolBar( tr( "Login" ));
	loginToolBar->addAction( loginAction );
	loginToolBar->addAction( logoutAction );

	editToolBar = addToolBar( tr( "Edit" ) );
	editToolBar->addAction( undoAction );
	editToolBar->addAction( redoAction );
	editToolBar->addAction( cutAction );
	editToolBar->addAction( copyAction );
	editToolBar->addAction( pasteAction );
	editToolBar->addAction( delAction );

	windowToolBar = addToolBar( tr( "Window" ) );
	windowToolBar->addAction( reloadAction );
	windowToolBar->addSeparator();
	windowToolBar->addAction( nextAction );
	windowToolBar->addAction( previousAction );
}

void MainWindow::createStatusBar()
{
	statusBar()->showMessage( tr( "Ready" ));

	m_statusBarConn = new QLabel( this );
	m_statusBarConn->setFrameStyle( QFrame::Panel | QFrame::Sunken );
	statusBar()->addPermanentWidget( m_statusBarConn );
	m_statusBarConn->setPixmap( QPixmap( ":/images/16x16/disconnected.png" ));
	m_statusBarConn->setToolTip( tr( "Status: offline" ));
	m_statusBarConn->setEnabled( true );

	m_statusBarSSL = new QLabel( this );
	m_statusBarSSL->setFrameStyle( QFrame::Panel | QFrame::Sunken );
	statusBar()->addPermanentWidget( m_statusBarSSL );
	m_statusBarSSL->setPixmap( QPixmap( ":/images/16x16/unencrypted.png" ));
	m_statusBarSSL->setToolTip( tr( "Encryption: N/A" ));
	m_statusBarSSL->setEnabled( false );
}

void MainWindow::updateStatusBar()
{
//	if ( !m_conn.isEmpty() )	{
//		m_statusBarConn->setPixmap( QPixmap( ":/images/16x16/connected.png" ));
//		m_statusBarConn->setToolTip( tr( "Status: connected" ));

//		m_statusBarSSL->setPixmap( QPixmap( ":/images/16x16/encrypted.png" ));
//		m_statusBarSSL->setToolTip( tr( "Encryption: none" ));
//		m_statusBarSSL->setEnabled( true );
//	}
//	else	{
//		m_statusBarConn->setPixmap( QPixmap( ":/images/16x16/disconnected.png" ));
//		m_statusBarConn->setToolTip( tr( "Status: offline" ));

//		m_statusBarSSL->setPixmap( QPixmap( ":/images/16x16/unencrypted.png" ));
//		m_statusBarSSL->setToolTip( tr( "Encryption: N/A" ));
//		m_statusBarSSL->setEnabled( false );
//	}
}

MdiChild *MainWindow::activeMdiChild()
{
    if (QMdiSubWindow *activeSubWindow = m_mdiArea->activeSubWindow())
	return qobject_cast< MdiChild* >(activeSubWindow->widget());
    return 0;
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QMdiSubWindow *window, m_mdiArea->subWindowList()) {
	MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());
	if (mdiChild->currentFile() == canonicalFilePath)
	    return window;
    }
    return 0;
}

void MainWindow::setActiveSubWindow( QWidget *window )
{
	if ( !window )
		return;
	m_mdiArea->setActiveSubWindow( qobject_cast< QMdiSubWindow* >( window ) );
}
