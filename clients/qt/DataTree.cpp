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
#include <QDebug>
#define WOLFRAME_LOWLEVEL_DEBUG
#ifdef WOLFRAME_LOWLEVEL_DEBUG
#define TRACE_PARSE_ERROR( TITLE, LINE)			qDebug() << "data tree parse error" << (TITLE) << "at line" << (LINE);
#define TRACE_PARSE_STATE( TITLE)			qDebug() << "data tree parser state" << (TITLE);
#define TRACE_PARSE_ITEM( TITLE, VALUE)			qDebug() << "data tree parser fetch item" << (TITLE) << (VALUE);
#define TRACE_PARSE_OBJECT( TITLE, VALUE)		qDebug() << "data tree parser create object" << (TITLE) << (VALUE);
#else
#define TRACE_PARSE_ERROR( TITLE, LINE)
#define TRACE_PARSE_STATE( TITLE)
#define TRACE_PARSE_ITEM( TITLE, VALUE)
#define TRACE_PARSE_OBJECT( TITLE, VALUE)
#endif

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
				ni->value = QSharedPointer<DataTree>( new DataTree( value_));
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
	return m_elemtype != Invalid;
}

QString DataTree::parseString( QString::const_iterator& itr, const QString::const_iterator& end)
{
	QChar eb = *itr;
	QString::const_iterator start;
	for (start=++itr; itr != end && *itr != eb; ++itr);
	QString rt( start, itr-start);
	if (itr != end) ++itr;
	return rt;
}

static void skipBrk( QString::const_iterator& itr, const QString::const_iterator& end)
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

DataTree::ElementType DataTree::parseNodeHeader( QString& nodename, QString::const_iterator& itr, const QString::const_iterator& end)
{
	for (; itr != end && itr->isLetter(); ++itr)
	{
		nodename.push_back( *itr);
	}
	for (++itr; itr != end && itr->isSpace(); ++itr);

	if (*itr == '[')
	{
		QString::const_iterator obrk = itr;
		for (++itr; itr != end && itr->isSpace(); ++itr);
		if (itr != end && *itr == ']')
		{
			++itr;
			return Array;
		}
		else
		{
			itr = obrk;
			return Invalid;
		}
	}
	else
	{
		return Single;
	}
}

static void skipSpaces( QString::const_iterator& itr, const QString::const_iterator& end)
{
	for (++itr; itr != end && itr->isSpace(); ++itr);
}

DataTree DataTree::fromString( const QString::const_iterator& begin, const QString::const_iterator& end)
{
	DataTree rt( Single);
	QString::const_iterator is = begin, es = end;

	while (is != es)
	{
		skipSpaces( is, es);

		if (is->isLetter())
		{
			TRACE_PARSE_ITEM( "letter", *is);
			QString nodename;
			ElementType elemtype = parseNodeHeader( nodename, is, es);
			if (elemtype == Invalid)
			{
				TRACE_PARSE_ERROR( "invalid tree", (int)__LINE__)
				return DataTree( Invalid);
			}
			skipSpaces( is, es);

			if (is == es)
			{
				rt.setNodeValue( QVariant( nodename));
			}
			else if (*is == '{')
			{
				TRACE_PARSE_STATE( "open node bracket");
				QString::const_iterator start = is+1;
				skipBrk( is, es);
				DataTree elem( fromString( start, is));
				if (elem.m_elemtype == Invalid)
				{
					TRACE_PARSE_ERROR( "invalid tree", (int)__LINE__)
					return DataTree( Invalid);
				}
				elem.m_elemtype = elemtype;
				rt.addNode( nodename, elem);
				if (is != es) ++is;
			}
			else if (*is == '=' && elemtype == Single)
			{
				TRACE_PARSE_STATE( "assignment");
				++is; skipSpaces( is, es);
				if (is == es)
				{
					TRACE_PARSE_ERROR( "invalid tree", (int)__LINE__)
					return DataTree( Invalid);
				}

				if (*is == '\'' || *is == '\"')
				{
					rt.addAttribute( nodename, DataTree( QVariant( parseString( is, es))));
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
					skipSpaces( is, es);
					rt.addAttribute( nodename, DataTree( QVariant( QString( start, is-start))));
				}
				if (is == es) break;
			}
			else
			{
				TRACE_PARSE_OBJECT( "node value", nodename);
				rt.setNodeValue( QVariant( nodename));
			}
		}
		else if (*is == '\'' || *is == '\"')
		{
			TRACE_PARSE_STATE( "open string");
			QString nodevalue = parseString( is, es);
			rt.setNodeValue( QVariant( nodevalue));
		}
		else if (*is == '{')
		{
			TRACE_PARSE_STATE( "open value bracket");
			QString::const_iterator start = ++is;
			skipBrk( is, es);
			QString nodevalue;
			nodevalue.push_back( '{');
			nodevalue.append( QString( start, is-start).trimmed());
			nodevalue.push_back( '}');
			rt.setNodeValue( nodevalue);
			if (is != es) ++is;
		}
		skipSpaces( is, es);
		if (is != es)
		{
			TRACE_PARSE_ITEM( "char", *is);
			if (*is == ';')
			{
				TRACE_PARSE_STATE( "node delimiter (semicolon)");
				++is;
			}
			else
			{
				TRACE_PARSE_ERROR( "invalid tree", (int)__LINE__)
				return DataTree( Invalid);
			}
		}
	}
	return rt;
}

