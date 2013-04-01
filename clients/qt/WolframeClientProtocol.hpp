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

#ifndef _Wolframe_CLIENT_PROTOCOL_HPP_INCLUDED
#define _Wolframe_CLIENT_PROTOCOL_HPP_INCLUDED
#include "WolframeClientProtocolBase.hpp"
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

class WolframeClientProtocol
		:public WolframeClientProtocolBase
{
	public:
		enum State {
			Init,
			Close,
			ReadBanner,
			AuthStart,
			AuthGetMechs,
			AuthMechNone,
			AuthMechNoneResponse,
			AuthorizedIdle,
			AuthorizedAnswer,
			AuthorizedRequest,
			AuthorizedAnswerData,
			AuthorizedAnswerResult
		};
		static const char* stateName( State i)
		{
			static const char* ar[] =
			{
				"Init","Close","ReadBanner","AuthStart","AuthGetMechs","AuthMechNone",
				"AuthMechNoneResponse","AuthorizedIdle","AuthorizedAnswer",
				"AuthorizedRequest","AuthorizedAnswerData","AuthorizedAnswerResult"
			};
			return ar[(int)i];
		}
		///\brief Constructor
		///\param[in] socket_ reference to socket to use
		explicit WolframeClientProtocol( QAbstractSocket* socket_=0)
			:m_state(Init)
			,m_socket(socket_)
			,m_gotQuit(false)
			,m_gotAuthorize(false){}
		~WolframeClientProtocol(){}

		void initSocket( QAbstractSocket* socket_)
		{
			m_socket = socket_;
		}

		///\brief overload of WolframeClientProtocolBase::getNextItem() with reading from network
		const Item* getNextItem();

		///\brief Handler for read event on the socket. Stears the whole protocol STM
		bool process();

		///\brief Push a request and process it if possible
		void pushRequest( const QString& tag, const QByteArray& content);

		///\brief Get the tag of the last answer or NULL if no answer available yet
		const QString* getAnswerTag() const;
		///\brief Get the success of the last answer or true if no answer available yet
		bool getAnswerSuccess() const;
		///\brief Get the content or message of the last answer or NULL if no answer available yet
		const QByteArray* getAnswerContent() const;
		///\brief Dispose the last answer
		void removeAnswer();

		///\brief Get the last protocol or network error
		const QString* getLastError() const;

		void quit()						{m_gotQuit = true;}
		bool isAuthorized() const				{return (int)m_state >= (int)AuthorizedIdle;}
		void authorize()					{m_gotAuthorize = true;}

	private:
		bool poll();
		bool sendLine( const QByteArray& line);
		bool sendCommandLine( const QByteArray& cmd, const QByteArray& arg);
		bool sendRequestContent();
		bool hasRequests() const;
		QString nextAnswerTag();
		void pushAnswerError( const QByteArray& msg);
		void pushAnswerContent( const QByteArray& content);

	private:
		State m_state;
		QAbstractSocket *m_socket;
		QQueue<QPair<QString,QByteArray> > m_requestqueue;	// queued commands (tag,content)
		QQueue<QString> m_requesttagqueue;			// queued tags of issued commands (tag)
		QQueue<QPair<QString,QByteArray> > m_answerqueue;	// queued answers (tag,content)
		QQueue<QPair<QString,QByteArray> > m_errorqueue;	// queued error answers (tag,message)
		QByteArray m_content;					// buffer for pending answer content
		QString m_lasterror;
		bool m_gotQuit;
		bool m_gotAuthorize;
};

#endif // _Wolframe_CLIENT_PROTOCOL_HPP_INCLUDED

