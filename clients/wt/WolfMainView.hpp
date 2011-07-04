/************************************************************************

 Copyright (C) 2011 Project Wolframe.
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

#ifndef _WOLF_MAIN_VIEW
#define _WOLF_MAIN_VIEW

#include "Wt/WContainerWidget"
#include "Wt/WText"
#include "Wt/WGridLayout"
#include "Wt/WScrollArea"
#include "Wt/WScrollBar"

#include "Wt/Ext/Menu"
#include "Wt/Ext/ToolBar"
#include "Wt/Ext/MessageBox"
#include "Wt/Ext/Dialog"
#include "Wt/Ext/Container"

using namespace Wt;
using namespace Wt::Ext;

namespace _Wolframe {
	namespace WtClient {

class WolfMainView : public WContainerWidget
{
	public:
		WolfMainView( WContainerWidget *_parent ) : WContainerWidget( _parent )
		{
			ToolBar *toolBar = new ToolBar( this );
			Menu *menu = new Menu( );
			MenuItem *item;
			
			toolBar->addButton( "File", menu );
			menu->addItem( "Logoff" );

			menu = new Menu( );
			toolBar->addButton( "Forms", menu );
			menu->addItem( "form1.ui" );
			menu->addItem( "form2.ui" );
			
			menu = new Menu( );
			toolBar->addButton( "Themes", menu );
			item = menu->addItem( "default" );
			item->setChecked( true );
			item->setCheckable( true );
			item->toggled( ).connect( SLOT( this, WolfMainView::chooseTheme ) );
			item = menu->addItem( "gray" );
			item->setCheckable( true );
			item->toggled( ).connect( SLOT( this, WolfMainView::chooseTheme ) );
			item = menu->addItem( "slate" );
			item->setCheckable( true );
			item->toggled( ).connect( SLOT( this, WolfMainView::chooseTheme ) );

			menu = new Menu( );
			toolBar->addButton( "Help", menu );
			item = menu->addItem( "About" );
			item->activated( ).connect( SLOT( this, WolfMainView::showAbout ) );
			menu->addSeparator( );
			item = menu->addItem( "About Wt" );
			item->activated( ).connect( SLOT( this, WolfMainView::showWtAbout ) );
			menu->addSeparator( );
			menu->addItem( "Debug Window" );			
		}
		
		void showAbout( )
		{
			Dialog dialog;
			dialog.setWindowTitle( "About Wolfbones" );
			dialog.setAutoScrollBars( true );

			Button *okButton = new Button( "Ok" );
			okButton->activated( ).connect( SLOT( &dialog, Dialog::accept ) );
			dialog.addButton( okButton );
			okButton->setDefault( true );

			WText *text = new WText( tr( "about.wolframe" ), Wt::XHTMLText );
			dialog.contents( )->addWidget( text );

			dialog.resize( 500,300 );
			dialog.exec( );
		}

		void showWtAbout( )
		{
			Dialog dialog;
			dialog.setWindowTitle( "About Wt" );
			dialog.setAutoScrollBars( true );

			Button *okButton = new Button( "Ok" );
			okButton->activated( ).connect( SLOT( &dialog, Dialog::accept ) );
			dialog.addButton( okButton );
			okButton->setDefault( true );

			WText *text = new WText( tr( "about.wt" ), Wt::XHTMLText );
			dialog.contents( )->addWidget( text );

			dialog.resize( 500,300 );
			dialog.exec( );
		}

		void chooseTheme( bool )
		{
		}
};

	} // namespace WtClient
} // namespace _Wolframe

#endif // _WOLF_MAIN_VIEW