DataTree DataTree::fromString( const QString& str)
{
	return fromString( str.begin(), str.end());
}

void DataTree::mapDataValueToString( const QVariant& val, QString& str)
{
	QString valstr = val.toString();
	QString::const_iterator vi = valstr.begin(), ve = valstr.end();
	skipSpaces( vi, ve);
	if (vi == ve)
	{
		str.append( valstr);
	}
	if (valstr.indexOf('\"') < 0)
	{
		str.push_back( '\"');
		str.append( valstr);
		str.push_back( '\"');
	}
	else if (valstr.indexOf('\'') < 0)
	{
		str.push_back( '\'');
		str.append( valstr);
		str.push_back( '\'');
	}
	else
	{
		str.push_back( '{');
		str.append( valstr);
		str.push_back( '}');
	}
}

void DataTree::mapDataTreeToString( const DataTree& dt, QString& str)
{
	if (dt.m_value.isValid())
	{
		mapDataValueToString( dt.m_value, str);
	}
	else
	{
		QList<DataTree::Node>::const_iterator ni = dt.m_nodear.begin(), ne = dt.m_nodear.end();
		for (; ni != ne; ++ni)
		{
			str.append( ni->name);
			if (ni - dt.m_nodear.begin() < dt.m_nofattributes)
			{
				str.append( " = ");
				QString valstr;
				mapDataTreeToString( *ni->value, valstr);
				mapDataValueToString( valstr, valstr);
			}
			else
			{
				str.append( " { ");
				mapDataTreeToString( *ni->value, str);
				str.append( " } ");
			}
		}
	}
}

QString DataTree::toString() const
{
	QString rt;
	mapDataTreeToString( *this, rt);
	return rt;
}

ActionDefinition::ActionDefinition( const QString& str)
{
	QString::const_iterator itr = str.begin(), end = str.end();
	for (; itr != end && itr->isLetter(); ++itr)
	m_doctype = QString( str.begin(), itr-str.begin());
	skipSpaces( itr, end);
	if (itr != end && itr->isLetter())
	{
		QString::const_iterator rootstart = itr;
		for (; itr != end && itr->isLetter(); ++itr);
		m_rootelement = QString( rootstart, itr-rootstart);
		skipSpaces( itr, end);
	}
	if (itr != end && *itr == '{')
	{
		++itr;
		QString::const_iterator start = itr;
		skipBrk( itr, end);
		m_structure = DataTree::fromString( start, itr);
	}
}

QString ActionDefinition::toString() const
{
	QString rt;
	rt.append( m_doctype);
	rt.push_back( ' ');
	rt.append( m_rootelement);
	rt.push_back( ' ');
	rt.push_back( '{');
	rt.append( m_structure.toString());
	rt.push_back( '}');
	return rt;
}



