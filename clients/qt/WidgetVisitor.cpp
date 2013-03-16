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
#include "WidgetVisitor_QLineEdit.hpp"
#include "WidgetVisitor_QTextEdit.hpp"
#include "WidgetVisitorState_QTableWidget.hpp"
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
#include <QLayout>

static bool isConvertibleToInt( const QVariant& val)
{
	if (val.type() == QVariant::String)
	{
		QString val_ = val.toString();
		QString::const_iterator vi = val_.begin(), ve = val_.end();
		if (*vi == '-') ++vi;
		for (; vi != ve; ++vi) if (*vi < '0' || *vi > '9') return false;
		return true;
	}
	if (val.type() == QVariant::ByteArray)
	{
		QByteArray val_ = val.toByteArray();
		QByteArray::const_iterator vi = val_.begin(), ve = val_.end();
		if (*vi == '-') ++vi;
		for (; vi != ve; ++vi) if (*vi < '0' || *vi > '9') return false;
		return true;
	}
	if (val.type() == QVariant::Int) return true;
	if (val.type() == QVariant::UInt) return true;
	return false;
}

WidgetVisitor::State::State( QWidget* widget_)
	:m_widget(widget_),m_entercnt(1)
{
	foreach (const QByteArray& prop, m_widget->dynamicPropertyNames())
	{
		if (prop.startsWith( "synonym:"))
		{
			QVariant synonym = m_widget->property( prop);
			m_synonyms.insert( prop.mid( 8, prop.size()-8), synonym.toByteArray());
		}
		if (!prop.startsWith( "_w_") && !prop.startsWith( "_q_"))
		{
			m_dynamicProperties.insert( prop, m_widget->property( prop));
		}
	}
}

QVariant WidgetVisitor::State::dynamicProperty( const QByteArray& name) const
{
	QHash<QByteArray,QVariant>::const_iterator di = m_dynamicProperties.find( name);
	if (di == m_dynamicProperties.end()) return QVariant();
	return di.value();
}

