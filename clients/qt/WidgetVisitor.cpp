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
#include "WidgetVisitor_QTreeWidget.hpp"
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
#include <QSharedPointer>

WidgetVisitor::State::State( QWidget* widget_)
	:m_widget(widget_)
{
	QList<QByteArray> props = m_widget->dynamicPropertyNames();
	QList<QByteArray>::const_iterator pi = props.begin(), pe = props.end();
	for (; pi != pe; ++pi)
	{
		if (pi->startsWith( "synonym:"))
		{
			QVariant synonym = m_widget->property( *pi);
			m_synonyms.insert( pi->mid( 8, pi->size()-8), synonym.toByteArray());
		}
	}
}

const QByteArray& WidgetVisitor::State::getSynonym( const QByteArray& name) const
{
	static const QByteArray empty;
	QHash<QByteArray,QByteArray>::const_iterator syi = m_synonyms.find( name);
	if (syi == m_synonyms.end()) return empty;
	return syi.value();
}

WidgetVisitor::State::DataElements::DataElements( const char* elem, ...)
{
	va_list ap;
	va_start( ap, elem);
	for(;;)
	{
		const char* name = va_arg( ap, const char*);
		if (!name) break;
		*this << name;
	}
}

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
	else if (clazz == "QTableWidget")
	{
		return WidgetVisitor::StateR( new WidgetVisitor::State( widget));
	}
	else if (clazz == "QTreeWidget")
	{
		return WidgetVisitor::StateR( new WidgetVisitorState_QTreeWidget( widget));
	}
	else
	{
		return WidgetVisitor::StateR( new WidgetVisitor::State( widget));
	}
}

WidgetVisitor::WidgetVisitor( QWidget* root, const QSharedPointer<WidgetVariableMap>& globals_)
	:m_globals(globals_)
{
	m_stk.push( widgetVisitorState( root));
}

WidgetVisitor::WidgetVisitor( const WidgetVisitor::StateR& state, const QSharedPointer<WidgetVariableMap>& globals_)
	:m_globals(globals_)
{
	m_stk.push( state);
}

WidgetVisitor::WidgetVisitor( const QStack<StateR>& stk_, const QSharedPointer<WidgetVariableMap>& globals_)
	:m_stk(stk_),m_globals(globals_)
{}

bool WidgetVisitor::enter( const QString& name, bool writemode)
{
	return enter( name.toAscii(), writemode);
}

