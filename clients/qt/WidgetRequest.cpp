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
#include "WidgetRequest.hpp"
#include "WidgetVisitor.hpp"
#include "DebugTerminal.hpp"
#include "DataSerializeItem.hpp"
#include "DataTreeSerialize.hpp"
#include <QXmlStreamWriter>
#include <QVariant>
#undef WOLFRAME_LOWLEVEL_DEBUG
#ifdef WOLFRAME_LOWLEVEL_DEBUG
#define TRACE_VALUE( TITLE, VALUE)			qDebug() << "widget answer XML " << (TITLE) << (VALUE);
#define TRACE_ASSIGNMENT( TITLE, NAME, VALUE)		qDebug() << "widget answer XML " << (TITLE) << (NAME) << "=" << (VALUE);
#else
#define TRACE_VALUE( TITLE, VALUE)
#define TRACE_ASSIGNMENT( TITLE, NAME, VALUE)
#endif

static QByteArray getWigdetRequest_( WidgetVisitor& visitor, bool debugmode)
{
	QVariant prop;
	QString docType,rootElement;
	bool isStandalone = true;
	QList<QString> selectedDataElements;
	bool hasSelectedDataElements = false;
	QList<DataSerializeItem> elements;
	QWidget* widget = visitor.widget();

	if (!widget)
	{
		qCritical() << "Invalid request (no widget defined)";
		return QByteArray();
	}
	QVariant action_v = widget->property( "action");
	if (action_v.isValid())
	{
		ActionDefinition action( action_v.toString());
		docType = action.doctype();
		rootElement = action.rootelement();
		isStandalone = rootElement.isEmpty();
		if (!action.isValid())
		{
			qCritical() << "Invalid request for doctype" << docType << "root" << rootElement;
			return QByteArray();
		}
		elements = getWidgetDataSerialization( action.structure(), visitor.widget());
	}
	else
	{
		QVariant dataelement_v = visitor.property( "dataelement");
		if (dataelement_v.isValid())
		{
			foreach (const QString& ee, dataelement_v.toString().trimmed().split( ','))
			{
				QString elem = ee.trimmed();
				if (!elem.isEmpty())
				{
					if (elem[0] == '_' && elem.size() == 1)
					{
						selectedDataElements.push_back( QString());
					}
					else
					{
						selectedDataElements.push_back( elem);
					}
				}
			}
			hasSelectedDataElements = true;
		}
		if ((prop = visitor.property("doctype")).isValid())
		{
			docType = prop.toString();
			isStandalone = false;
		}
		if ((prop = visitor.property("rootelement")).isValid())
		{
			rootElement = prop.toString();
			isStandalone = false;
		}
		if (!isStandalone && rootElement.isEmpty())
		{
			rootElement = docType;
		}
		if (!isStandalone && docType.isEmpty())
		{
			docType = rootElement;
		}
		QVariant dataobjectname = visitor.property( "dataobject");
		if (dataobjectname.isValid() && !visitor.enter( dataobjectname.toString(), false))
		{
			qCritical() << "action dataobject does not address a known widget:" << dataobjectname;
			return QByteArray();
		}
		if (hasSelectedDataElements)
		{
			elements = visitor.elements( selectedDataElements);
		}
		else
		{
			elements = visitor.elements();
		}
		if (dataobjectname.isValid())
		{
			visitor.leave( false);
		}
	}
	QByteArray rt;
	QXmlStreamWriter xml( &rt);
	if (debugmode)
	{
		xml.setAutoFormatting( true);
		xml.setAutoFormattingIndent( 2);
	}
	xml.writeStartDocument( "1.0", isStandalone);
	if (!isStandalone)
	{
		if (rootElement == docType)
		{
			xml.writeDTD( QString( "<!DOCTYPE %1>").arg( docType));
		}
		else
		{
			xml.writeDTD( QString( "<!DOCTYPE %1 SYSTEM '%2'>").arg( rootElement).arg( docType));
		}
	}

	xml.writeStartElement( rootElement);
	QList<DataSerializeItem>::const_iterator ie = elements.begin(), ee = elements.end();
	for (; ie != ee; ++ie)
	{
		QVariant attribute;
		switch (ie->type())
		{
			case DataSerializeItem::OpenTag:
				xml.writeStartElement( ie->value().toString());
				break;

			case DataSerializeItem::CloseTag:
				xml.writeEndElement();
				break;

			case DataSerializeItem::Attribute:
				attribute = ie->value();
				++ie;
				if (ie == ee || ie->type() != DataSerializeItem::Value)
				{
					qCritical() << "producing illegal XML";
					return QByteArray();
				}
				xml.writeAttribute( attribute.toString(), ie->value().toString());
				break;

			case DataSerializeItem::Value:
				xml.writeCharacters( ie->value().toString());
				break;
		}
	}
	xml.writeEndElement();
	xml.writeEndDocument();
	return rt;
}

bool isActionRequest( const QString& tag)
{
	return !tag.isEmpty() && tag.at(0) == '-';
}

QString actionRequestRecipientId( const QString& tag)
{
	if (tag.isEmpty()) return QString();
	return tag.mid( 1, tag.size()-1);
}

QPair<QString,QByteArray> getActionRequest( WidgetVisitor& visitor, bool debugmode)
{
	QPair<QString,QByteArray> rt;
	rt.second = getWigdetRequest_( visitor, debugmode);
	rt.first = QString("-") + visitor.widgetid();
	qDebug() << "action request of " << visitor.objectName() << "=" << rt.first << ":" << rt.second;
	return rt;
}

