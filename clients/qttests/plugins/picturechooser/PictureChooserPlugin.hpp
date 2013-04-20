/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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

#ifndef _PICTURECHOOSERPLUGIN_HPP_INCLUDED
#define _PICTURECHOOSERPLUGIN_HPP_INCLUDED

#include <QDesignerCustomWidgetInterface>

class PictureChooserPlugin : public QObject, public QDesignerCustomWidgetInterface
{
	Q_OBJECT
	Q_INTERFACES( QDesignerCustomWidgetInterface )
#if QT_VERSION >= 0x050000
	Q_PLUGIN_METADATA( IID "org.qt-project.Qt.QDesignerCustomWidgetInterface" )
#endif // QT_VERSION >= 0x050000
	
	public:
		PictureChooserPlugin( QObject *_parent = 0 );
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

#endif // _PICTURECHOOSERPLUGIN_HPP_INCLUDED
