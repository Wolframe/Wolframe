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

#include "NetworkDataLoader.hpp"
#include "WolframeClientProtocol.hpp"
#include <QDebug>
#include <QByteArray>
#include <QXmlStreamWriter>

NetworkDataLoader::NetworkDataLoader( WolframeClient *_wolframeClient, bool _debug )
	: m_wolframeClient( _wolframeClient ),
	  m_map( new QHash<QByteArray, QPair<QString, QString> >( ) ),
	  m_debug( _debug )
{
	connect( m_wolframeClient, SIGNAL( answerReceived( bool,const QByteArray&,const QByteArray&) ),
		this, SLOT( gotAnswer( bool,const QByteArray&,const QByteArray&) ) );
}

void NetworkDataLoader::datarequest( const QByteArray& tag, const QByteArray& content)
{
	m_wolframeClient->request( tag, content);
}

void NetworkDataLoader::gotAnswer( bool success, const QByteArray& tag, const QByteArray& content )
{
	if( !success ) {
		qCritical( ) << "ERROR: " << tag << content;
		emit error( tag, content);
		return;
	}
	emit answer( tag, content);
}

