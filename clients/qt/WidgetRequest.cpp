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

QString WidgetRequest::actionWidgetRequestTag( QString recipientid_)
{
	return QString("-") + recipientid_;
}

QString WidgetRequest::actionWidgetRequestTag( QString recipientid_, QString followform_)
{
	return QString("-") + recipientid_ + "-" + followform_;
}

QString WidgetRequest::domainLoadWidgetRequestTag( QString recipientid_)
{
	return recipientid_;
}

WidgetRequest::Type WidgetRequest::type() const
{
	if (tag.size() && tag.at(0) == '-') return Action;
	return DomainLoad;
}

QString WidgetRequest::recipientid() const
{
	if (type() == Action)
	{
		int formidx = tag.indexOf( '-', 1);
		if (formidx < 0)
		{
			return tag.mid( 1, tag.size()-1);
		}
		else
		{
			return tag.mid( 1, formidx-1);
		}
	}
	return tag;
}

QString WidgetRequest::followform() const
{
	int formidx = tag.indexOf( '-', 1);
	if (formidx < 0)
	{
		return "";
	}
	else
	{
		return tag.mid( formidx+1, tag.size()-formidx-1);
	}
}

static QByteArray getRequestXML( const QString& docType, const QString& rootElement, bool isStandalone, const QList<DataSerializeItem>& elements, bool debugmode)
{
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

static QByteArray getWidgetRequest_( WidgetVisitor& visitor, const QVariant& actiondef, bool debugmode)
{
	QWidget* widget = visitor.widget();
	QString actionstr( actiondef.toString());
	ActionDefinition action( actionstr);
	qDebug() << "checking action condition properties" << action.condProperties();
	foreach (const QString& cond, action.condProperties())
	{
		if (!visitor.property( cond).isValid())
		{
			// one of the preconditions is not met, return empty (no) request
			qDebug() << "suppressing action" << actionstr << "because condition" << cond << "is not met (condition not valid)";
			return QByteArray();
		}
	}
	QString docType = action.doctype();
	QString rootElement = action.rootelement();
	bool isStandalone = rootElement.isEmpty();
	if (!action.isValid())
	{
		qCritical() << "invalid request for action doctype=" << docType << "root=" << rootElement;
		return QByteArray();
	}
	QList<DataSerializeItem> elements = getWidgetDataSerialization( action.structure(), widget);
	return getRequestXML( docType, rootElement, isStandalone, elements, debugmode);
}

WidgetRequest getActionRequest( WidgetVisitor& visitor, bool debugmode)
{
	WidgetRequest rt;
	QWidget* widget = visitor.widget();
	if (!widget)
	{
		qCritical() << "invalid request (no widget defined)";
		return rt;
	}
	QVariant action_v = widget->property( "action");
	if (!action_v.isValid())
	{
		qCritical() << "undefined request. action (property action) does not exist in" << visitor.className() << visitor.objectName();
		return rt;
	}
	rt.content = getWidgetRequest_( visitor, action_v, debugmode);
	rt.tag = WidgetRequest::actionWidgetRequestTag( visitor.widgetid());
	qDebug() << "action request of " << visitor.objectName() << "=" << rt.tag << ":" << rt.content;
	return rt;
}

WidgetRequest getMenuActionRequest( WidgetVisitor& visitor, const QString& menuitem, bool debugmode)
{
	WidgetRequest rt;
	QWidget* widget = visitor.widget();
	if (!widget)
	{
		qCritical() << "menu action on non existing widget" << menuitem;
		return rt;
	}
	QByteArray propname = QByteArray("action:") + menuitem.toAscii();
	QVariant action_v = widget->property( propname);
	if (!action_v.isValid())
	{
		qCritical() << "menu item action (property" << propname << ") does not exist for" << menuitem << "in" << visitor.className() << visitor.objectName();
		return rt;
	}
	rt.content = getWidgetRequest_( visitor, action_v, debugmode);
	rt.tag = WidgetRequest::actionWidgetRequestTag( visitor.widgetid(), menuitem);
	qDebug() << "action request of " << visitor.objectName() << "=" << rt.tag << ":" << rt.content;
	return rt;
}

WidgetRequest getWidgetRequest( WidgetVisitor& visitor, bool debugmode)
{
	WidgetRequest rt;
	QWidget* widget = visitor.widget();
	if (!widget)
	{
		qCritical() << "request on non existing widget";
		return rt;
	}
	QVariant action_v = widget->property( "action");
	if (!action_v.isValid())
	{
		qCritical() << "undefined request. action (property action) does not exist in" << visitor.className() << visitor.objectName();
		return rt;
	}
	rt.tag = WidgetRequest::domainLoadWidgetRequestTag( visitor.widgetid());
	rt.content = getWidgetRequest_( visitor, action_v, debugmode);
	qDebug() << "widget request of " << visitor.objectName() << "=" << rt.tag << ":" << rt.content;
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





