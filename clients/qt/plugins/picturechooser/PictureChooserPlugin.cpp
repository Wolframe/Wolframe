//
// PictureChooserPlugin.cpp
//

#include "PictureChooserPlugin.hpp"
#include "PictureChooser.hpp"

#include <QtPlugin>
 
PictureChooserPlugin::PictureChooserPlugin( QObject *_parent )
	: QObject( _parent ), m_initialized( false )
{
}

bool PictureChooserPlugin::isContainer( ) const
{
	return false;
}

bool PictureChooserPlugin::isInitialized( ) const
{
	return m_initialized;
}

QIcon PictureChooserPlugin::icon( ) const
{
	return QIcon( );
}

QString PictureChooserPlugin::domXml( ) const
{
	return 	"<ui language=\"c++\">\n"
		" <widget class=\"PictureChooser\" name=\"pictureChooser\">\n"
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

QString PictureChooserPlugin::group( ) const
{
	return "Wolframe Widgets";
}

QString PictureChooserPlugin::includeFile( ) const
{
	return "PictureChooser.hpp";
}
 
QString PictureChooserPlugin::name( ) const
{
	return "PictureChooser";
}

QString PictureChooserPlugin::toolTip( ) const
{
	return "";
}

QString PictureChooserPlugin::whatsThis( ) const
{
	return "";
}

QWidget *PictureChooserPlugin::createWidget( QWidget *_parent )
{
	// is overridden by property
	return new PictureChooser( _parent );
}

void PictureChooserPlugin::initialize( QDesignerFormEditorInterface* /* core */ )
{
	if( m_initialized ) return;
	
	m_initialized = true;
}
