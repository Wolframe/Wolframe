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
		
	private:
		void initialize( );

	protected:

	private slots:
		void login( );

	};
} // namespace QtClient
} // namespace _Wolframe

#endif // _LOGIN_DIALOG_HPP_INCLUDED
