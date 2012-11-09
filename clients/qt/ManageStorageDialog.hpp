//
// ManageStorageDialog.hpp
//

#ifndef _MANAGE_STORAGE_DIALOG_HPP_INCLUDED
#define _MANAGE_STORAGE_DIALOG_HPP_INCLUDED

#include <QDialog>
#include <QPushButton>
#include <QComboBox>

#include "FileChooser.hpp"
#include "StorageWidget.hpp"

namespace _Wolframe {
	namespace QtClient {

	class ManageStorageDialog : public QDialog
	{
	Q_OBJECT
		
	public:
		ManageStorageDialog( QWidget *_parent = 0 );
	
	private:
		StorageWidget *m_leftStorage;
		QPushButton *m_left;
		QPushButton *m_right;
		QComboBox *m_syncMode;
		QPushButton *m_synchronize;
		QPushButton *m_delete;
		StorageWidget *m_rightStorage;
		QPushButton *m_exit;
			
	private:
		void initialize( );
		void loadSettings( );
		
	private slots:
		void terminate( );

	};

} // namespace QtClient
} // namespace _Wolframe

#endif // _MANAGE_STORAGE_DIALOG_HPP_INCLUDED
