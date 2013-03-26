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

#ifndef _Wolframe_CLIENT_HPP_INCLUDED
#define _Wolframe_CLIENT_HPP_INCLUDED

#include <QObject>
#include <QWidget>
#include <QAbstractSocket>
#ifdef WITH_SSL
#include <QSslSocket>
#include <QSslCertificate>
#include <QList>
#endif
#include <QQueue>
#include <QStringList>
#include <QTimer>

#include "connection.hpp"

class WolframeClient : public QObject
{
	Q_OBJECT

	public:
		enum State {
			Disconnected,
			AboutToConnect,
			Connected,
			Data,
			AboutToDisconnect
		};

		struct WolframeRequest
		{
			QString command;
			QStringList params;
			QString content;

			WolframeRequest( const QString &_command, const QStringList &_params, const QString &_content )
				: command( _command ), params( _params ), content( _content ) { }
		};

	private:
		ConnectionParameters m_connParams;
		State m_state;
		QAbstractSocket *m_socket;
		QWidget *m_parent;
		bool m_hasErrors;
#ifdef WITH_SSL
		bool m_initializedSsl;
#endif
		QString m_answer;
		QStringList m_params;
		QString m_command;
		QQueue<WolframeRequest> m_queue;	// queued commands
		QTimer *m_timeoutTimer;

	public:
		WolframeClient( const ConnectionParameters _connParams,	QWidget *_parent = 0 );
		virtual ~WolframeClient( );

		void setConnectionParameters( const ConnectionParameters _connParams );

// low-level commands, pre-protocol, for debugging mainly
		void connect( );
		void disconnect( );
		void sendLine( QString line );

// generic send command function, implementing the frame of the protocol
		void sendCommand( QString command );
		void sendCommand( QString command, QStringList params );
		void sendCommand( QString command, QStringList params, QString content );
		void sendCommand( QString command, QString content );
		void sendCommand( struct WolframeRequest );

// high-level commands
		void capa( );
		void auth( );
		void mech( QString mech );
		void request( QString type, QString content );
		void request( QString content );

// inquire status
		bool isConnected( ) const;
		const QString serverName() const;
		bool isEncrypted( ) const;
		const QString encryptionName() const;

	private slots:
		void timeoutOccurred( );
		void error( QAbstractSocket::SocketError );
#ifdef WITH_SSL
		void initializeSsl( );
		QSslCertificate getCertificate( QString filename );
		void sslErrors( const QList<QSslError> &errors );
		void encrypted( );
		void peerVerifyError( const QSslError &e );
#endif
		void dataAvailable( );
		void privateConnected( );
		void privateDisconnected( );

		void handleResult( );

	signals:
// low-level events
		void connected( );
		void disconnected( );
		void error( QString error );

// low-level commands, pre-protocol, for debugging mainly
		void lineReceived( QString line );
		void lineSent( QString line );

// generic implementation of a command execute implementing the frame of the protocol
		void resultReceived( );

// high-level commands
		void capasReceived( QStringList cataList );
		void mechsReceived( QStringList mechList );
		void authOk( );
		void authFailed( );
		void answerReceived( QStringList params, QString content );
};

#endif // _Wolframe_CLIENT_HPP_INCLUDED
