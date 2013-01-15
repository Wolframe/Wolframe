//
// FormChooseDialog.hpp
//

#ifndef _FORM_CHOOSE_DIALOG_HPP_INCLUDED
#define _FORM_CHOOSE_DIALOG_HPP_INCLUDED

#include <QDialog>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QStringList>
#include <QKeyEvent>

class FormChooseDialog : public QDialog
{
	Q_OBJECT
		
	public:
		FormChooseDialog( const QStringList _formNames, QWidget *_parent = 0 );
	
	private:
		QComboBox *m_forms;
		QDialogButtonBox* m_buttons;
		QStringList m_formNames;
		QString m_form;
		
	public:
		QString form( ) { return m_form; }
		
	private:
		void initialize( );
	
	private slots:
		void ok( );
		void cancel( );

	protected:
		void keyPressEvent( QKeyEvent *e );
};

#endif // _FORM_CHOOSE_DIALOG_HPP_INCLUDED
