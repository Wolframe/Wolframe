//
// LoginDialog.hpp
//

#ifndef _LOGIN_DIALOG_HPP_INCLUDED
#define _LOGIN_DIALOG_HPP_INCLUDED

#include <QDialog>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QStringList>
#include <QKeyEvent>

#include "WolframeClient.hpp"

namespace _Wolframe {
	namespace QtClient {

	class LoginDialog : public QDialog
	{
	Q_OBJECT
		
	public:
		LoginDialog( WolframeClient *_wolframeClient, QWidget *_parent = 0 );
	
	private:
		QLineEdit *m_host;
		QSpinBox *m_port;
		QPushButton *m_prefs;
		QComboBox *m_mechs;
		QComboBox *m_username;
		QLineEdit *m_password;
		QDialogButtonBox* m_buttons;
		WolframeClient *m_wolframeClient;
		QStringList m_mechList;

	signals:
		void authenticationOk( );
		void authenticationFailed( );
		
	private:
		void initialize( );

	protected:
		void keyPressEvent( QKeyEvent *e );
		
	private slots:
		void mechsReceived( QStringList mechs );
		void login( );
		void authOk( );
		void authFailed( );
		void showPreferences( );
	};
} // namespace QtClient
} // namespace _Wolframe

#endif // _LOGIN_DIALOG_HPP_INCLUDED
