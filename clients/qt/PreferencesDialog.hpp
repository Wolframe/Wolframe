//
// PreferencesDialog.hpp
//

#ifndef _PREFERENCES_DIALOG_HPP_INCLUDED
#define _PREFERENCES_DIALOG_HPP_INCLUDED

#include <QDialog>

#include "settings.hpp"

#include "ui_PreferencesDialog.h"
#include "ui_PreferencesDialogDeveloper.h"
#include "ui_PreferencesDialogInterface.h"

class PreferencesDialogInterface : public QWidget, public Ui::PreferencesDialogInterface
{
	Q_OBJECT
	
	public:
		PreferencesDialogInterface( QWidget *_parent = 0 );
};

class PreferencesDialogDeveloper : public QWidget, public Ui::PreferencesDialogDeveloper
{
	Q_OBJECT
	
	public:
		PreferencesDialogDeveloper( QWidget *_parent = 0 );
};

class PreferencesDialog : public QDialog, public Ui::PreferencesDialog
{
	Q_OBJECT
		
	public:
		PreferencesDialog( ApplicationSettings &settings, QStringList _languages, QWidget *_parent = 0 );
	
	private:
		ApplicationSettings &m_settings;
		QStringList m_languages;
		PreferencesDialogInterface *m_interface;
		PreferencesDialogDeveloper *m_developer;
		
	private:
		void initialize( );
		void loadSettings( );
		
	private slots:
		void toggleLoadMode( bool checked );
		void toggleLocale( bool checked );
		void restoreDefaults( );
		void apply( );
		void cancel( );
};


#endif // _PREFERENCES_DIALOG_HPP_INCLUDED
