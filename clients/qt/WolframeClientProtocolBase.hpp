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

#ifndef _Wolframe_CLIENT_PROTOCOL_BASE_HPP_INCLUDED
#define _Wolframe_CLIENT_PROTOCOL_BASE_HPP_INCLUDED

#include <QObject>
#include <QString>
#include <QByteArray>

class WolframeClientProtocolBase
{
public:
	WolframeClientProtocolBase()
		:m_parsestate(InitMode){}

	void pushData( const QByteArray& buf);
	void pushData( const char* buf, int bufsize);

	struct Item
	{
		enum Type
		{
			Data,		//< m_tag=request tag, m_data=unescaped data
			Line		//< m_tag=command name, m_data=rest of line without CRLF/LF
		};
		Type m_type;
		QByteArray m_tag;		//< item tag
		QByteArray m_data;	//< item data
	};

	const Item* getNextItem();

	bool setDataMode();
	bool setLineMode();

	static QByteArray escapedContent( const QByteArray& data);

private:
	QByteArray getNextLine();
	bool hasData() const;
	bool getItemDataUnescaped();

	enum ParseState
	{
		InitMode,
		LineMode,
		DataMode
	};
	ParseState m_parsestate;
	Item m_item;
	QByteArray m_buf;
	int m_bufpos;
};

#endif // _Wolframe_CLIENT_PROTOCOL_BASE_HPP_INCLUDED

