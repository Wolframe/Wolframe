//
// PreferencesDialog.hpp
//

#ifndef _PREFERENCES_DIALOG_HPP_INCLUDED
#define _PREFERENCES_DIALOG_HPP_INCLUDED

#include <QDialog>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>

#include "FileChooser.hpp"

namespace _Wolframe {
	namespace QtClient {

	class PreferencesDialog : public QDialog
	{
	Q_OBJECT
		
	public:
		PreferencesDialog( QWidget *_parent = 0 );
	
	private:
		QLineEdit *m_host;
		QSpinBox *m_port;
		QCheckBox *m_secure;
		FileChooser *m_clientCertFile;
		FileChooser *m_clientKeyFile;
		FileChooser *m_CACertFile;
		QDialogButtonBox* m_buttons;
		
	private:
		void initialize( );
		void loadSettings( );
		
	private slots:
		void toggleSecure( int state );
		void apply( );
		void cancel( );
	};

} // namespace QtClient
} // namespace _Wolframe

#endif // _PREFERENCES_DIALOG_HPP_INCLUDED
