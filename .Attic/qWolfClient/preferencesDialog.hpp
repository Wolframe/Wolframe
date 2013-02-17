#ifndef _PREFERENCES_DIALOG_HPP_INCLUDED
#define _PREFERENCES_DIALOG_HPP_INCLUDED

#include <QDialog>

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PreferencesDialog(QWidget *parent = 0);
	~PreferencesDialog();

private:
	Ui::PreferencesDialog *ui;
};

#endif // _PREFERENCES_DIALOG_HPP_INCLUDED
