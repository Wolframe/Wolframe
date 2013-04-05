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

#include "WolframeClientProtocol.hpp"
#undef WOLFRAME_LOWLEVEL_DEBUG

const QString* WolframeClientProtocol::getLastError() const
{
	return (m_lasterror.isEmpty())?0:&m_lasterror;
}

bool WolframeClientProtocol::sendCommandLine( const QByteArray& cmd, const QByteArray& arg)
{
	return sendLine( QByteArray( cmd).append(' ').append( arg));
}

bool WolframeClientProtocol::sendLine( const QByteArray& line)
{
	qint64 status = m_socket->write( QByteArray( line).append( "\r\n"));
	if (status < 0)
	{
		m_lasterror = "send line failed: ";
		m_lasterror.append( m_socket->errorString());
		return false;
	}
	if (status < line.size() + 2)
	{
		m_lasterror = "write line failed (only partial write on socket)";
		return false;
	}
	qDebug() << "send line:" << line;
	return true;
}

bool WolframeClientProtocol::sendRequestContent()
{
	if (m_requestqueue.isEmpty()) return false;
	QByteArray escdata = WolframeClientProtocolBase::escapedContent( m_requestqueue.head().second);
	escdata.append( "\r\n.\r\n");
	qint64 status = m_socket->write( escdata);
	if (status < 0)
	{
		m_lasterror = "send request content failed: ";
		m_lasterror.append( m_socket->errorString());
		return false;
	}
	if (status < escdata.size())
	{
		m_lasterror = "write request content failed (only partial write on socket)";
		return false;
	}
#ifdef WOLFRAME_LOWLEVEL_DEBUG
	qDebug() << "network send content (esc" << status <<"):" << escdata;
#endif
	m_requesttagqueue.enqueue( m_requestqueue.head().first);
	m_requestqueue.dequeue();
	return true;
}

QString WolframeClientProtocol::nextAnswerTag()
{
	QString rt = m_requesttagqueue.head();
	m_requesttagqueue.dequeue();
	return rt;
}

void WolframeClientProtocol::pushAnswerError( const QByteArray& msg)
{
	m_errorqueue.enqueue( qMakePair( nextAnswerTag(), msg));
	qDebug() << "answer queue size (push) =" << m_errorqueue.size() + m_answerqueue.size();
}

void WolframeClientProtocol::pushAnswerContent( const QByteArray& content)
{
	m_answerqueue.enqueue( qMakePair( nextAnswerTag(), content));
	qDebug() << "answer queue size (push) =" << m_errorqueue.size() + m_answerqueue.size();
}

bool WolframeClientProtocol::hasRequests() const
{
	return !m_requestqueue.isEmpty();
}

bool WolframeClientProtocol::poll()
{
	qint64 len = m_socket->bytesAvailable();
	QByteArray blk = m_socket->read( len);
	if (len)
	{
		pushData( blk);
		return true;
	}
	return false;
}

const WolframeClientProtocol::Item* WolframeClientProtocol::getNextItem()
{
	const Item* rt = WolframeClientProtocolBase::getNextItem();
	if (!rt)
	{
		poll();
		rt = WolframeClientProtocolBase::getNextItem();
	}
	return rt;
}