bool WidgetVisitor::State::setDynamicProperty( const QByteArray& name, const QVariant& value)
{
	m_dynamicProperties.insert( name, value);
	m_widget->setProperty( name, value);
	return true;
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
	*this << elem;
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
	else if (clazz == "QLineEdit")
	{
		return WidgetVisitor::StateR( new WidgetVisitorState_QLineEdit( widget));
	}
	else if (clazz == "QTextEdit")
	{
		return WidgetVisitor::StateR( new WidgetVisitorState_QTextEdit( widget));
	}
	else if (clazz == "QTableWidget")
	{
		return WidgetVisitor::StateR( new WidgetVisitorState_QTableWidget( widget));
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

WidgetVisitor::WidgetVisitor( QWidget* root)
{
	m_stk.push( widgetVisitorState( root));
}

WidgetVisitor::WidgetVisitor( const WidgetVisitor::StateR& state)
{
	m_stk.push( state);
}

WidgetVisitor::WidgetVisitor( const QStack<StateR>& stk_)
	:m_stk(stk_)
{}

bool WidgetVisitor::enter( const QString& name, bool writemode)
{
	return enter( name.toAscii(), writemode);
}

bool WidgetVisitor::enter( const QByteArray& name, bool writemode)
{
	if (m_stk.empty()) return false;
	QByteArray synonym = m_stk.top()->getSynonym( name);
	if (!synonym.isEmpty())
	{
		int followidx = synonym.indexOf( '.');
		if (followidx >= 0)
		{
			int entercnt = 0;
			QByteArray prefix( synonym.mid( 0, followidx));
			QByteArray rest( synonym.mid( followidx+1, synonym.size()-followidx-1));
			do
			{
				if (!enter( prefix, writemode))
				{
					for (; entercnt > 0; --entercnt) leave( writemode);
					return false;
				}
				++entercnt;
				followidx = rest.indexOf( '.');
				if (followidx < 0)
				{
					if (!enter( rest, writemode))
					{
						for (; entercnt > 0; --entercnt) leave( writemode);
						return false;
					}
					++entercnt;
				}
				prefix = rest.mid( 0, followidx);
				rest = rest.mid( followidx+1, rest.size()-followidx-1);
			} while (followidx >= 0);
			m_stk.top()->entercnt( entercnt);
		}
		else
		{
			return enter( synonym, writemode);
		}
	}
	if (m_stk.top()->enter( name, writemode))
	{
		return true;
	}
	if (!name.isEmpty())
	{
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
	return false;
}

void WidgetVisitor::leave( bool writemode)
{
	if (m_stk.empty()) return;
	int cnt = m_stk.top()->entercnt();
	while (cnt-- > 0)
	{
		if (!m_stk.top()->leave( writemode))
		{
			m_stk.pop();
		}
	}
}

QVariant WidgetVisitor::property( const char* name)
{
	return property( QByteArray( name));
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
		return visitor.property( value.mid( substidx+1, endidx-substidx-1));
	}
	while (substidx > 0)
	{
		if (endidx < substidx)
		{
			qCritical() << "brackets { } not balanced";
			break;
		}
		rt.append( value.mid( startidx, substidx-startidx));
		int sb = value.indexOf( '{', substidx+1);
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

void WidgetVisitor::resetState()
{
	if (!m_stk.isEmpty())
	{
		QVariant state = m_stk.top()->getState();
		if (state.isValid())
		{
			m_stk.top()->widget()->setProperty( "_w_state", state);
		}
		m_stk.top()->clear();
	}
}

void WidgetVisitor::restoreState()
{
	QVariant state = m_stk.top()->widget()->property( "_w_state");
	m_stk.top()->setState( state);

	QVariant initialFocus = m_stk.top()->dynamicProperty( "initialFocus");
	if (initialFocus.toBool()) widget()->setFocus();
}

WidgetVisitor WidgetVisitor::getRootElement( const QByteArray& name)
{
	if (m_stk.empty()) return WidgetVisitor();
	if (m_stk.at(0)->widget()->objectName() == name)
	{
		WidgetVisitor rt( m_stk.at(0));
		return rt;
	}
	QList<QWidget*> ww = m_stk.at(0)->widget()->findChildren<QWidget*>(name);
	if (ww.size() == 1)
	{
		WidgetVisitor rt( ww[0]);
		return rt;
	}
	return WidgetVisitor();
}

QVariant WidgetVisitor::property( const QByteArray& name, int level)
{
	if (m_stk.empty()) return QVariant()/*invalid*/;
	QByteArray synonym = m_stk.top()->getSynonym( name);
	if (!synonym.isEmpty())
	{
		return property( synonym, level);
	}
	QVariant rt;
	if ((rt = m_stk.top()->property( name)).isValid()) return resolve( rt);
	if ((rt = m_stk.top()->dynamicProperty( name)).isValid()) return resolve( rt);

	int followidx = name.indexOf( '.');
	if (followidx >= 0)
	{
		QByteArray prefix( name.mid( 0, followidx));
		QByteArray rest( name.mid( followidx+1, name.size()-followidx-1));
		if (enter( prefix, false))
		{
			rt = property( rest, level+1);
			leave( false);
			if (m_stk.top()->isRepeatingDataElement( prefix))
			{
				// ... handle array
				QList<QVariant> rtlist;
				rtlist.push_back( rt);

				while (enter( prefix, false))
				{
					rt = property( rest, level+1);
					leave( false);
					rtlist.push_back( rt);
				}
				return QVariant( rtlist);
			}
			return rt;
		}
		if (level == 0)
		{
			return getRootElement( prefix).property( rest, 1);
		}
	}
	return rt;
}

QByteArray WidgetVisitor::objectName() const
{
	if (m_stk.isEmpty()) return QByteArray();
	return m_stk.top()->widget()->objectName().toAscii();
}

QByteArray WidgetVisitor::className() const
{
	if (m_stk.isEmpty()) return QByteArray();
	return m_stk.top()->widget()->metaObject()->className();
}

QByteArray WidgetVisitor::requestUID()
{
	if (m_stk.isEmpty()) return QByteArray();
	static qint64 g_cnt = 0;
	QVariant ruid = m_stk.top()->widget()->property( "_w_requestid");
	if (ruid.type() == QVariant::ByteArray) return ruid.toByteArray();
	QByteArray rt =  objectName();
	rt.append( ":");
	rt.append( QVariant( ++g_cnt).toByteArray());
	m_stk.top()->widget()->setProperty( "_w_requestid", QVariant(rt));
	return rt;
}

QVariant WidgetVisitor::property( const QByteArray& name)
{
	return property( name, 0);
}

bool WidgetVisitor::setProperty( const char* name, const QVariant& value)
{
	return setProperty( QByteArray(name), value);
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
		QByteArray prefix( name.mid( 0, followidx));
		QByteArray rest( name.mid( followidx+1, name.size()-followidx-1));
		if (enter( prefix, true))
		{
			bool rt = setProperty( rest, value, level+1);
			leave( true);
			if (m_stk.top()->isRepeatingDataElement( prefix))
			{
				qCritical() << "cannot set property addressing a set of properties:" << prefix;
			}
			return rt;
		}
		if (level == 0)
		{
			return getRootElement( prefix).setProperty( rest, value, 1);
		}
	}
	else
	{
		if (m_stk.top()->setDynamicProperty( name, value)) return true;
	}
	return false;
}

QList<WidgetVisitor> WidgetVisitor::findNodes( NodeProperty prop, const QByteArray& cond)
{
	if (m_stk.isEmpty()) return QList<WidgetVisitor>();
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
				ar.push_back( WidgetVisitor( stk));
				if (prop( ww, cond))
				{
					rt.push_back( WidgetVisitor( stk));
					//... append child matching to result
				}
				stk.pop();
			}
			++idx;
		}
	} while (endidx < ar.size());
	return rt;
}

