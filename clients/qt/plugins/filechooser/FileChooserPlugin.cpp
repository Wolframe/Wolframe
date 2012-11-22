//
// FileChooserPlugin.cpp
//

#include "FileChooserPlugin.hpp"
#include "FileChooser.hpp"

#include <QtPlugin>
 
FileChooserPlugin::FileChooserPlugin( QObject *_parent )
	: QObject( _parent ), m_initialized( false )
{
}

bool FileChooserPlugin::isContainer( ) const
{
	return false;
}

bool FileChooserPlugin::isInitialized( ) const
{
	return m_initialized;
}

QIcon FileChooserPlugin::icon( ) const
{
	return QIcon( );
}

QString FileChooserPlugin::domXml( ) const
{
	return 	"<ui language=\"c++\">\n"
		" <widget class=\"FileChooser\" name=\"fileChooser\">\n"
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

QString FileChooserPlugin::group( ) const
{
	return "Wolframe Widgets";
}

QString FileChooserPlugin::includeFile( ) const
{
	return "FileChooser.hpp";
}
 
QString FileChooserPlugin::name( ) const
{
	return "FileChooser";
}

QString FileChooserPlugin::toolTip( ) const
{
	return "";
}

QString FileChooserPlugin::whatsThis( ) const
{
	return "";
}

QWidget *FileChooserPlugin::createWidget( QWidget *_parent )
{
	// is overridden by property
	return new FileChooser( FileChooser::SelectExistingFile, false, _parent );
}

void FileChooserPlugin::initialize( QDesignerFormEditorInterface* /* core */ )
{
	if( m_initialized ) return;
	
	m_initialized = true;
}
