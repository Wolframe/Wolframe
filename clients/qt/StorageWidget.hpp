//
// StorageWidget.hpp
//

#ifndef _STORAGEWIDGET_HPP_INCLUDED
#define _STORAGEWIDGET_HPP_INCLUDED

#include <QObject>
#include <QWidget>
#include <QString>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QTreeWidget>

#include "FileChooser.hpp"

namespace _Wolframe {
	namespace QtClient {

	class StorageWidget : public QWidget
	{
	Q_OBJECT
			
	public:
		StorageWidget( QWidget *_parent = 0 );
	
	private:
		void initialize( );
		void loadSettings( );
		void loadTree( );
				
	private:
		QRadioButton *m_loadModeLocalFile;
		QRadioButton *m_loadModeLocalDatabase;
		QRadioButton *m_loadModeNetwork;
		QLineEdit *m_host;
		QSpinBox *m_port;
		QCheckBox *m_secure;
		FileChooser *m_clientCertFile;
		FileChooser *m_clientKeyFile;
		FileChooser *m_CACertFile;
		FileChooser *m_dbName;
		FileChooser *m_uiFormsDir;
		FileChooser *m_uiFormTranslationsDir;
		FileChooser *m_dataLoaderDir;
		QTreeWidget *m_treeWidget;
		QCheckBox *m_toggleAll;

	private slots:
		void toggleSecure( int state );
		void toggleLoadMode( bool checked );
	};

} // namespace QtClient
} // namespace _Wolframe

#endif // _STORAGEWIDGET_HPP_INCLUDED