WidgetVisitor WidgetVisitor::getSubWidgetVisitor( const QWidget* subwidget) const
{
	if (m_stk.isEmpty()) return WidgetVisitor();
	WidgetVisitor rt( *this);
	foreach (QWidget* ww, widget()->findChildren<QWidget*>())
	{
		rt.m_stk.push_back( widgetVisitorState( ww));
		if (ww == subwidget) return rt;
		WidgetVisitor follow( rt.getSubWidgetVisitor( subwidget));
		if (follow.widget()) return follow;
		rt.m_stk.pop_back();
	}
	return WidgetVisitor();
}

bool WidgetVisitor::setProperty( const QString& name, const QVariant& value)
{
	return setProperty( name.toAscii(), value, 0);
}

bool WidgetVisitor::setProperty( const QByteArray& name, const QVariant& value)
{
	return setProperty( name, value, 0);
}

static bool isReservedProperty( const QByteArray& key)
{
	// skip dynamic properties used by the Qt stylesheet engine:
	if (key.startsWith( "_q_")) return true;
	if (key == "initialFocus") return true;
	// skip _w_ dynamic properties, they are used as internal Wolframe properties:
	if (key.startsWith( "_w_")) return true;
	// skip synonym/link declarations:
	if (key.indexOf( ':') >= 0) return true;
	// ignore Wolframe elements:
	if (key[0] == 'd')
	{
		if (key == "doctype" || key == "dataobject" || key == "dataelement")
		{
			return true;
		}
	}
	else
	{
		if (key == "rootelement" || key == "form")
		{
			return true;
		}
	}
	return false;
}

struct WidgetVisitorStackElement
{
	QList<QByteArray> dataelements;
	int nof_attributes;
	int dataelementidx;
	bool isContent;
	bool hasSelectedDataelements;

	WidgetVisitorStackElement()
		:nof_attributes(0)
		,dataelementidx(0)
		,isContent(false)
		,hasSelectedDataelements(false)
	{}
	WidgetVisitorStackElement( const WidgetVisitor::StateR& state, const QList<QByteArray>* selectedDataElements)
		:nof_attributes(0)
		,dataelementidx(0)
		,isContent(false)
		,hasSelectedDataelements(selectedDataElements?true:false)
	{
		if (!selectedDataElements)
		{
			foreach (const QByteArray& prop, state->widget()->dynamicPropertyNames())
			{
				if (!isReservedProperty( prop))
				{
					dataelements.push_back( prop);
				}
			}
			dataelements.append( state->dataelements());
			foreach (const QWidget* child, state->widget()->findChildren<QWidget*>())
			{
				if (qobject_cast<const QLabel*>(child)) continue;
				if (qobject_cast<const QAbstractButton*>(child)) continue;
				if (qobject_cast<const QLayout*>(child)) continue;
				QByteArray objname = child->objectName().toAscii();
				if (!objname.isEmpty() && !objname.startsWith("qt_"))
				{
					dataelements << objname;
				}
			}
		}
		else
		{
			foreach (const QByteArray& elem, *selectedDataElements)
			{
				if (elem.size() > 0 && elem[0] == '@')
				{
					dataelements.push_back( elem.mid( 1, elem.size()-1));
				}
			}
			nof_attributes = dataelements.size();
			foreach (const QByteArray& elem, *selectedDataElements)
			{
				if (elem.size() == 0 || elem[0] != '@')
				{
					dataelements.push_back( elem);
				}
			}
		}
	}
	WidgetVisitorStackElement( const WidgetVisitorStackElement& o)
		:dataelements( o.dataelements)
		,nof_attributes(o.nof_attributes)
		,dataelementidx( o.dataelementidx)
		,isContent(o.isContent)
		,hasSelectedDataelements(o.hasSelectedDataelements)
	{}
};

