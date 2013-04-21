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

#ifndef _NETWORK_DATA_LOADER_INCLUDED
#define _NETWORK_DATA_LOADER_INCLUDED

#include "DataLoader.hpp"
#include "WolframeClient.hpp"

#include <QHash>
#include <QPair>
#include <QScopedPointer>
	
class NetworkDataLoader : public DataLoader
{	
	// intentionally omitting Q_OBJECT here, is done in DataLoader!
	
	public:
		NetworkDataLoader( WolframeClient *_wolframeClient, bool _debug = false );
		virtual ~NetworkDataLoader( ) {}

		virtual void datarequest( const QString& tag, const QByteArray& content);

	public slots:
		virtual void gotAnswer( bool success, const QString& tag, const QByteArray& content);

	private:
		WolframeClient *m_wolframeClient;
		QScopedPointer<QHash<QByteArray, QPair<QString, QString> > > m_map;
		bool m_debug;
};

#endif // _NETWORK_DATA_LOADER_INCLUDED
//
// NetworkDataLoader.hpp
//
