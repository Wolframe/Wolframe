//
// FileChooserPlugin.hpp
//

#ifndef _FILECHOOSERPLUGIN_HPP_INCLUDED
#define _FILECHOOSERPLUGIN_HPP_INCLUDED

#include <QObject>
#include <QDesignerCustomWidgetInterface>
#include <QWidget>

class FileChooserPlugin : public QObject, public QDesignerCustomWidgetInterface
{
	Q_OBJECT
	Q_INTERFACES( QDesignerCustomWidgetInterface )
	
	public:
		FileChooserPlugin( QObject *_parent = 0 );
		bool isContainer( ) const;
		bool isInitialized( ) const;
		QIcon icon( ) const;
		QString domXml( ) const;
		QString group( ) const;
		QString includeFile( ) const;
		QString name( ) const;
		QString toolTip( ) const;
		QString whatsThis( ) const;
		QWidget *createWidget( QWidget *_parent );
		void initialize( QDesignerFormEditorInterface *_core );
		
	private:
		bool m_initialized;
};

#endif // _FILECHOOSERPLUGIN_HPP_INCLUDED
