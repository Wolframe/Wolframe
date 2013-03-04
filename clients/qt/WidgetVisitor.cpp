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
#include "WidgetVisitor.hpp"
#include "WidgetVisitor_QComboBox.hpp"
#include "WidgetVisitor_QListWidget.hpp"
#include "FileChooser.hpp"
#include "PictureChooser.hpp"

#include <QDebug>
#include <QByteArray>
#include <QXmlStreamWriter>
#include <QLineEdit>
#include <QComboBox>
#include <QPlainTextEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QListWidget>
#include <QTreeWidget>
#include <QTableWidget>

static WidgetVisitor::StateR widgetVisitorState( QWidget* widget)
{
	QString clazz = widget->metaObject()->className();
	if (clazz == "QComboBox")
	{
		return WidgetVisitor::StateR( new WidgetVisitorState_QComboBox( widget));
	}
	else if (clazz == "QListWidget")
	{
		return WidgetVisitor::StateR( new WidgetVisitorState_QListWidget( widget));
	}
	else
	{
		return WidgetVisitor::StateR( new WidgetVisitor::State( widget));
	}
}

WidgetVisitor::WidgetVisitor( QWidget* root, QHash<QByteArray,QVariant>* globals_)
	:m_globals(globals_)
{
	m_stk.push( widgetVisitorState( root));
}

WidgetVisitor::WidgetVisitor( const WidgetVisitor::StateR& state, QHash<QByteArray, QVariant>* globals_)
	:m_globals(globals_)
{
	m_stk.push( state);
}

bool WidgetVisitor::enter( const QString& name)
{
	return enter( name.toAscii());
}

bool WidgetVisitor::enter( const QByteArray& name)
{
	if (m_stk.empty()) return false;
	if (m_stk.top()->enter( name))
	{
		return true;
	}
	QWidget* wdg;
	if ((wdg = m_stk.top()->childwidget( name)) != 0)
	{
		///... child widget allocated
		m_stk.push( widgetVisitorState( wdg));
		return true;
	}
	return false;
}

void WidgetVisitor::leave()
{
	if (m_stk.empty()) return;
	if (!m_stk.top()->leave())
	{
		m_stk.pop();
	}
}

QVariant WidgetVisitor::property( const QString& name)
{
	return property( name.toAscii());
}

QVariant WidgetVisitor::property( const QByteArray& name, int level)
{
	if (m_stk.empty()) return QVariant()/*invalid*/;

	QVariant rt = m_stk.top()->property( name);
	if (rt.isValid()) return rt;

	int followidx = name.indexOf( '.');
	if (followidx >= 0)
	{
		if (level == 0 && name.startsWith( "global."))
		{
			return (*m_globals)[ name.mid( 7, name.size()-7)];
		}
		QByteArray prefix( name.mid( 0, followidx));
		QByteArray rest( name.mid( followidx+1, name.size()-followidx-1));
		if (enter( prefix))
		{
			rt = property( rest, level+1);
			leave();
			return rt;
		}
		if (level == 0 && m_stk.at(0)->widget()->objectName() == prefix)
		{
			WidgetVisitor rootvisit( m_stk.at(0), m_globals);
			return rootvisit.property( rest, 1);
		}
		QList<QByteArray> props = m_stk.top()->widget()->dynamicPropertyNames();
		QList<QByteArray>::const_iterator pi = props.begin(), pe = props.end();
		for (; pi != pe; ++pi)
		{
			if (pi->startsWith( "synonym:"))
			{
				if (name == pi->mid( 8, pi->size()-8))
				{
					QVariant synonym = m_stk.top()->widget()->property( *pi);
					if (synonym.type() == QVariant::String)
					{
						return property( synonym.toString());
					}
					else if (synonym.type() == QVariant::ByteArray)
					{
						return property( synonym.toByteArray());
					}
				}
			}
		}
	}
	return rt;
}

QVariant WidgetVisitor::property( const QByteArray& name)
{
	return property( name, 0);
}

bool WidgetVisitor::setProperty( const QByteArray& name, const QVariant& value, int level)
{
	if (m_stk.empty()) return false;
	if (m_stk.top()->setProperty( name, value)) return true;

	int followidx = name.indexOf( '.');
	if (followidx >= 0)
	{
		if (level == 0 && name.startsWith( "global."))
		{
			m_globals->insert( name.mid( 7, name.size()-7), value);
			return true;
		}
		QByteArray prefix( name.mid( 0, followidx));
		QByteArray rest( name.mid( followidx+1, name.size()-followidx-1));
		if (enter( prefix))
		{
			bool rt = setProperty( rest, value, level+1);
			leave();
			return rt;
		}
		if (level == 0 && m_stk.at(0)->widget()->objectName() == prefix)
		{
			WidgetVisitor rootvisit( m_stk.at(0), m_globals);
			return rootvisit.setProperty( rest, value, 1);
		}
		QList<QByteArray> props = m_stk.top()->widget()->dynamicPropertyNames();
		QList<QByteArray>::const_iterator pi = props.begin(), pe = props.end();
		for (; pi != pe; ++pi)
		{
			if (pi->startsWith( "synonym:"))
			{
				if (name == pi->mid( 8, pi->size()-8))
				{
					QVariant synonym = m_stk.top()->widget()->property( *pi);
					if (synonym.type() == QVariant::String)
					{
						return setProperty( synonym.toString(), value);
					}
					else if (synonym.type() == QVariant::ByteArray)
					{
						return setProperty( synonym.toByteArray(), value);
					}
				}
			}
		}
	}
	return false;
}