QByteArray getWigdetRequest( WidgetVisitor& visitor, bool debugmode)
{
	QByteArray rt = getWigdetRequest_( visitor, debugmode);
	qDebug() << "widget request of " << visitor.objectName() << "=" << rt;
	return rt;
}

struct WidgetAnswerStackElement
{
	QString name;
	QString tok;
	bool istag;
	bool ischild;

	explicit WidgetAnswerStackElement( const QXmlStreamReader& xml, bool istag_, bool ischild_)
		:name(xml.name().toString())
		,tok()
		,istag(istag_)
		,ischild(ischild_)
	{}

	WidgetAnswerStackElement( const WidgetAnswerStackElement& o)
		:name( o.name)
		,tok( o.tok)
		,istag(o.istag)
		,ischild(o.ischild)
	{}
};

static void XMLERROR( const QXmlStreamReader& xml, const QList<WidgetAnswerStackElement>& stk, const QString& message)
{
	QXmlStreamAttributes attributes;
	QString path;
	QList<WidgetAnswerStackElement>::const_iterator pi = stk.begin(), pe = stk.end();
	for (; pi != pe; ++pi)
	{
		path.append( "/");
		path.append( pi->name);
	}
	qCritical() << (message.isEmpty()?xml.errorString():message)
		<< "in XML at line " << xml.lineNumber() << ", column " << xml.columnNumber()
		<< " path " << path;
}

static void setAttributes( WidgetVisitor& visitor, const QList<WidgetAnswerStackElement>& stk, const QXmlStreamReader& xml, const QXmlStreamAttributes& attributes)
{
	QXmlStreamAttributes::const_iterator ai = attributes.begin(), ae = attributes.end();
	for (; ai != ae; ++ai)
	{
		TRACE_ASSIGNMENT( "ATTRIBUTE", ai->name().toString(), ai->value().toString());
		if (!visitor.setProperty( ai->name().toString(), ai->value().toString()))
		{
			XMLERROR( xml, stk, QString( "failed to set property '") + ai->name().toString() + "'");
		}
	}
}

bool setWidgetAnswer( WidgetVisitor& visitor, const QByteArray& answer)
{
	QList<WidgetAnswerStackElement> stk;
	QXmlStreamReader xml( answer);
	int taglevel = 0;
	visitor.resetState();

	qDebug() << "feeding widget " << visitor.objectName() << "with XML";

	for (xml.readNext(); !xml.atEnd(); xml.readNext())
	{
		if (xml.isStartElement())
		{
			++taglevel;
			if (taglevel == 1)
			{
				setAttributes( visitor, stk, xml, xml.attributes());
				//... ignore XML root element but set attributes
				continue;
			}
			QString tagname = xml.name().toString();
			TRACE_VALUE( "OPEN TAG", tagname);
			if (!stk.isEmpty() && !stk.last().istag)
			{
				XMLERROR( xml, stk, QString( "element not defined: '") + stk.last().name + "/" + tagname + "'");
				return false;
			}
			QWidget* prev_widget = visitor.widget();
			bool istag = visitor.enter( tagname, true);
			bool ischild = (prev_widget != visitor.widget());
			stk.push_back( WidgetAnswerStackElement( xml, istag, ischild));

			QXmlStreamAttributes attributes = xml.attributes();
			if (istag)
			{
				if (ischild) visitor.resetState();
				setAttributes( visitor, stk, xml, attributes);
			}
			else
			{
				if (!attributes.isEmpty())
				{
					XMLERROR( xml, stk, QString( "substructure not defined: '") + tagname + "'");
				}
			}
		}
		else if (xml.isEndElement())
		{
			--taglevel;
			if (taglevel == 0)
			{
				//... root element ignored. so is the end element belonging to root element.
				continue;
			}
			TRACE_VALUE( "CLOSE TAG", "");
			if (stk.isEmpty())
			{
				XMLERROR( xml, stk, QString( "unexpected end element: XML tags not balanced"));
				return false;
			}
			QString::const_iterator ti = stk.last().tok.begin(), te = stk.last().tok.end();
			for (; ti != te && ti->isSpace(); ++ti);
			bool tokIsEmpty = (ti == te);
			if (stk.last().istag)
			{
				TRACE_VALUE( "CONTENT", stk.last().tok);
				if (!tokIsEmpty && !visitor.setProperty( "", stk.last().tok))
				{
					XMLERROR( xml, stk, "failed to set content element");
				}
				if (stk.last().ischild)
				{
					visitor.endofDataFeed();
					visitor.restoreState();
				}
				visitor.leave( true);
			}
			else
			{
				TRACE_ASSIGNMENT( "PROPERTY", stk.last().name, stk.last().tok);
				if (!tokIsEmpty && !visitor.setProperty( stk.last().name, stk.last().tok))
				{
					XMLERROR( xml, stk, QString( "failed to set property '") + stk.last().name + "'");
				}
			}
			stk.removeLast();
		}
		else if (xml.isEntityReference())
		{
			XMLERROR( xml, stk, QString( "unexpected entity reference in content element: no entity references supported"));
			return false;
		}
		else if (xml.isCDATA() || xml.isCharacters() || xml.isWhitespace())
		{
			if (stk.isEmpty())
			{
				XMLERROR( xml, stk, QString( "unexpected content element: no XML tag context defined"));
				return false;
			}
			stk.last().tok.append( xml.text());
		}
	}
	visitor.endofDataFeed();
	visitor.restoreState();
	return true;
}





