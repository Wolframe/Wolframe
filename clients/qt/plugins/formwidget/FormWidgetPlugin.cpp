//
// FormWidgetPlugin.cpp
//

#include "FormWidgetPlugin.hpp"
#include "FormWidget.hpp"

#include <QtPlugin>
#include <QDebug>
 
FormWidgetPlugin::FormWidgetPlugin( QObject *_parent )
	: QObject( _parent ), m_initialized( false )
{
}

bool FormWidgetPlugin::isContainer( ) const
{
	return false;
}

bool FormWidgetPlugin::isInitialized( ) const
{
	return m_initialized;
}

QIcon FormWidgetPlugin::icon( ) const
{
	return QIcon( );
}

QString FormWidgetPlugin::domXml( ) const
{
	return 	"<ui language=\"c++\">\n"
		" <widget class=\"FormWidget\" name=\"formWidget\">\n"
		"  <property name=\"geometry\">\n"
		"   <rect>\n"
		"    <x>0</x>\n"
		"    <y>0</y>\n"
		"    <width>100</width>\n"
		"    <height>100</height>\n"
		"   </rect>\n"
		"  </property>\n"
		" </widget>\n"
		"</ui>\n";
}

QString FormWidgetPlugin::group( ) const
{
	return "Wolframe Widgets";
}

QString FormWidgetPlugin::includeFile( ) const
{
	return "FormWidget.hpp";
}
 
QString FormWidgetPlugin::name( ) const
{
	return "FormWidget";
}

QString FormWidgetPlugin::toolTip( ) const
{
	return "";
}

QString FormWidgetPlugin::whatsThis( ) const
{
	return "";
}

QWidget *FormWidgetPlugin::createWidget( QWidget *_parent )
{
	if( m_initialized ) {
// in designer mode, return a preview version only
		return new FormWidget( _parent );
	} else {
// in normal form load mode, but we must inject some parent pointers, how?
		return new FormWidget( _parent );
	}
}

void FormWidgetPlugin::initialize( QDesignerFormEditorInterface* /* core */ )
{
	if( m_initialized ) return;

	m_initialized = true;
}
