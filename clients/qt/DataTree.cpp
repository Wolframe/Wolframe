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
#define TRACE_ERROR( COMP, TITLE, LINE)			qDebug() << "DataTree" << (COMP) << "error" << (TITLE) << "at line" << (LINE);
#define TRACE_STATE( COMP, TITLE)			qDebug() << "DataTree" << (COMP) << "state" << (TITLE);
#define TRACE_ITEM( COMP, TITLE, VALUE)			qDebug() << "DataTree" << (COMP) << "item" << (TITLE) << (VALUE);
#define TRACE_OBJECT( COMP, TITLE, VALUE)		qDebug() << "DataTree" << (COMP) << "object" << (TITLE) << (VALUE);
#else
#define TRACE_ERROR( COMP, TITLE, LINE)
#define TRACE_STATE( COMP, TITLE)
#define TRACE_ITEM( COMP, TITLE, VALUE)
#define TRACE_OBJECT( COMP, TITLE, VALUE)
#endif

static bool isAlphaNum( QChar ch)
{
	return ch.isLetter() || ch == '_' || ch.isDigit();
}

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

void DataTree::setNodeValue( const QVariant& value_)
{
	if (m_value.isValid())
	{
		if (m_value.type() == QVariant::List)
		{
			QList<QVariant> lst = m_value.toList();
			lst.push_back( value_);
			m_value = QVariant( lst);
		}
		else
		{
			QList<QVariant> lst;
			lst.push_back( m_value);
			lst.push_back( value_);
			m_value = QVariant( lst);
		}
	}
	else
	{
		m_value = QVariant( value_);
	}
}

static void skipSpaces( QString::const_iterator& itr, const QString::const_iterator& end)
{
	for (; itr != end && itr->isSpace(); ++itr);
}

static void skipNonSpaces( QString::const_iterator& itr, const QString::const_iterator& end)
{
	for (; itr != end && !itr->isSpace() && *itr != ';'; ++itr);
}

