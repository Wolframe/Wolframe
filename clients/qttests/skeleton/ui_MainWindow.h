/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created: Tue Apr 9 07:41:50 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMdiArea>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpen;
    QAction *actionExit;
    QAction *actionAbout;
    QAction *actionAboutQt;
    QAction *actionDebugTerminal;
    QAction *actionPreferences;
    QAction *actionManageStorage;
    QAction *actionRestart;
    QAction *actionOpenForm;
    QAction *actionOpenFormNewWindow;
    QAction *actionContents;
    QAction *actionIndex;
    QAction *actionContextHelp;
    QAction *actionTechnicalSupport;
    QAction *actionReportBug;
    QAction *actionCheckForUpdates;
    QAction *actionReload;
    QAction *actionClose;
    QAction *actionCloseAll;
    QAction *actionTile;
    QAction *actionCascade;
    QAction *actionNextWindow;
    QAction *actionPreviousWindow;
    QAction *actionUndo;
    QAction *actionRedo;
    QAction *actionCut;
    QAction *actionPaste;
    QAction *actionDelete;
    QAction *actionSelectAll;
    QAction *actionCopy;
    QAction *actionLogin;
    QAction *actionLogout;
    QAction *actionPrint;
    QAction *actionManageServers;
    QAction *actionTest;
    QAction *action_Close_form;
    QMdiArea *centralWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QStatusBar *statusbar;
    QToolBar *toolBarConnection;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->setEnabled(true);
        MainWindow->resize(607, 395);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/wolframe.png"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        MainWindow->setDocumentMode(true);
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8("../../../../../../../../../../home/abaumann/.designer/backup/open.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpen->setIcon(icon1);
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/images/exit.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionExit->setIcon(icon2);
        actionExit->setMenuRole(QAction::QuitRole);
        actionExit->setProperty("defaultShortcut", QVariant(QString::fromUtf8("QKeySequence::Quit")));
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        actionAbout->setMenuRole(QAction::AboutRole);
        actionAboutQt = new QAction(MainWindow);
        actionAboutQt->setObjectName(QString::fromUtf8("actionAboutQt"));
        actionAboutQt->setMenuRole(QAction::AboutQtRole);
        actionDebugTerminal = new QAction(MainWindow);
        actionDebugTerminal->setObjectName(QString::fromUtf8("actionDebugTerminal"));
        actionDebugTerminal->setCheckable(true);
        actionPreferences = new QAction(MainWindow);
        actionPreferences->setObjectName(QString::fromUtf8("actionPreferences"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/images/configure.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionPreferences->setIcon(icon3);
        actionPreferences->setMenuRole(QAction::PreferencesRole);
        actionPreferences->setProperty("defaultShortcut", QVariant(QString::fromUtf8("QKeySequence::Preferences")));
        actionManageStorage = new QAction(MainWindow);
        actionManageStorage->setObjectName(QString::fromUtf8("actionManageStorage"));
        actionRestart = new QAction(MainWindow);
        actionRestart->setObjectName(QString::fromUtf8("actionRestart"));
        actionOpenForm = new QAction(MainWindow);
        actionOpenForm->setObjectName(QString::fromUtf8("actionOpenForm"));
        actionOpenForm->setEnabled(false);
        actionOpenFormNewWindow = new QAction(MainWindow);
        actionOpenFormNewWindow->setObjectName(QString::fromUtf8("actionOpenFormNewWindow"));
        actionOpenFormNewWindow->setEnabled(false);
        actionContents = new QAction(MainWindow);
        actionContents->setObjectName(QString::fromUtf8("actionContents"));
        actionContents->setEnabled(false);
        actionContents->setVisible(true);
        actionContents->setProperty("defaultShortcut", QVariant(QString::fromUtf8("QKeySequence::HelpContents")));
        actionIndex = new QAction(MainWindow);
        actionIndex->setObjectName(QString::fromUtf8("actionIndex"));
        actionIndex->setEnabled(false);
        actionContextHelp = new QAction(MainWindow);
        actionContextHelp->setObjectName(QString::fromUtf8("actionContextHelp"));
        actionContextHelp->setEnabled(false);
        actionTechnicalSupport = new QAction(MainWindow);
        actionTechnicalSupport->setObjectName(QString::fromUtf8("actionTechnicalSupport"));
        actionTechnicalSupport->setEnabled(false);
        actionReportBug = new QAction(MainWindow);
        actionReportBug->setObjectName(QString::fromUtf8("actionReportBug"));
        actionReportBug->setEnabled(false);
        actionCheckForUpdates = new QAction(MainWindow);
        actionCheckForUpdates->setObjectName(QString::fromUtf8("actionCheckForUpdates"));
        actionCheckForUpdates->setEnabled(false);
        actionReload = new QAction(MainWindow);
        actionReload->setObjectName(QString::fromUtf8("actionReload"));
        actionReload->setEnabled(false);
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/images/refresh.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionReload->setIcon(icon4);
        actionReload->setVisible(true);
        actionReload->setProperty("defaultShortcut", QVariant(QString::fromUtf8("QKeySequence::Refresh")));
        actionClose = new QAction(MainWindow);
        actionClose->setObjectName(QString::fromUtf8("actionClose"));
        actionClose->setEnabled(false);
        actionClose->setShortcutContext(Qt::WidgetShortcut);
        actionClose->setProperty("defaultShortcut", QVariant(QString::fromUtf8("QKeySequence::Close")));
        actionCloseAll = new QAction(MainWindow);
        actionCloseAll->setObjectName(QString::fromUtf8("actionCloseAll"));
        actionCloseAll->setEnabled(false);
        actionTile = new QAction(MainWindow);
        actionTile->setObjectName(QString::fromUtf8("actionTile"));
        actionTile->setEnabled(false);
        actionCascade = new QAction(MainWindow);
        actionCascade->setObjectName(QString::fromUtf8("actionCascade"));
        actionCascade->setEnabled(false);
        actionNextWindow = new QAction(MainWindow);
        actionNextWindow->setObjectName(QString::fromUtf8("actionNextWindow"));
        actionNextWindow->setEnabled(false);
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/images/next.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionNextWindow->setIcon(icon5);
        actionNextWindow->setProperty("defaultShortcut", QVariant(QString::fromUtf8("QKeySequence::NextChild")));
        actionPreviousWindow = new QAction(MainWindow);
        actionPreviousWindow->setObjectName(QString::fromUtf8("actionPreviousWindow"));
        actionPreviousWindow->setEnabled(false);
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/images/previous.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionPreviousWindow->setIcon(icon6);
        actionPreviousWindow->setProperty("defaultShortcut", QVariant(QString::fromUtf8("QKeySequence::PreviousChild")));
        actionUndo = new QAction(MainWindow);
        actionUndo->setObjectName(QString::fromUtf8("actionUndo"));
        actionUndo->setEnabled(false);
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/images/undo.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionUndo->setIcon(icon7);
        actionUndo->setProperty("defaultShortcut", QVariant(QString::fromUtf8("QKeySequence::Undo")));
        actionRedo = new QAction(MainWindow);
        actionRedo->setObjectName(QString::fromUtf8("actionRedo"));
        actionRedo->setEnabled(false);
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/images/redo.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionRedo->setIcon(icon8);
        actionRedo->setProperty("defaultShortcut", QVariant(QString::fromUtf8("QKeySequence::Redo")));
        actionCut = new QAction(MainWindow);
        actionCut->setObjectName(QString::fromUtf8("actionCut"));
        actionCut->setEnabled(false);
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/images/cut.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionCut->setIcon(icon9);
        actionCut->setProperty("defaultShortcut", QVariant(QString::fromUtf8("QKeySequence::Cut")));
        actionPaste = new QAction(MainWindow);
        actionPaste->setObjectName(QString::fromUtf8("actionPaste"));
        actionPaste->setEnabled(false);
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/images/paste.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionPaste->setIcon(icon10);
        actionPaste->setProperty("defaultShortcut", QVariant(QString::fromUtf8("QKeySequence::Paste")));
        actionDelete = new QAction(MainWindow);
        actionDelete->setObjectName(QString::fromUtf8("actionDelete"));
        actionDelete->setEnabled(false);
        QIcon icon11;
        icon11.addFile(QString::fromUtf8(":/images/delete.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDelete->setIcon(icon11);
        actionDelete->setProperty("defaultShortcut", QVariant(QString::fromUtf8("QKeySequence::Delete")));
        actionSelectAll = new QAction(MainWindow);
        actionSelectAll->setObjectName(QString::fromUtf8("actionSelectAll"));
        actionSelectAll->setEnabled(false);
        actionSelectAll->setProperty("defaultShortcut", QVariant(QString::fromUtf8("QKeySequence::SelectAll")));
        actionCopy = new QAction(MainWindow);
        actionCopy->setObjectName(QString::fromUtf8("actionCopy"));
        actionCopy->setEnabled(false);
        QIcon icon12;
        icon12.addFile(QString::fromUtf8(":/images/copy.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionCopy->setIcon(icon12);
        actionCopy->setProperty("defaultShortcut", QVariant(QString::fromUtf8("QKeySequence::Copy")));
        actionLogin = new QAction(MainWindow);
        actionLogin->setObjectName(QString::fromUtf8("actionLogin"));
        actionLogin->setEnabled(false);
        QIcon icon13;
        icon13.addFile(QString::fromUtf8(":/images/login.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionLogin->setIcon(icon13);
        actionLogout = new QAction(MainWindow);
        actionLogout->setObjectName(QString::fromUtf8("actionLogout"));
        actionLogout->setEnabled(false);
        QIcon icon14;
        icon14.addFile(QString::fromUtf8(":/images/logout.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionLogout->setIcon(icon14);
        actionPrint = new QAction(MainWindow);
        actionPrint->setObjectName(QString::fromUtf8("actionPrint"));
        actionPrint->setEnabled(false);
        QIcon icon15;
        icon15.addFile(QString::fromUtf8(":/images/print.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionPrint->setIcon(icon15);
        actionPrint->setProperty("defaultShortcut", QVariant(QString::fromUtf8("QKeySequence::Print")));
        actionManageServers = new QAction(MainWindow);
        actionManageServers->setObjectName(QString::fromUtf8("actionManageServers"));
        actionTest = new QAction(MainWindow);
        actionTest->setObjectName(QString::fromUtf8("actionTest"));
        action_Close_form = new QAction(MainWindow);
        action_Close_form->setObjectName(QString::fromUtf8("action_Close_form"));
        centralWidget = new QMdiArea(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        centralWidget->setViewMode(QMdiArea::SubWindowView);
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 607, 19));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        MainWindow->setMenuBar(menuBar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);
        toolBarConnection = new QToolBar(MainWindow);
        toolBarConnection->setObjectName(QString::fromUtf8("toolBarConnection"));
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBarConnection);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionLogin);
        menuFile->addAction(actionLogout);
        menuFile->addAction(actionManageServers);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        toolBarConnection->addAction(actionLogin);
        toolBarConnection->addAction(actionLogout);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Wolframe Qt Skeleton", 0, QApplication::UnicodeUTF8));
        MainWindow->setProperty("themeAuthor", QVariant(QApplication::translate("MainWindow", "Andreas Baumann", 0, QApplication::UnicodeUTF8)));
        MainWindow->setProperty("themeBaseStyle", QVariant(QApplication::translate("MainWindow", "plastique", 0, QApplication::UnicodeUTF8)));
        actionOpen->setText(QApplication::translate("MainWindow", "&Open", 0, QApplication::UnicodeUTF8));
        actionOpen->setShortcut(QApplication::translate("MainWindow", "Ctrl+O", 0, QApplication::UnicodeUTF8));
        actionExit->setText(QApplication::translate("MainWindow", "&Exit", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionExit->setToolTip(QApplication::translate("MainWindow", "Exit the Wolframe client", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionExit->setShortcut(QApplication::translate("MainWindow", "Ctrl+Q", 0, QApplication::UnicodeUTF8));
        actionAbout->setText(QApplication::translate("MainWindow", "&About Wolframe Client", 0, QApplication::UnicodeUTF8));
        actionAboutQt->setText(QApplication::translate("MainWindow", "About &Qt", 0, QApplication::UnicodeUTF8));
        actionDebugTerminal->setText(QApplication::translate("MainWindow", "&Debug Terminal", 0, QApplication::UnicodeUTF8));
        actionDebugTerminal->setIconText(QApplication::translate("MainWindow", "Debug Terminal", 0, QApplication::UnicodeUTF8));
        actionDebugTerminal->setShortcut(QApplication::translate("MainWindow", "Ctrl+D", 0, QApplication::UnicodeUTF8));
        actionPreferences->setText(QApplication::translate("MainWindow", "&Preferences", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionPreferences->setToolTip(QApplication::translate("MainWindow", "Configure application preferences", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionManageStorage->setText(QApplication::translate("MainWindow", "&Manage Storage", 0, QApplication::UnicodeUTF8));
        actionRestart->setText(QApplication::translate("MainWindow", "&Restart", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionRestart->setToolTip(QApplication::translate("MainWindow", "Restart the Wolframe client", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionOpenForm->setText(QApplication::translate("MainWindow", "&Open form..", 0, QApplication::UnicodeUTF8));
        actionOpenForm->setProperty("defaultShortcut", QVariant(QApplication::translate("MainWindow", "QKeySequence::Open", 0, QApplication::UnicodeUTF8)));
        actionOpenFormNewWindow->setText(QApplication::translate("MainWindow", "Open form in &new window..", 0, QApplication::UnicodeUTF8));
        actionOpenFormNewWindow->setProperty("defaultShortcut", QVariant(QApplication::translate("MainWindow", "QKeySequence::New", 0, QApplication::UnicodeUTF8)));
        actionContents->setText(QApplication::translate("MainWindow", "Contents", 0, QApplication::UnicodeUTF8));
        actionIndex->setText(QApplication::translate("MainWindow", "Index", 0, QApplication::UnicodeUTF8));
        actionContextHelp->setText(QApplication::translate("MainWindow", "Context Help", 0, QApplication::UnicodeUTF8));
        actionTechnicalSupport->setText(QApplication::translate("MainWindow", "Technical Support", 0, QApplication::UnicodeUTF8));
        actionReportBug->setText(QApplication::translate("MainWindow", "Report Bug...", 0, QApplication::UnicodeUTF8));
        actionCheckForUpdates->setText(QApplication::translate("MainWindow", "Check for Updates...", 0, QApplication::UnicodeUTF8));
        actionReload->setText(QApplication::translate("MainWindow", "&Reload Window", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionReload->setToolTip(QApplication::translate("MainWindow", "Reload the active window", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionClose->setText(QApplication::translate("MainWindow", "&Close", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionClose->setToolTip(QApplication::translate("MainWindow", "Close the active window", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionCloseAll->setText(QApplication::translate("MainWindow", "Close &all", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionCloseAll->setToolTip(QApplication::translate("MainWindow", "Close all the windows", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionTile->setText(QApplication::translate("MainWindow", "&Tile", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionTile->setToolTip(QApplication::translate("MainWindow", "Tile the windows", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionCascade->setText(QApplication::translate("MainWindow", "&Cascade", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionCascade->setToolTip(QApplication::translate("MainWindow", "Cascade the windows", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionNextWindow->setText(QApplication::translate("MainWindow", "Ne&xt", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionNextWindow->setToolTip(QApplication::translate("MainWindow", "Move the focus to the next window", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionPreviousWindow->setText(QApplication::translate("MainWindow", "Pre&vious", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionPreviousWindow->setToolTip(QApplication::translate("MainWindow", "Move the focus to the previous window", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionUndo->setText(QApplication::translate("MainWindow", "&Undo", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionUndo->setToolTip(QApplication::translate("MainWindow", "Undo the last action", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionRedo->setText(QApplication::translate("MainWindow", "&Redo", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionRedo->setToolTip(QApplication::translate("MainWindow", "Redo the last undo", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionCut->setText(QApplication::translate("MainWindow", "Cu&t", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionCut->setToolTip(QApplication::translate("MainWindow", "Cut the current selection's contents to the clipboard", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionPaste->setText(QApplication::translate("MainWindow", "&Paste", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionPaste->setToolTip(QApplication::translate("MainWindow", "Paste the clipboard's contents into the current selection", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionDelete->setText(QApplication::translate("MainWindow", "Delete", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionDelete->setToolTip(QApplication::translate("MainWindow", "Delete the current selection's contents", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionSelectAll->setText(QApplication::translate("MainWindow", "Select &All", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionSelectAll->setToolTip(QApplication::translate("MainWindow", "Select all items in the current window", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionCopy->setText(QApplication::translate("MainWindow", "&Copy", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionCopy->setToolTip(QApplication::translate("MainWindow", "Copy the current selection's contents to the clipboard", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionLogin->setText(QApplication::translate("MainWindow", "&Login", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionLogin->setToolTip(QApplication::translate("MainWindow", "Login to a Wolframe server", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionLogout->setText(QApplication::translate("MainWindow", "L&ogout", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionLogout->setToolTip(QApplication::translate("MainWindow", "Logout from the Wolframe server", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionPrint->setText(QApplication::translate("MainWindow", "&Print", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionPrint->setToolTip(QApplication::translate("MainWindow", "Print current document", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionManageServers->setText(QApplication::translate("MainWindow", "&Manage servers...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionManageServers->setToolTip(QApplication::translate("MainWindow", "Manage the list of Wolframe servers", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionTest->setText(QApplication::translate("MainWindow", "&English", 0, QApplication::UnicodeUTF8));
        actionTest->setIconText(QApplication::translate("MainWindow", "English", 0, QApplication::UnicodeUTF8));
        action_Close_form->setText(QApplication::translate("MainWindow", "&Close form", 0, QApplication::UnicodeUTF8));
        action_Close_form->setProperty("defaultShortcut", QVariant(QApplication::translate("MainWindow", "QKeySequence::Close", 0, QApplication::UnicodeUTF8)));
        menuFile->setTitle(QApplication::translate("MainWindow", "&File", 0, QApplication::UnicodeUTF8));
        toolBarConnection->setWindowTitle(QApplication::translate("MainWindow", "toolBar_2", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