QList<WidgetVisitor::Element> WidgetVisitor::elements( const QList<QByteArray>& selectedDataElements)
{
	return elements( &selectedDataElements);
}

QList<WidgetVisitor::Element> WidgetVisitor::elements()
{
	return elements( 0);
}

static QList<QByteArray> getSuffixDataElements( const QList<QByteArray>& ba, const QByteArray& prefix)
{
	QList<QByteArray> rt;
	foreach (const QByteArray& de, ba)
	{
		if (de.startsWith( prefix) && de.size() > prefix.size() && de.at( prefix.size()) == '.')
		{
			rt.push_back( de.mid( prefix.size(), de.size()- prefix.size()));
		}
	}
	return rt;
}

QList<WidgetVisitor::Element> WidgetVisitor::elements( const QList<QByteArray>* selectedDataElements)
{
	if (m_stk.isEmpty()) return QList<WidgetVisitor::Element>();

	QStack<WidgetVisitorStackElement> elemstk;
	elemstk.push_back( WidgetVisitorStackElement( m_stk.top(), selectedDataElements));

	QList<WidgetVisitor::Element> rt;
	while (!elemstk.isEmpty())
	{
		if (elemstk.top().dataelements.size() > elemstk.top().dataelementidx)
		{
			const QByteArray& dataelem = elemstk.top().dataelements.at( elemstk.top().dataelementidx);

			if (elemstk.top().hasSelectedDataelements && !elemstk.top().isContent && elemstk.top().dataelementidx < elemstk.top().nof_attributes)
			{
				//... special handling of dataelement explicitely marked as attribute '@':
				//    we allow digging in substructures for this one single element if it exists
				QVariant val = property( dataelem);
				if (val.isValid() && !dataelem.isEmpty() && val.type() != QVariant::List)
				{
					rt.push_back( Element( Element::Attribute, dataelem));
					rt.push_back( Element( Element::Value, val));
					++elemstk.top().dataelementidx;
					continue;
				}
			}
			int stksize = m_stk.size();
			if (m_stk.top()->isRepeatingDataElement( dataelem))
			{
				//... handle array
				if (m_stk.top()->enter( dataelem, false))
				{
					///... if enter is internal in widget then we accumulate data
					//     element references for comparison with selected data elements
					QList<QByteArray> selected = getSuffixDataElements( elemstk.top().dataelements, dataelem);
					elemstk.top().isContent = true;
					elemstk.push_back( WidgetVisitorStackElement( m_stk.top(), &selected));
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
				elemstk.top().isContent = true;
				if (stksize == m_stk.size())
				{
					///... if enter is internal in widget then we accumulate data
					//     element references for comparison with selected data elements
					QList<QByteArray> selected = getSuffixDataElements( elemstk.top().dataelements, dataelem);
					++elemstk.top().dataelementidx;
					elemstk.push_back( WidgetVisitorStackElement( m_stk.top(), &selected));
				}
				else
				{
					++elemstk.top().dataelementidx;
					elemstk.push_back( WidgetVisitorStackElement( m_stk.top(), 0));
				}
				rt.push_back( Element( Element::OpenTag, dataelem));
			}
			else
			{
				QVariant val = property( dataelem);
				if (val.isValid())
				{
					if (dataelem.isEmpty() || val.type() == QVariant::List)
					{
						elemstk.top().isContent = true;
					}
					if (!elemstk.top().isContent)
					{
						if (elemstk.top().hasSelectedDataelements)
						{
							if (elemstk.top().dataelementidx >= elemstk.top().nof_attributes)
							{
								elemstk.top().isContent = true;
							}
						}
						else
						{
							if (!isConvertibleToInt( val)) elemstk.top().isContent = true;
						}
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
						else if (dataelem.isEmpty())
						{
							rt.push_back( Element( Element::Value, val));
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
				}
				else if (elemstk.top().hasSelectedDataelements)
				{
					qCritical() << "not found element:" << dataelem << "(elements defined with dataelement are mandatory)";
				}
				++elemstk.top().dataelementidx;
			}
		}
		else
		{
			elemstk.pop_back();
			if (!elemstk.isEmpty())
			{
				if (rt.back().type() == Element::OpenTag)
				{
					rt.pop_back();
				}
				else
				{
					rt.push_back( Element( Element::CloseTag, ""));
				}
				leave( false);
			}
		}
	}
	return rt;
}



