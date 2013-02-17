#include "preferencesDialog.hpp"
#include "ui_preferencesDialog.h"

PreferencesDialog::PreferencesDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PreferencesDialog)
{
	ui->setupUi(this);
}

PreferencesDialog::~PreferencesDialog()
{
	delete ui;
}
