#ifndef _SERVER_DEFINITION_DIALOG_HPP_INCLUDED
#define _SERVER_DEFINITION_DIALOG_HPP_INCLUDED

#include <QDialog>
#include <QAbstractButton>

#include "connection.hpp"
#include "WolframeClient.hpp"

namespace Ui {
class ServerDefinitionDialog;
}

class ServerDefinitionDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ServerDefinitionDialog( ConnectionParameters& params, QWidget *_parent = 0 );
	~ServerDefinitionDialog();

private slots:
	void done( int retCode );	// overwrite the done method in order
					// to validate the data
	void selectCertFile();
	void selectKeyFile();
	void selectCAbundleFile();

	void SSLtoggle();
	void updateSSLfields();
	void updateButtons();

	void testConnection();
	void error( QString error );
	void connected( );
	void disconnected( );

private:
	void buildParams( ConnectionParameters& params );

private:
	Ui::ServerDefinitionDialog	*ui;
	ConnectionParameters&		m_params;
	QString				m_currentDir;
	WolframeClient *m_client;

};

#endif // _SERVER_DEFINITION_DIALOG_HPP_INCLUDED