bool WolframeClientProtocol::process()
{
	const Item* item = 0;
	poll();

	for (;;)
	{
		qDebug() << "protocol state " << stateName(m_state);
		switch (m_state)
		{
			case Close:
				return false;

			case Init:
				m_gotAuthorize = true;
				if (!getNextItem()) return true;
				m_state = ReadBanner;
				continue;

			case ReadBanner:
				item = getNextItem();
				if (!item) return true;
				if (strcmp( item->m_tag, "OK") == 0)
				{
					m_state = AuthStart;
					continue;
				}
				if (strcmp( item->m_tag, "ERR") == 0)
				{
					m_lasterror = "rejected connection: ";
					m_lasterror.append( item->m_data);
					return false;
				}
				if (strcmp( item->m_tag, "BYE") == 0)
				{
					m_lasterror = "server closed connection";
					m_state = Close;
					return false;
				}
				m_lasterror = "protocol error. OK/ERR/BYE expected, got: ";
				m_lasterror.append( item->m_tag);
				m_state = Close;
				return false;

			case AuthStart:
				if (m_gotQuit)
				{
					if (!sendLine( "QUIT")) return false;
					m_state = Close;
					return true;
				}
				if (!m_gotAuthorize) return true;
				if (!sendLine( "AUTH")) return false;
				m_state = AuthGetMechs;
				continue;

			case AuthGetMechs:
				item = getNextItem();
				if (!item) return true;
				if (strcmp( item->m_tag, "MECHS") == 0)
				{
					QList<QByteArray> mechs = item->m_data.split(' ');
					if (!mechs.contains("NONE"))
					{
						m_lasterror = "requred authorization mech NONE is not supported by the server";
						m_state = Close;
					}
					if (m_gotQuit)
					{
						if (!sendLine( "QUIT")) return false;
						m_state = Close;
						return true;
					}
					m_state = AuthMechNone;
					continue;
				}
				if (strcmp( item->m_tag, "ERR") == 0)
				{
					m_lasterror = "error on AUTH request: ";
					m_lasterror.append( item->m_data);
					return false;
				}
				if (strcmp( item->m_tag, "BYE") == 0)
				{
					m_lasterror = "server closed connection";
					m_state = Close;
					return false;
				}
				m_lasterror = "protocol error. MECHS/ERR/BYE expected, got: ";
				m_lasterror.append( item->m_tag);
				m_state = Close;
				return false;

			case AuthMechNone:
				if (!sendLine( "MECH NONE")) return false;
				m_state = AuthMechNoneResponse;

			case AuthMechNoneResponse:
				item = getNextItem();
				if (!item) return true;
				if (strcmp( item->m_tag, "OK") == 0)
				{
					m_gotAuthorize = false;
					m_state = AuthorizedIdle;
					continue;
				}
				if (strcmp( item->m_tag, "ERR") == 0)
				{
					m_lasterror = "error on MECH NONE request: ";
					m_lasterror.append( item->m_data);
					m_state = Close;
					return false;
				}
				if (strcmp( item->m_tag, "BYE") == 0)
				{
					m_lasterror = "server closed connection";
					m_state = Close;
					return false;
				}
				m_lasterror = "protocol error. OK/ERR/BYE expected, got: ";
				m_lasterror.append( item->m_tag);
				m_state = Close;
				return false;

			case AuthorizedIdle:
				item = getNextItem();
				if (item)
				{
					m_state = AuthorizedAnswer;
					continue;
				}
				if (m_gotQuit)
				{
					if (!sendLine( "QUIT")) return false;
					m_state = Close;
					return true;
				}
				if (m_requestqueue.isEmpty())
				{
					if (!m_requesttagqueue.isEmpty())
					{
						qDebug() << "pending requests" << m_requesttagqueue.size() << "top" << m_requesttagqueue.head();
					}
					if (m_gotAuthorize)
					{
						m_state = AuthStart;
						continue;
					}
					return true;
				}
				else
				{
					if (!sendLine( "REQUEST")) return false;
					m_state = AuthorizedRequest;
					continue;
				}
				return true;

			case AuthorizedRequest:
				if (!sendRequestContent()) return false;
				m_state = AuthorizedIdle;
				continue;

			case AuthorizedAnswer:
				///... item has been fetched in state 'AuthorizedIdle' !
				if (strcmp( item->m_tag, "ANSWER") == 0)
				{
					m_state = AuthorizedAnswerData;
					setDataMode();
					continue;
				}
				if (strcmp( item->m_tag, "ERR") == 0)
				{
					pushAnswerError( item->m_data);
					m_state = AuthorizedIdle;
					continue;
				}
				if (strcmp( item->m_tag, "BYE") == 0)
				{
					m_lasterror = "server closed connection";
					m_state = Close;
					return false;
				}
				m_lasterror = "protocol error. ANSWER/ERR/BYE expected, got: ";
				m_lasterror.append( item->m_tag);
				m_state = Close;
				return false;

			case AuthorizedAnswerData:
				item = getNextItem();
				if (!item) return true;
				if (item->m_type != Item::Data)
				{
					m_lasterror = "internal error. expected data, got line";
					return false;
				}
				m_content = item->m_data;
				m_state = AuthorizedAnswerResult;
				continue;

			case AuthorizedAnswerResult:
				item = getNextItem();
				if (!item) return true;
				if (strcmp( item->m_tag, "OK") == 0)
				{
					pushAnswerContent( m_content);
					m_content.clear();
					m_state = AuthorizedIdle;
					setLineMode();
					continue;
				}
				if (strcmp( item->m_tag, "ERR") == 0)
				{
					pushAnswerError( item->m_data);
					m_content.clear();
					m_state = AuthorizedIdle;
					continue;
				}
				if (strcmp( item->m_tag, "BYE") == 0)
				{
					m_lasterror = "server closed connection";
					m_state = Close;
					return false;
				}
				m_lasterror = "protocol error. OK/ERR/BYE expected, got: ";
				m_lasterror.append( item->m_tag);
				m_state = Close;
				return false;
		}
	}
}

void WolframeClientProtocol::pushRequest( const QString& tag, const QByteArray& content)
{
	typedef QPair<QString,QByteArray> Request;
	if (tag[0] != '-')
	{
		QList<Request>::iterator ri = m_requestqueue.begin(), re = m_requestqueue.end();
		for (; ri != re; ++ri)
		{
			if (ri->first == tag)
			{
				qDebug() << "replace pending domain load request, tag=" << tag;
				ri->second = content;
				return;
			}
		}
	}
	qDebug() << "push request tag=" << tag;
	m_requestqueue.enqueue( qMakePair( tag, content));
	process();
}

bool WolframeClientProtocol::getAnswerSuccess() const
{
	return m_errorqueue.isEmpty();
}

const QString* WolframeClientProtocol::getAnswerTag() const
{
	if (m_errorqueue.isEmpty())
	{
		if (m_answerqueue.isEmpty()) return 0;
		return &m_answerqueue.head().first;
	}
	else
	{
		return &m_errorqueue.head().first;
	}
}

const QByteArray* WolframeClientProtocol::getAnswerContent() const
{
	if (m_errorqueue.isEmpty())
	{
		if (m_answerqueue.isEmpty()) return 0;
		return &m_answerqueue.head().second;
	}
	else
	{
		return &m_errorqueue.head().second;
	}
}

void WolframeClientProtocol::removeAnswer()
{
	if (m_errorqueue.isEmpty())
	{
		m_answerqueue.dequeue();
	}
	else
	{
		m_errorqueue.dequeue();
	}
}