bool WidgetVisitor::setProperty( const QString& name, const QVariant& value)
{
	return setProperty( name.toAscii(), value, 0);
}

bool WidgetVisitor::setProperty( const QByteArray& name, const QVariant& value)
{
	return setProperty( name, value, 0);
}

struct WidgetVisitorStackElement
{
	const char** dataelements;
	int dataelementidx;
	QByteArray nameprefix;
	QList<QByteArray> selectedDataElements;
	bool isContent;

	WidgetVisitorStackElement()
		:dataelements(0)
		,dataelementidx(0)
		,nameprefix()
		,isContent(false)
	{}
	explicit WidgetVisitorStackElement( const WidgetVisitor::StateR& state, const QByteArray& nameprefix_=QByteArray())
		:dataelements(state->dataelements())
		,dataelementidx(0)
		,nameprefix(nameprefix_)
		,isContent(false)
	{
		QList<QByteArray> dprops = state->widget()->dynamicPropertyNames();
		if (dprops.contains( "dataelement"))
		{
			selectedDataElements = state->widget()->property( "dataelement").toByteArray().split( ',');
		}
	}
	WidgetVisitorStackElement( const WidgetVisitorStackElement& o)
		:dataelements( o.dataelements)
		,dataelementidx( o.dataelementidx)
		,nameprefix( o.nameprefix)
		,selectedDataElements(o.selectedDataElements)
		,isContent(o.isContent)
	{}
};

static bool isReservedProperty( const QByteArray& key)
{
	// skip _q_ dynamic properties, they are used by the Qt stylesheet engine
	if( key.startsWith( "_q_")) return true;
	// skip globals
	if( key.startsWith( "global.")) return true;
	// skip synonym declarations
	if( key.startsWith( "synonym:")) return true;
	// ignore our own actions
	if( key == "doctype" || key == "rootelement" || key == "dataelement" || key == "action" || key == "initAction" || key == "form" || key == "state")
	{
		return true;
	}
	return false;
}

QList<WidgetVisitor::Element> WidgetVisitor::elements()
{
	QList<WidgetVisitor::Element> rt;
	if (m_stk.isEmpty()) return rt;

	QStack<WidgetVisitorStackElement> elemstk;
	elemstk.push_back( WidgetVisitorStackElement( m_stk.top()));

	while (!m_stk.isEmpty() && !elemstk.isEmpty())
	{
		if (elemstk.top().dataelements && elemstk.top().dataelements[ elemstk.top().dataelementidx])
		{
			QByteArray elemname( elemstk.top().dataelements[ elemstk.top().dataelementidx]);
			if (!elemstk.top().selectedDataElements.isEmpty())
			{
				QByteArray dm;
				if (!elemstk.top().nameprefix.isEmpty())
				{
					dm.append( elemstk.top().nameprefix);
				}
				dm.append( elemname);
				if (!elemstk.top().selectedDataElements.contains( dm))
				{
					++elemstk.top().dataelementidx;
					continue;
				}
			}
			else if (isReservedProperty( elemname))
			{
				++elemstk.top().dataelementidx;
				continue;
			}
			int stksize = m_stk.size();
			if (enter( elemname))
			{
				if (stksize == m_stk.size())
				{
					///... if enter is internal in widget then we accumulate data
					//     element references for comparison with selected data elements
					QByteArray nameprefix = elemstk.top().nameprefix;
					nameprefix.append( elemname);
					nameprefix.push_back( '.');
					++elemstk.top().dataelementidx;
					elemstk.push_back( WidgetVisitorStackElement( m_stk.top(), nameprefix));
				}
				else
				{
					++elemstk.top().dataelementidx;
					elemstk.push_back( WidgetVisitorStackElement( m_stk.top()));
				}
				rt.push_back( Element( Element::OpenTag, elemstk.top().dataelements[ elemstk.top().dataelementidx]));
			}
			else
			{
				QVariant val = property( elemname);
				if (!elemstk.top().isContent)
				{
					if (!val.canConvert( QVariant::Int)) elemstk.top().isContent = false;
				}
				if (elemstk.top().isContent)
				{
					rt.push_back( Element( Element::OpenTag, elemname));
					rt.push_back( Element( Element::Value, val));
					rt.push_back( Element( Element::CloseTag, ""));
				}
				else
				{
					rt.push_back( Element( Element::Attribute, elemname));
					rt.push_back( Element( Element::Value, val));
				}
				++elemstk.top().dataelementidx;
			}
		}
		else
		{
			rt.push_back( Element( Element::CloseTag, ""));
			elemstk.pop_back();
			leave();
		}
	}
	return rt;
}


