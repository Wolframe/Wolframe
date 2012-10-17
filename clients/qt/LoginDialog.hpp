//
// LoginDialog.hpp
//

#ifndef _LOGIN_DIALOG_HPP_INCLUDED
#define _LOGIN_DIALOG_HPP_INCLUDED

#include <QDialog>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QComboBox>
#include <QGridLayout>
#include <QStringList>
#include <QCloseEvent>
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
		QComboBox *m_username;
		QLineEdit *m_password;
		QDialogButtonBox* m_buttons;
		WolframeClient *m_wolframeClient;
		bool m_succeeded;

	signals:
		void authenticationOk( );
		void authenticationFailed( );
		
	private:
		void initialize( );

	protected:
		void closeEvent( QCloseEvent *event );
		void keyPressEvent( QKeyEvent *e );
		
	private slots:
		void login( );

	};
} // namespace QtClient
} // namespace _Wolframe

#endif // _LOGIN_DIALOG_HPP_INCLUDED