bool WidgetVisitor::enter( const QByteArray& name, bool writemode)
{
	if (m_stk.empty()) return false;
	if (m_stk.top()->enter( name, writemode))
	{
		return true;
	}
	QList<QWidget*> children = m_stk.top()->widget()->findChildren<QWidget*>( name);
	if (children.size() > 1)
	{
		qCritical() << "ambiguus widget reference" << name;
		return false;
	}
	if (children.isEmpty()) return false;

	m_stk.push( widgetVisitorState( children[0]));
	return true;
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

template <class StringType>
static void append_escaped_( StringType& dest, const StringType& value)
{
	if (value.indexOf('\'') >= 0 || value.indexOf(',') >= 0 || value.indexOf('&') >= 0)
	{
		dest.push_back( '\'');
		int idx = 0, nxt;
		while ((nxt=value.indexOf( '\'', idx)) >= idx)
		{
			dest.append( value.mid(idx,nxt));
			dest.push_back( '\'');
			dest.push_back( '\'');
			idx = nxt + 1;
		}
		dest.push_back( '\'');
	}
	else
	{
		dest.append( value);
	}
}

static void append_escaped( QString& dest, const QVariant& value)
{
	append_escaped_( dest, value.toString());
}

static void append_escaped( QByteArray& dest, const QVariant& value)
{
	append_escaped_( dest, value.toByteArray());
}

template <class StringType>
static QVariant expand_variable_references( WidgetVisitor& visitor, const StringType& value)
{
	int startidx = 0;
	int substidx = value.indexOf( '{');
	int endidx = value.indexOf( '}', substidx);
	StringType rt;
	if (substidx == 0 && endidx == value.size()-1)
	{
		return visitor.property( value.mid( substidx, endidx-substidx));
	}
	while (substidx > 0)
	{
		if (endidx < substidx)
		{
			qCritical() << "brackets { } not balanced";
			break;
		}
		rt.append( value.mid( startidx, substidx-startidx));
		int sb = value.indexOf( '{', substidx);
		if (sb > 0 && sb < endidx)
		{
			qCritical() << "brackets { { nested";
			break;
		}
		substidx++;

		// evaluate property value and append it expanded as substutute to rt:
		QVariant propvalue = visitor.property( value.mid( substidx, endidx-substidx));
		if (propvalue.type() == QVariant::List)
		{
			QList<QVariant> propvaluelist = propvalue.toList();
			QList<QVariant>::const_iterator li = propvaluelist.begin(), le = propvaluelist.end();
			int lidx = 0;
			for (; li != le; ++li,++lidx)
			{
				if (lidx) rt.push_back( ",");
				append_escaped( rt, *li);
			}
		}
		else
		{
			append_escaped( rt, propvalue.toString());
		}
		startidx = endidx + 1;
		// skip to next embedded variable reference:
		substidx = value.indexOf( '{', startidx);
		endidx = value.indexOf( '}', substidx);
	}
	rt.append( value.mid( startidx, value.size()-startidx));
	return QVariant(rt);
}

QVariant WidgetVisitor::resolve( const QVariant& value)
{
	if (value.type() == QVariant::String)
	{
		QString valuestr = value.toString();
		if (valuestr.indexOf( '{') >= 0)
		{
			return expand_variable_references( *this, valuestr);
		}
	}
	else if (value.type() == QVariant::ByteArray)
	{
		QByteArray valuestr = value.toByteArray();
		if (valuestr.indexOf( '{') >= 0)
		{
			return expand_variable_references( *this, valuestr);
		}
	}
	return value;
}

QVariant WidgetVisitor::property( const QByteArray& name, int level)
{
	if (m_stk.empty()) return QVariant()/*invalid*/;
	QByteArray synonym = m_stk.top()->getSynonym( name);
	if (!synonym.isEmpty())
	{
		return property( synonym, level);
	}
	QVariant rt = m_stk.top()->property( name);
	if (rt.isValid()) return resolve( rt);

	int followidx = name.indexOf( '.');
	if (followidx >= 0)
	{
		if (level == 0 && name.startsWith( "global."))
		{
			WidgetVariableMap::const_iterator gi = m_globals->find( name.mid( 7, name.size()-7));
			if (gi != m_globals->end())
			{
				return resolve( gi.value());
			}
		}
		QByteArray prefix( name.mid( 0, followidx));
		QByteArray rest( name.mid( followidx+1, name.size()-followidx-1));
		if (enter( prefix, false))
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
	}
	return rt;
}

QByteArray WidgetVisitor::requestUID()
{
	if (m_stk.isEmpty()) return QByteArray();
	static qint64 g_cnt = 0;
	QVariant ruid = m_stk.top()->widget()->property( "_w_requestid");
	if (ruid.type() == QVariant::ByteArray) return ruid.toByteArray();
	QByteArray rt =  m_stk.top()->widget()->metaObject()->className();
	rt.append( ":");
	rt.append( QVariant( ++g_cnt).toByteArray());
	m_stk.top()->widget()->setProperty( "_w_requestid", QVariant(rt));
	return rt;
}

QVariant WidgetVisitor::property( const QByteArray& name)
{
	return property( name, 0);
}

bool WidgetVisitor::setProperty( const QByteArray& name, const QVariant& value, int level)
{
	if (m_stk.empty()) return false;
	QByteArray synonym = m_stk.top()->getSynonym( name);
	if (!synonym.isEmpty())
	{
		return setProperty( synonym, value, level);
	}
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
		if (enter( prefix, true))
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
	}
	return false;
}

QList<WidgetVisitor> WidgetVisitor::findNodes( NodeProperty prop, const QByteArray& cond)
{
	if (m_stk.empty()) return QList<WidgetVisitor>();
	QList<WidgetVisitor> rt;
	QVector<WidgetVisitor> ar;
	ar.push_back( *this);
	if (prop( m_stk.top()->widget(), cond))
	{
		rt.push_back( *this);
		//... append *this matching to result
	}
	int endidx = ar.size(), idx = 0;
	QStack<StateR> stk = m_stk;
	do
	{
		endidx = ar.size();
		while (idx < endidx)
		{
			stk = ar[idx].m_stk;
			foreach( QWidget* ww, ar[idx].widget()->findChildren<QWidget*>())
			{
				stk.push_back( widgetVisitorState( ww));
				ar.push_back( WidgetVisitor( stk, globals()));
				if (prop( ww, cond))
				{
					rt.push_back( WidgetVisitor( stk, globals()));
					//... append child matching to result
				}
				stk.pop();
			}
			++idx;
		}
	} while (endidx < ar.size());
	return rt;
}

bool nodeProperty_hasGlobals( const QWidget* widget, const QByteArray& )
{
	QList<QByteArray> dprops = widget->dynamicPropertyNames();
	QList<QByteArray>::iterator di = dprops.begin(), de = dprops.end();
	for (; di != de; ++di)
	{
		if (di->startsWith( "global.")) return true;
	}
	return false;
}

