#ifndef _MANAGE_SERVERS_DIALOG_HPP_INCLUDED
#define _MANAGE_SERVERS_DIALOG_HPP_INCLUDED

#include <QDialog>
#include "connection.hpp"

namespace Ui {
class ManageServersDialog;
}

class ManageServersDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ManageServersDialog( QVector< ConnectionParameters >& params, QWidget *parent = 0 );
	~ManageServersDialog();

private slots:
	void done( int retCode );

	void newConnection();
	void editConnection();
	void delConnection();

	void updateButtons();
	void connectionBrief() const;

private:
	Ui::ManageServersDialog			*ui;
	QVector< ConnectionParameters >		m_localParams;
	QVector< ConnectionParameters >&	m_globalParams;
};

#endif // _MANAGE_SERVERS_DIALOG_HPP_INCLUDED
