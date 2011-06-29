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

#include "Wt/WApplication"
#include "Wt/WOverlayLoadingIndicator"

using namespace Wt;

class WtClientApp : public WApplication
{
	public:
		WtClientApp( const WEnvironment &_env )
			: WApplication( _env ) {
			setTitle( "Wt Wolframe Client" );
		}
};

namespace {

WApplication *createApp( const WEnvironment &_env )
{
	WtClientApp *app = new WtClientApp( _env );
	app->setLoadingIndicator( new WOverlayLoadingIndicator( ) );
	return app;
}

}

int main( int argc, char *argv[] )
{
	return WRun( argc, argv, &createApp );
}

