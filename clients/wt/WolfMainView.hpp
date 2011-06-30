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
#include "Wt/WMenu"
#include "Wt/WStackedWidget"
#include "Wt/WText"
#include "Wt/WSubMenuItem"

using namespace Wt;

namespace _Wolframe {
	namespace WtClient {

class WolfMainView : public WContainerWidget
{
	public:
		WolfMainView( WContainerWidget *_parent ) : WContainerWidget( _parent )
		{
// create the stack where the contents will be located
 Wt::WStackedWidget *contents = new Wt::WStackedWidget(this);

 // create a menu
 Wt::WMenu *menu = new Wt::WMenu(contents, Wt::Horizontal, this);
 menu->setRenderAsList(true);
 menu->setStyleClass( WString("menu" ));

 // add four items using the default lazy loading policy.
 menu->addItem("Introduction", new Wt::WText("intro"));
 menu->addItem("Download", new Wt::WText("Not yet available"));
 menu->addItem("Demo", new Wt::WText("Not yet available"));
 menu->addItem(new Wt::WMenuItem("Demo2", new Wt::WText("Not yet available")));

		}
};

	} // namespace WtClient
} // namespace _Wolframe

#endif // _WOLF_MAIN_VIEW
