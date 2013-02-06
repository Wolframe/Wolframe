/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
 All rights reserved.

 This file is part of Project Wolframe.

 Commercial Usage
    Licensees holding valid Project Wolframe Commercial licenses may
    use this file in accordance with the Project Wolframe
    Commercial License Agreement provided with the Software or,
    alternatively, in accordance with the terms contained
    in a written agreement between the licensee and Project Wolframe.

 GNU General Public License Usage
    Alternatively, you can redistribute this file and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Wolframe is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/

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