void WidgetVisitor::initializeGlobals()
{
	foreach (WidgetVisitor ww, findNodes( nodeProperty_hasGlobals))
	{
		QList<QByteArray> dprops = ww.widget()->dynamicPropertyNames();
		QList<QByteArray>::iterator di = dprops.begin(), de = dprops.end();
		for (; di != de; ++di)
		{
			if (di->startsWith( "global."))
			{
				QByteArray globalname( di->mid( 7, di->size()-7));
				m_globals->insert( globalname, resolve( ww.property( *di)));
			}
		}
	}
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
	const QList<QByteArray>* dataelements;
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
		:dataelements(&state->dataelements())
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
	if (key.startsWith( "_q_")) return true;
	// skip _w_ dynamic properties, they are used as internal Wolframe properties
	if (key.startsWith( "_w_")) return true;
	// skip globals
	if (key.startsWith( "global.")) return true;
	// skip synonym declarations
	if (key.startsWith( "synonym:")) return true;
	// ignore our own actions
	if (key == "doctype" || key == "rootelement" || key == "dataelement" || key == "action" || key == "initAction" || key == "form" || key == "state")
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
		if (elemstk.top().dataelements && elemstk.top().dataelements->size() > elemstk.top().dataelementidx)
		{
			const QByteArray& dataelem = (*elemstk.top().dataelements)[ elemstk.top().dataelementidx];
			if (!elemstk.top().selectedDataElements.isEmpty())
			{
				QByteArray dm;
				if (!elemstk.top().nameprefix.isEmpty())
				{
					dm.append( elemstk.top().nameprefix);
				}
				dm.append( dataelem);
				if (!elemstk.top().selectedDataElements.contains( dm))
				{
					++elemstk.top().dataelementidx;
					continue;
				}
			}
			else if (isReservedProperty( dataelem))
			{
				++elemstk.top().dataelementidx;
				continue;
			}
			int stksize = m_stk.size();
			if (m_stk.top()->isRepeating( dataelem))
			{
				//... handle lists
				if (m_stk.top()->enter( dataelem, false))
				{
					///... if enter is internal in widget then we accumulate data
					//     element references for comparison with selected data elements
					QByteArray nameprefix = elemstk.top().nameprefix;
					nameprefix.append( dataelem);
					nameprefix.push_back( '.');
					elemstk.push_back( WidgetVisitorStackElement( m_stk.top(), nameprefix));
					rt.push_back( Element( Element::OpenTag, dataelem));
				}
				else
				{
					++elemstk.top().dataelementidx;
				}
				continue;
			}
			else if (enter( dataelem, false))
			{
				if (stksize == m_stk.size())
				{
					///... if enter is internal in widget then we accumulate data
					//     element references for comparison with selected data elements
					QByteArray nameprefix = elemstk.top().nameprefix;
					nameprefix.append( dataelem);
					nameprefix.push_back( '.');
					++elemstk.top().dataelementidx;
					elemstk.push_back( WidgetVisitorStackElement( m_stk.top(), nameprefix));
				}
				else
				{
					++elemstk.top().dataelementidx;
					elemstk.push_back( WidgetVisitorStackElement( m_stk.top()));
				}
				rt.push_back( Element( Element::OpenTag, dataelem));
			}
			else
			{
				QVariant val = property( dataelem);
				if (!elemstk.top().isContent)
				{
					if (!val.canConvert( QVariant::Int)) elemstk.top().isContent = true;
				}
				if (val.type() == QVariant::List)
				{
					elemstk.top().isContent = true;
				}
				if (elemstk.top().isContent)
				{
					if (val.type() == QVariant::List)
					{
						QList<QVariant> vlist = val.toList();
						foreach (const QVariant& velem, vlist)
						{
							rt.push_back( Element( Element::OpenTag, dataelem));
							rt.push_back( Element( Element::Value, velem));
							rt.push_back( Element( Element::CloseTag, ""));
						}
					}
					else
					{
						rt.push_back( Element( Element::OpenTag, dataelem));
						rt.push_back( Element( Element::Value, val));
						rt.push_back( Element( Element::CloseTag, ""));
					}
				}
				else
				{
					rt.push_back( Element( Element::Attribute, dataelem));
					rt.push_back( Element( Element::Value, val));
				}
				++elemstk.top().dataelementidx;
			}
		}
		else
		{
			elemstk.pop_back();
			if (!elemstk.isEmpty()) rt.push_back( Element( Element::CloseTag, ""));
			leave();
		}
	}
	return rt;
}