DataTree::ElementType DataTree::parseNodeHeader( QString& nodename, QString::const_iterator& itr, const QString::const_iterator& end)
{
	for (; itr != end && isAlphaNum(*itr); ++itr)
	{
		nodename.push_back( *itr);
	}
	skipSpaces( itr, end);

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

DataTree DataTree::fromString( const QString::const_iterator& begin, const QString::const_iterator& end)
{
	DataTree rt( Single);
	QString::const_iterator is = begin, es = end;
	TRACE_OBJECT( "fromString", "enter", QString(is,es-is))

	while (is != es)
	{
		skipSpaces( is, es);

		if (isAlphaNum(*is))
		{
			TRACE_ITEM( "fromString", "letter", *is);
			QString nodename;
			ElementType elemtype = parseNodeHeader( nodename, is, es);
			if (elemtype == Invalid)
			{
				TRACE_ERROR( "fromString", "invalid tree", (int)__LINE__)
				return DataTree( Invalid);
			}
			skipSpaces( is, es);
			TRACE_OBJECT( "fromString", "node name", nodename);

			if (is == es)
			{
				rt.setNodeValue( QVariant( nodename));
			}
			else if (*is == '{')
			{
				TRACE_STATE( "fromString", "open node bracket");
				QString::const_iterator start = is+1;
				skipBrk( is, es);
				DataTree elem( fromString( start, is));
				if (elem.m_elemtype == Invalid)
				{
					TRACE_ERROR( "fromString", "invalid tree", (int)__LINE__)
					return DataTree( Invalid);
				}
				elem.m_elemtype = elemtype;
				rt.addNode( nodename, elem);
				if (is != es)
				{
					++is;
				}
			}
			else if (*is == '=')
			{
				if (elemtype != Single)
				{
					TRACE_ERROR( "fromString", "invalid tree", (int)__LINE__)
					return DataTree( Invalid);
				}
				TRACE_STATE( "fromString", "assignment");
				++is; skipSpaces( is, es);
				if (is == es)
				{
					TRACE_ERROR( "fromString", "invalid tree", (int)__LINE__)
					return DataTree( Invalid);
				}
				if (*is == '\'' || *is == '\"')
				{
					QString content( parseString( is, es));
					TRACE_OBJECT( "fromString", "string attribute", content);
					rt.addAttribute( nodename, DataTree( QVariant( content)));
				}
				else if (*is == '{')
				{
					QString::const_iterator start = is;
					skipBrk( is, es);
					if (is != es) is++;
					TRACE_OBJECT( "fromString", "brk attribute", QString( start, is-start));
					rt.addAttribute( nodename, DataTree( QVariant( QString( start, is-start))));
				}
				else
				{
					QString::const_iterator start = is;
					skipNonSpaces( is, es);
					if (is == start)
					{
						TRACE_ERROR( "fromString", "invalid tree", (int)__LINE__)
						return DataTree( Invalid);
					}
					TRACE_OBJECT( "fromString", "nonspace attribute", QString( start, is-start));
					rt.addAttribute( nodename, DataTree( QVariant( QString( start, is-start))));
				}
				if (is == es) break;
			}
			else
			{
				TRACE_OBJECT( "fromString", "node value", nodename);
				rt.setNodeValue( QVariant( nodename));
			}
		}
		else if (*is == '\'' || *is == '\"')
		{
			TRACE_STATE( "fromString", "open string");
			QString nodevalue = parseString( is, es);
			rt.setNodeValue( QVariant( nodevalue));
		}
		else if (*is == '{')
		{
			TRACE_STATE( "fromString", "open value bracket");
			QString::const_iterator start = ++is;
			skipBrk( is, es);
			QString nodevalue;
			nodevalue.push_back( '{');
			nodevalue.append( QString( start, is-start).trimmed());
			nodevalue.push_back( '}');
			TRACE_OBJECT( "fromString", "curly bracket value", nodevalue);
			rt.setNodeValue( nodevalue);
			if (is != es)
			{
				++is;
			}
		}
		skipSpaces( is, es);
		if (is != es)
		{
			if (*is == ';' || *is == ',')
			{
				TRACE_STATE( "fromString", "node delimiter (semicolon or comma)");
				++is;
			}
			else
			{
				TRACE_ITEM( "fromString", "delimiter char", *is)
				TRACE_ERROR( "fromString", "invalid tree", (int)__LINE__)
				return DataTree( Invalid);
			}
		}
	}
	TRACE_STATE( "fromString", "return")
	return rt;
}

DataTree DataTree::fromString( const QString& str)
{
	return fromString( str.begin(), str.end());
}

bool DataTree::mapDataValueToString( const QVariant& val, QString& str)
{
	TRACE_ITEM( "toString", "value variant", val)
	QString valstr = val.toString();
	QString::const_iterator vi = valstr.begin(), ve = valstr.end();
	int brkcnt=0;
	for (; vi != ve; ++vi)
	{
		if (*vi == '}') --brkcnt;
		if (*vi == '{') ++brkcnt;
	}
	if (brkcnt != 0) return false;

	vi = valstr.begin(), ve = valstr.end();
	skipNonSpaces( vi, ve);

	bool hasSQuotes = (valstr.indexOf('\'') >= 0);
	bool hasDQuotes = (valstr.indexOf('\"') >= 0);

	if (vi == ve && !hasSQuotes && !hasDQuotes)
	{
		TRACE_OBJECT( "toString", "value without spaces", valstr)
		str.append( valstr);
	}
	else if (!hasDQuotes)
	{
		TRACE_OBJECT( "toString", "value without double quotes", valstr)
		str.push_back( '\"');
		str.append( valstr);
		str.push_back( '\"');
	}
	else if (!hasSQuotes)
	{
		TRACE_OBJECT( "toString", "value without single quotes", valstr)
		str.push_back( '\'');
		str.append( valstr);
		str.push_back( '\'');
	}
	else
	{
		TRACE_OBJECT( "toString", "value in curly brackets", valstr)
		str.push_back( '{');
		str.append( valstr);
		str.push_back( '}');
	}
	return true;
}

bool DataTree::mapDataTreeToString( const DataTree& dt, QString& str)
{
	if (dt.m_value.isValid())
	{
		TRACE_STATE( "toString", "map node value")
		return mapDataValueToString( dt.m_value, str);
	}
	QList<DataTree::Node>::const_iterator ni = dt.m_nodear.begin(), ne = dt.m_nodear.end();
	for (; ni != ne; ++ni)
	{
		if (ni != dt.m_nodear.begin())
		{
			str.append( "; ");
		}
		TRACE_OBJECT( "toString", "node name", ni->name)
		str.append( ni->name);
		if (ni - dt.m_nodear.begin() < dt.m_nofattributes)
		{
			TRACE_STATE( "toString", "attribute")
			str.append( " = ");
			if (!mapDataTreeToString( *ni->value, str)) return false;
		}
		else
		{
			TRACE_STATE( "toString", "content")
			if (ni->value->m_elemtype == Array)
			{
				str.append( "[]");
			}
			str.append( " { ");
			if (!mapDataTreeToString( *ni->value, str)) return false;
			str.append( " }");
		}
	}
	return true;
}

QString DataTree::toString() const
{
	QString rt;
	if (!mapDataTreeToString( *this, rt))
	{
		qCritical() << "Failed to map tree to string (tree data may contain non escapable characters)";
	}
	return rt;
}

static QList<QString> getConditionProperties( const QString& str)
{
	bool isValue = false;
	QList<QString> rt;

	QString::const_iterator itr = str.begin(), end = str.end();
	QString::const_iterator start = end;
	for (; itr != end; ++itr)
	{
		if (*itr == '{')
		{
			++itr;
			skipSpaces( itr, end);
			if (*itr == '{') isValue = true;
			start = itr;
		}
		else if (*itr == '}')
		{
			if (start != end && isValue)
			{
				rt.push_back( QString( start, itr-start).trimmed());
			}
			isValue = false;
		}
		else if (*itr == '=')
		{
			isValue = true;
		}
	}
	return rt;
}

ActionDefinition::ActionDefinition( const QString& str)
{
	m_condProperties = getConditionProperties( str);

	QString::const_iterator itr = str.begin(), end = str.end();
	skipSpaces( itr, end);
	for (; itr != end && isAlphaNum(*itr); ++itr);
	m_doctype = QString( str.begin(), itr-str.begin());
	skipSpaces( itr, end);
	if (itr != end && isAlphaNum(*itr))
	{
		QString::const_iterator rootstart = itr;
		for (; itr != end && isAlphaNum(*itr); ++itr);
		m_rootelement = QString( rootstart, itr-rootstart);
		skipSpaces( itr, end);
	}
	if (itr != end && *itr == '{')
	{
		QString::const_iterator start = itr+1;
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


