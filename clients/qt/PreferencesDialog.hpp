//
// PreferencesDialog.hpp
//

#ifndef _PREFERENCES_DIALOG_HPP_INCLUDED
#define _PREFERENCES_DIALOG_HPP_INCLUDED

#include <QDialog>
#include <QStringList>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QComboBox>

#include "FileChooser.hpp"

namespace _Wolframe {
	namespace QtClient {

	class PreferencesDialog : public QDialog
	{
	Q_OBJECT
		
	public:
		PreferencesDialog( QWidget *_parent = 0 );
		PreferencesDialog( QStringList _languages, QWidget *_parent = 0 );
	
	private:
		QRadioButton *m_loadModeLocalFile;
		QRadioButton *m_loadModeLocalDatabase;
		QRadioButton *m_loadModeNetwork;
		QLineEdit *m_host;
		QSpinBox *m_port;
		QCheckBox *m_secure;
		FileChooser *m_clientCertFile;
		FileChooser *m_clientKeyFile;
		FileChooser *m_CACertFile;
		QDialogButtonBox* m_buttons;
		FileChooser *m_dbName;
		FileChooser *m_uiFormsDir;
		FileChooser *m_uiFormTranslationsDir;
		FileChooser *m_dataLoaderDir;
		QCheckBox *m_debug;
		QComboBox *m_locale;
		QCheckBox *m_systemLanguage;
		QStringList m_languages;
	
	signals:
		void prefsChanged( );
		
	private:
		void initialize( );
		void loadSettings( );
		
	private slots:
		void toggleSecure( int state );
		void toggleLoadMode( bool checked );
		void toggleSystemLanguage( int state );
		void apply( );
		void cancel( );
	};

} // namespace QtClient
} // namespace _Wolframe

#endif // _PREFERENCES_DIALOG_HPP_INCLUDED
