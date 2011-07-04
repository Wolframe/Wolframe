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
#include "Wt/WBorderLayout"
#include "Wt/WFitLayout"
#include "Wt/WScrollBar"

#include "Wt/Ext/Menu"
#include "Wt/Ext/ToolBar"
#include "Wt/Ext/Dialog"
#include "Wt/Ext/Container"
#include "Wt/Ext/LineEdit"

using namespace Wt;
using namespace Wt::Ext;

namespace _Wolframe {
	namespace WtClient {

class WolfMainView : public WContainerWidget
{
	private:
		Panel *south;
		MenuItem *menuItemDebugWindow;

	public:
		WolfMainView( WContainerWidget *_parent ) : WContainerWidget( _parent )
		{
			resize( WLength::Auto, WLength( 100, WLength::Percentage ) );

			WBorderLayout *theLayout = new WBorderLayout( this );

			// north: a menu bar

			Panel *north = new Panel( );
			north->setBorder( false );
			north->resize( WLength::Auto, 35 );

			ToolBar *toolBar = new ToolBar( );
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
			menuItemDebugWindow = menu->addItem( "Debug Window" );
			menuItemDebugWindow->setChecked( false );
			menuItemDebugWindow->setCheckable( true );
			menuItemDebugWindow->activated( ).connect( SLOT( this, WolfMainView::toggleDebugWindow ) );

			north->setLayout( new WFitLayout( ) );
			north->layout( )->addWidget( toolBar );
			theLayout->addWidget( north, WBorderLayout::North );

			// center: main forms
			Panel *center = new Panel( );
			center->setBorder( false );
			center->setResizable( true );
			center->resize( WLength::Auto, 250 );
			center->setCollapsible( true );
			center->setAnimate( true );
			center->setLayout( new WFitLayout( ) );
			center->layout( )->addWidget( new WText( "form" ) );
			theLayout->addWidget( center, WBorderLayout::Center );

			// south: a debug window, resizeable

			south = new Panel( );
			south->setTitle( "Debug Window" );
			south->setBorder( false );
			south->resize( WLength::Auto, 55 );
			south->setResizable( true );
			south->setCollapsible( true );
			south->setAnimate( true );
			south->setCollapsed( true );
			south->expanded( ).connect( SLOT( this, WolfMainView::enableDebugWindow ) );
			south->collapsed( ).connect( SLOT( this, WolfMainView::disableDebugWindow ) );
			south->setLayout( new WFitLayout( ) );
			south->layout( )->addWidget( new LineEdit( "blabla" ) );
			theLayout->addWidget( south, WBorderLayout::South );
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

		void toggleDebugWindow( )
		{
			south->setCollapsed( !south->isCollapsed( ) );
			menuItemDebugWindow->setChecked( !south->isCollapsed( ) );
		}

		void enableDebugWindow( )
		{
			menuItemDebugWindow->setChecked( true );
		}

		void disableDebugWindow( )
		{
			menuItemDebugWindow->setChecked( false );
		}
};

	} // namespace WtClient
} // namespace _Wolframe

#endif // _WOLF_MAIN_VIEW
