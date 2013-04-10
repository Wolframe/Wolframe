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
#include "DataTree.hpp"

void DataTree::addNode( const QString& name_, const DataTree& value_)
{
	if (m_elemtype != Array)
	{
		m_nodear.push_back( Node( name_, value_));
	}
}

void DataTree::addAttribute( const QString& name_, const DataTree& value_)
{
	if (m_nofattributes == m_nodear.size())
	{
		++m_nofattributes;
	}
	addNode( name_, value_);
}

void DataTree::addArrayElement( const DataTree& value_)
{
	if (m_elemtype == Array)
	{
		m_nodear.push_back( Node( QString(""), value_));
	}
}

void DataTree::setNode( const QString& name_, const DataTree& value_)
{
	if (m_elemtype != Array)
	{
		QList<Node>::iterator ni = m_nodear.begin(), ne = m_nodear.end();
		for (; ni != ne; ++ni)
		{
			if (ni->name == name_)
			{
				ni->value = new DataTree( value_);
				return;
			}
		}
		addNode( name_, value_);
	}
}

const DataTree& DataTree::node( const QString& name_) const
{
	static const DataTree inValidNode;
	foreach (const Node& nd, m_nodear)
	{
		if (nd.name == name_) return *nd.value;
	}
	return inValidNode;
}

const QVariant& DataTree::value() const
{
	return m_value;
}

bool DataTree::isValid() const
{
	return m_value.isValid() || !m_nodear.isEmpty();
}

QString DataTree::parseString( QString::const_iterator& itr, const QString::const_iterator& end)
{
	QChar eb = *itr;
	QString::const_iterator start;
	for (start=++itr; itr != end && *itr != eb; ++itr);
	return QString( start, itr-start);
}

void DataTree::skipBrk( QString::const_iterator& itr, const QString::const_iterator& end)
{
	int brkcnt = 1;
	++itr;
	for (; itr != end; ++itr)
	{
		if (*itr == '{')
		{
			++brkcnt;
			continue;
		}
		if (*itr == '}')
		{
			--brkcnt;
			if (brkcnt == 0) break;
		}
	}
}

void DataTree::setNodeValue( const QVariant& nodevalue)
{
	if (m_value.isValid())
	{
		if (m_value.isValid())
		{
			if (m_value.type() == QVariant::List)
			{
				QList<QVariant> lst = m_value.toList();
				lst.push_back( nodevalue);
				m_value = QVariant( lst);
			}
			else
			{
				QList<QVariant> lst;
				lst.push_back( m_value);
				lst.push_back( nodevalue);
				m_value = QVariant( lst);
			}
		}
		else
		{
			m_value = QVariant( nodevalue);
		}
	}
}

DataTree DataTree::fromString( const QString::const_iterator& begin, const QString::const_iterator& end)
{
	DataTree rt;
	QString::const_iterator is = begin, es = end;
	QString value;
	for (; is != es; ++is)
	{
		QString nodename = value.trimmed();
		QString nodevalue;
		if (*is == '\'' || *is == '\"')
		{
			nodevalue = parseString( is, es);
			if (!nodename.isEmpty())
			{
				rt.setNodeValue( QVariant( nodename));
			}
			rt.setNodeValue( QVariant( nodevalue));
		}
		else if (*is == '{')
		{
			QString::const_iterator start = is;
			++start;
			skipBrk( is, es);

			if (nodename.isEmpty())
			{
				nodevalue.push_back( '{');
				nodevalue.append( QString( start, is-start).trimmed());
				nodevalue.push_back( '}');
				rt.setNodeValue( nodevalue);
			}
			else
			{
				rt.addNode( nodename, fromString( start, is));
				if (is == es) break;
			}
		}
		else if (*is == '=')
		{
			++is;
			for (; is != es && *is == ' '; ++is);
			if (is == es)
			{
				rt.addAttribute( nodename, DataTree( QVariant()));
			}
			else if (*is == '\'' || *is == '\"')
			{
				nodevalue = parseString( is, es);
				rt.addAttribute( nodename, DataTree( QVariant( nodevalue)));
			}
			else if (*is == '{')
			{
				QString::const_iterator start = is;
				skipBrk( is, es);
				if (is != es) is++;
				rt.addAttribute( nodename, DataTree( QVariant( QString( start, is-start))));
			}
			else
			{
				QString::const_iterator start = is;
				for (; is != es && *is != ' '; ++is);
				rt.addAttribute( nodename, DataTree( QVariant( QString( start, is-start))));
			}
			if (is == es) break;
		}
		else
		{
			value.push_back( *is);
		}
	}
	QString rest = value.trimmed();
	if (!rest.isEmpty())
	{
		rt.setNodeValue( QVariant( rest));
	}
	return rt;
}

DataTree DataTree::fromString( const QString& str)
{
	return fromString( str.begin(), str.end());
}

QString DataTree::toString() const
{
	/*[-]*////HIE WIITER
}


