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

#include "WolframeClientProtocolBase.hpp"
#include <QDebug>

void WolframeClientProtocolBase::pushData( const QByteArray& buf)
{
	if (m_bufpos > 0)
	{
		m_buf = m_buf.mid( m_bufpos, m_buf.size() - m_bufpos);
		m_bufpos = 0;
	}
	m_buf.append( buf);
}

void WolframeClientProtocolBase::pushData( const char* buf, int bufsize)
{
	if (m_bufpos > 0)
	{
		m_buf = m_buf.mid( m_bufpos, m_buf.size() - m_bufpos);
		m_bufpos = 0;
	}
	m_buf.append( buf, bufsize);
}

QByteArray WolframeClientProtocolBase::getNextLine()
{
	int eolnpos = m_buf.indexOf( '\n', m_bufpos);
	if (eolnpos >= 0)
	{
		QByteArray rt = m_buf.mid( m_bufpos, eolnpos - m_bufpos);
		if (rt.endsWith( '\r')) rt.resize( rt.size()-1);
		m_bufpos = eolnpos+1;
		return rt;
	}
	return QByteArray();
}

QByteArray WolframeClientProtocolBase::escapedContent( const QByteArray& data)
{
	QByteArray rt;
	int datapos = 0;

	int curpos = data.indexOf( '\n');
	for (; curpos >= 0; curpos = data.indexOf( '\n', datapos))
	{
		rt.append( data.mid( datapos, curpos +1 -datapos));
		datapos = curpos + 1;

		if (curpos+1 < data.size() && data.at(curpos+1) == '.')
		{
			rt.append( '.'); //... escape the dot after LF
		}
	}
	rt.append( data.mid( datapos, data.size() -datapos));
	return rt;
}

bool WolframeClientProtocolBase::getItemDataUnescaped()
{
	int curpos = m_buf.indexOf( '\n', m_bufpos);
	for (; curpos >= 0; curpos=m_buf.indexOf( '\n', m_bufpos))
	{
		if (curpos+1 < m_buf.size() && m_buf.at(curpos+1) == '.')
		{
			int eodpos;
			if (curpos > 0 && m_buf.at( curpos -1) == '\r')
			{
				eodpos = curpos -1;
			}
			else
			{
				eodpos = curpos;
			}
			int followpos = -1;
			if (curpos+2 < m_buf.size() && m_buf.at(curpos+2) == '\r')
			{
				if (curpos+3 < m_buf.size() && m_buf.at(curpos+3) == '\n')
				{
					followpos = curpos+4;
				}
			}
			else if (curpos+2 < m_buf.size() && m_buf.at(curpos+2) == '\n')
			{
				followpos = curpos+3;
			}
			if (followpos >= 0)
			{
				//.... we detected the EoD Marker (CR)LF dot (CR)LF
				m_item.m_data.append( m_buf.mid( m_bufpos, eodpos-eodpos));
				m_bufpos = followpos;
				return true;
			}
			else
			{
				m_item.m_data.append( m_buf.mid( m_bufpos, curpos +1 -m_bufpos));
				//... append line as data including the EOLN (LF)
				m_bufpos = curpos + 2;
				//... pos after the escaped dot
			}
		}
		else
		{
			m_item.m_data.append( m_buf.mid( m_bufpos, curpos +1 -m_bufpos));
			//... append line as data including the EOLN (LF)
			m_bufpos = curpos + 1;
			//... pos at start of the next line (nothing to escape)
		}
	}
	return false;
}

bool WolframeClientProtocolBase::hasData() const
{
	return m_bufpos < m_buf.size();
}


bool WolframeClientProtocolBase::setDataMode()
{
	if (m_parsestate != InitMode) return false;
	m_item.m_data.clear();
	m_item.m_tag.clear();
	m_item.m_type = Item::Data;
	m_parsestate = DataMode;
	return true;
}

bool WolframeClientProtocolBase::setLineMode()
{
	if (m_parsestate != InitMode) return false;
	m_item.m_data.clear();
	m_item.m_tag.clear();
	m_item.m_type = Item::Line;
	m_parsestate = LineMode;
	return true;
}

const WolframeClientProtocolBase::Item* WolframeClientProtocolBase::getNextItem()
{
	for (;;) switch (m_parsestate)
	{
		case InitMode:
			setLineMode();
			continue;

		case LineMode:
		{
			QByteArray line = getNextLine();

			if (line.isEmpty())
			{
				if (!hasData())
				{
					//... not enough data to complete the request, caller has to push more data
					return 0;
				}
				qDebug() << "received line:" << line;
				continue; //... an empty line is ignored
			}
			else
			{
				qDebug() << "received line:" << line;
			}
			int cmdendpos = line.indexOf( ' ');
			m_item.m_tag = line.mid( 0, cmdendpos).toUpper();
			m_item.m_data = line.mid( cmdendpos + 1, line.size() -cmdendpos -1).trimmed();
			m_item.m_type = Item::Line;
			m_parsestate = InitMode;
			return &m_item;
		}
		case DataMode:
		{
			if (getItemDataUnescaped())
			{
				m_parsestate = InitMode;
				return &m_item;
			}
			else
			{
				//... not enough data to complete the request, caller has to push more data
				return 0;
			}
		}
	}
}

