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
#include <QXmlStreamWriter>
#include <QVariant>

static QByteArray getWigdetRequest_( WidgetVisitor& visitor, WidgetVisitor& dataroot, bool debugmode)
{
	QList<QByteArray> props = visitor.widget()->dynamicPropertyNames();
	QString docType,rootElement;
	bool isStandalone = true;
	QList<QByteArray> selectedDataElements;
	bool hasSelectedDataElements = false;

	if (props.contains("dataelement"))
	{
		selectedDataElements = visitor.property( "dataelement").toByteArray().split( ',');
		hasSelectedDataElements = true;
	}
	if (props.contains("doctype"))
	{
		docType = visitor.property( "doctype").toString();
		isStandalone = false;
	}
	if (props.contains("rootelement"))
	{
		rootElement = visitor.property( "rootelement").toString();
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
	QByteArray rt;
	QXmlStreamWriter xml( &rt);
	QList<WidgetVisitor::Element> elements;
	if (hasSelectedDataElements)
	{
		elements = dataroot.elements( selectedDataElements);
	}
	else
	{
		elements = dataroot.elements();
	}
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
	QList<WidgetVisitor::Element>::const_iterator ie = elements.begin(), ee = elements.end();
	for (; ie != ee; ++ie)
	{
		QVariant attribute;
		/*[-]*/qDebug() << "ELEMENT" << ie->toString();
		switch (ie->type())
		{
			case WidgetVisitor::Element::OpenTag:
				xml.writeStartElement( ie->value().toString());
				break;

			case WidgetVisitor::Element::CloseTag:
				xml.writeEndElement();
				break;

			case WidgetVisitor::Element::Attribute:
				attribute = ie->value();
				++ie;
				if (ie == ee || ie->type() != WidgetVisitor::Element::Value)
				{
					qCritical() << "producing illegal XML";
					return QByteArray();
				}
				xml.writeAttribute( attribute.toString(), ie->value().toString());
				break;

			case WidgetVisitor::Element::Value:
				xml.writeCharacters( ie->value().toString());
				break;
		}
	}

	xml.writeEndElement();
	xml.writeEndDocument();
	return rt;
}

QByteArray getActionRequest( WidgetVisitor& visitor, bool debugmode)
{
	QByteArray rt;
	QVariant rootElement;
	rootElement = visitor.property( "rootelement");
	if (rootElement.isValid())
	{
		WidgetVisitor dataroot = visitor.getRootElement( rootElement.toByteArray());
		if (!dataroot.widget())
		{
			qCritical() << "action rootelement does not address a known widget:" << rootElement;
			return rt;
		}
		rt = getWigdetRequest_( visitor, dataroot, debugmode);
		qDebug() << "widget request of " << dataroot.objectName() << "=" << rt;
		visitor.leave();
	}
	else
	{
		rt = getWigdetRequest_( visitor, visitor, debugmode);
	}
	return rt;
}

QByteArray getWigdetRequest( WidgetVisitor& visitor, bool debugmode)
{
	return getWigdetRequest_( visitor, visitor, debugmode);
}

struct WidgetAnswerStackElement
{
	QByteArray name;
	QString tok;
	bool istag;

	explicit WidgetAnswerStackElement( const QXmlStreamReader& xml, bool istag_)
		:name(xml.name().toString().toAscii())
		,tok()
		,istag(istag_)
	{}

	WidgetAnswerStackElement( const WidgetAnswerStackElement& o)
		:name( o.name)
		,tok( o.tok)
		,istag(o.istag)
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


bool setWidgetAnswer( WidgetVisitor& visitor, const QByteArray& answer)
{
	QList<WidgetAnswerStackElement> stk;
	QXmlStreamReader xml( answer);
	int taglevel = 0;

	qDebug( ) << "feeding widget " << visitor.objectName() << " with XML:\n" << answer;

	for (xml.readNext(); !xml.atEnd(); xml.readNext())
	{
		if (xml.isStartElement())
		{
			++taglevel;
			if (taglevel == 1)
			{
				//... ignore XML root element
				continue;
			}
			QString tagname = xml.name().toString();
			if (!stk.isEmpty() && !stk.last().istag)
			{
				XMLERROR( xml, stk, QString( "element not defined: '") + stk.last().name + "/" + tagname + "'");
				return false;
			}
			bool istag = visitor.enter( tagname, true);
			stk.push_back( WidgetAnswerStackElement( xml, istag));
			QXmlStreamAttributes attributes = xml.attributes();
			if (istag)
			{
				QXmlStreamAttributes::const_iterator ai = attributes.begin(), ae = attributes.end();
				for (; ai != ae; ++ai)
				{
					if (!visitor.setProperty( ai->name().toString(), ai->value().toString()))
					{
						XMLERROR( xml, stk, QString( "failed to set property '") + ai->name().toString() + "'");
					}
				}
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
				if (!tokIsEmpty && !visitor.setProperty( QByteArray(""), stk.last().tok))
				{
					XMLERROR( xml, stk, "failed to set content element");
				}
				visitor.leave();
			}
			else
			{
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
	return true;
}




