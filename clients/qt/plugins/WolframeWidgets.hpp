//
// WolframeWidgets.hpp
//

#ifndef _WOLFRAMEWIDGET_HPP_INCLUDED
#define _WOLFRAMEWIDGET_HPP_INCLUDED

#include <QObject>
#include <QDesignerCustomWidgetCollectionInterface>
#include <QWidget>

class WolframeWidgets : public QObject, public QDesignerCustomWidgetCollectionInterface
{
	Q_OBJECT
	Q_INTERFACES( QDesignerCustomWidgetCollectionInterface )
	
	public:
		WolframeWidgets( QObject *_parent = 0 );
		
		virtual QList<QDesignerCustomWidgetInterface *> customWidgets( ) const;
		
	private:
		QList<QDesignerCustomWidgetInterface *> m_plugins;
};

#endif // _WOLFRAMEWIDGET_HPP_INCLUDED
