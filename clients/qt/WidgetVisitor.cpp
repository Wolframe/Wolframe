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
#include "WidgetRequest.hpp"
#include "WidgetVisitorStateConstructor.hpp"
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

static void logError( QWidget* widget, const char* msg, const QString& arg)
{
	if (widget)
	{
		qCritical() << "error widget visitor" << widget->metaObject()->className() << widget->objectName() << msg << (arg.isEmpty()?"":":") << arg;
	}
	else
	{
		qCritical() << "error " << msg << (arg.isEmpty()?"":":") << arg;
	}
}

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

WidgetVisitor::State::State( const State& o)
	:m_widget(o.m_widget)
	,m_synonyms(o.m_synonyms)
	,m_links(o.m_links)
	,m_assignments(o.m_assignments)
	,m_datasignals(o.m_datasignals)
	,m_dataslots(o.m_dataslots)
	,m_dynamicProperties(o.m_dynamicProperties)
	,m_synonym_entercnt(o.m_synonym_entercnt)
	,m_internal_entercnt(o.m_internal_entercnt){}

WidgetVisitor::State::State( QWidget* widget_)
	:m_widget(widget_)
	,m_synonym_entercnt(1)
	,m_internal_entercnt(0)
{
	foreach (const QByteArray& prop, m_widget->dynamicPropertyNames())
	{
		if (prop.indexOf(':') >= 0)
		{
			if (prop.startsWith( "synonym:"))
			{
				QVariant synonym = m_widget->property( prop);
				m_synonyms.insert( prop.mid( 8, prop.size()-8), synonym.toByteArray());
			}
			else if (prop.startsWith( "link:"))
			{
				QVariant link = m_widget->property( prop);
				m_links.push_back( LinkDef( prop.mid( 5, prop.size()-5), link.toByteArray()));
			}
			else if (prop.startsWith( "assign:"))
			{
				QVariant value = m_widget->property( prop);
				m_assignments.push_back( Assignment( prop.mid( 7, prop.size()-7), value.toByteArray()));
			}
			else if (prop.startsWith( "datasignal:"))
			{
				QVariant value = m_widget->property( prop);
				if (prop == "datasignal:onload")
				{
					m_datasignals.id[(int)WidgetVisitor::OnLoad].push_back( value.toByteArray());
				}
				if (prop == "datasignal:domainload")
				{
					m_datasignals.id[(int)WidgetVisitor::DomainChange].push_back( value.toByteArray());
				}
				else
				{
					qCritical() << "error widget visitor state" << widget_->metaObject()->className() << widget_->objectName() << ": defined unknown data signal name" << prop;
				}
			}
			else if (prop.startsWith( "dataslot:"))
			{
				QVariant value = m_widget->property( prop);
				if (prop == "dataslot:onload")
				{
					m_dataslots.id[(int)WidgetVisitor::OnLoad].push_back( value.toByteArray());
				}
				if (prop == "dataslot:domainload")
				{
					m_dataslots.id[(int)WidgetVisitor::DomainChange].push_back( value.toByteArray());
				}
				else
				{
					qCritical() << "error widget visitor state" << widget_->metaObject()->className() << widget_->objectName() << ": defined unknown data slot name" << prop;
				}
			}
		}
		if (!prop.startsWith( "_w_") && !prop.startsWith( "_q_"))
		{
			m_dynamicProperties.insert( prop, m_widget->property( prop));
		}
	}
	static qint64 g_cnt = 0;
	QVariant ruid = m_widget->property( "widgetid");
	if (!ruid.isValid())
	{
		QByteArray rt =  m_widget->objectName().toAscii();
		rt.append( ":");
		rt.append( QVariant( ++g_cnt).toByteArray());
		m_widget->setProperty( "widgetid", QVariant(rt));
	}
}

bool WidgetVisitor::is_widgetid( const QByteArray& id)
{
	return id.indexOf(':') >= 0;
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

QByteArray WidgetVisitor::State::getLink( const QByteArray& name) const
{
	int ii = 0, nn = m_links.size();
	for (; ii<nn; ++ii)
	{
		if (m_links.at( ii).first == name)
		{
			return m_links.at( ii).second;
		}
	}
	return QByteArray();
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

WidgetVisitor::WidgetVisitor( QWidget* root)
{
	m_stk.push( createWidgetVisitorState( root));
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
	return enter( name.toAscii(), writemode, 0);
}

bool WidgetVisitor::enter( const QByteArray& name, bool writemode)
{
	return enter( name, writemode, 0);
}

bool WidgetVisitor::enter_root( const QByteArray& name)
{
	if (m_stk.empty()) return false;
	QWidget* ww = predecessor( name);
	if (ww)
	{
		m_stk.push_back( createWidgetVisitorState( ww));
		return true;
	}
	return false;
}

bool WidgetVisitor::enter( const QByteArray& name, bool writemode, int level)
{
	if (m_stk.empty()) return false;

	// [A] check if name is a synonym and follow it if yes:
	QByteArray synonym = m_stk.top()->getSynonym( name);
	if (!synonym.isEmpty())
	{
		int followidx = synonym.indexOf( '.');
		if (followidx >= 0)
		{
			// ... the referenced item is a multipart reference so it gets complicated
			int entercnt = 0;
			QByteArray prefix( synonym.mid( 0, followidx));
			QByteArray rest( synonym.mid( followidx+1, synonym.size()-followidx-1));
			do
			{
				if (!enter( prefix, writemode, level+entercnt))
				{
					for (; entercnt > 0; --entercnt) leave( writemode);
					return false;
				}
				++entercnt;
				followidx = rest.indexOf( '.');
				if (followidx < 0)
				{
					if (!enter( rest, writemode, level+entercnt))
					{
						for (; entercnt > 0; --entercnt) leave( writemode);
						return false;
					}
					++entercnt;
				}
				prefix = rest.mid( 0, followidx);
				rest = rest.mid( followidx+1, rest.size()-followidx-1);
			} while (followidx >= 0);
			m_stk.top()->m_synonym_entercnt = entercnt;
		}
		else
		{
			return enter( synonym, writemode, level);
		}
	}

	// [B] check if name refers to a widget internal item and follow it if yes:
	if (m_stk.top()->enter( name, writemode))
	{
		++m_stk.top()->m_internal_entercnt;
		return true;
	}

	if (m_stk.top()->m_internal_entercnt == 0)
	{
		// [C] check if name refers to a symbolic link and follow the link if yes:
		QByteArray lnk = m_stk.top()->getLink( name);
		if (!lnk.isEmpty())
		{
			QWidget* lnkwdg = resolveLink( lnk);
			if (!lnkwdg)
			{
				ERROR( "failed to resolve symbolic link to widget");
				return false;
			}
			m_stk.push_back( createWidgetVisitorState( lnkwdg));
			return true;
		}

		// [D] on top level check if name refers to an ancessor or an ancessor child and follow it if yes:
		if (level == 0 && !name.isEmpty() && enter_root( name))
		{
			return true;
		}

		// [E] check if name refers to a child and follow it if yes:
		if (!name.isEmpty())
		{
			QList<QWidget*> children = m_stk.top()->m_widget->findChildren<QWidget*>( name);
			if (children.size() > 1)
			{
				ERROR( "ambiguus widget reference", name);
				return false;
			}
			if (children.isEmpty()) return false;
			m_stk.push( createWidgetVisitorState( children[0]));
			return true;
		}
	}
	return false;
}

void WidgetVisitor::leave( bool writemode)
{
	if (m_stk.empty()) return;
	int cnt = m_stk.top()->m_synonym_entercnt;
	while (cnt-- > 0)
	{
		if (m_stk.top()->m_internal_entercnt)
		{
			if (!m_stk.top()->leave( writemode))
			{
				ERROR( "illegal state: internal state leave failed");
			}
			--m_stk.top()->m_internal_entercnt;
		}
		else
		{
			m_stk.pop();
		}
	}
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
			logError( visitor.widget(), "brackets { } not balanced", "");
			break;
		}
		rt.append( value.mid( startidx, substidx-startidx));
		int sb = value.indexOf( '{', substidx+1);
		if (sb > 0 && sb < endidx)
		{
			logError( visitor.widget(), "brackets { { nested", "");
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

QWidget* WidgetVisitor::predecessor( const QByteArray& name) const
{
	if (m_stk.isEmpty()) return 0;
	QWidget* wdg = m_stk.at(0)->m_widget;
	QObject* prn = wdg->parent();
	for (; prn != 0; prn = prn->parent())
	{
		if (qobject_cast<QWidget*>( prn))
		{
			wdg = qobject_cast<QWidget*>( prn);
			if (wdg->objectName() == name) return wdg;
			QList<QWidget*> cld = wdg->findChildren<QWidget*>(name);
			if (cld.size() == 1) return cld.at(0);
		}
	}
	return 0;
}

QWidget* WidgetVisitor::uirootwidget() const
{
	if (m_stk.isEmpty()) return 0;
	QWidget* wdg = m_stk.at(0)->m_widget;
	QObject* prn = wdg->parent();
	for (; prn != 0; prn = prn->parent())
	{
		if (qobject_cast<QWidget*>( prn))
		{
			wdg = qobject_cast<QWidget*>( prn);
		}
	}
	return wdg;
}

static bool nodeProperty_hasWidgetId( const QWidget* widget, const QByteArray& cond)
{
	QVariant requestid = widget->property( "widgetid");
	return (requestid.isValid() && requestid.toByteArray() == cond);
}

QWidget* WidgetVisitor::resolveLink( const QByteArray& link)
{
	QWidget* wdg = uirootwidget();
	if (!wdg) return 0;
	WidgetVisitor visitor( wdg);
	QList<QWidget*> wdglist = visitor.findSubNodes( nodeProperty_hasWidgetId, link);
	if (wdglist.isEmpty()) return 0;
	if (wdglist.size() > 1)
	{
		ERROR( "ambiguus widget link reference", link);
	}
	return wdglist.at(0);
}

void WidgetVisitor::resetState()
{
	if (!m_stk.isEmpty())
	{
		QVariant state = m_stk.top()->getState();
		m_stk.top()->clear();
		if (state.isValid())
		{
			m_stk.top()->m_widget->setProperty( "_w_state", state);
		}
	}
}

void WidgetVisitor::restoreState()
{
	if (!m_stk.isEmpty())
	{
		QVariant state = m_stk.top()->m_widget->property( "_w_state");
		m_stk.top()->setState( state);

		QVariant initialFocus = m_stk.top()->m_widget->property( "initialFocus");
		if (initialFocus.toBool()) widget()->setFocus();
	}
}

void WidgetVisitor::clear()
{
	if (!m_stk.isEmpty())
	{
		m_stk.top()->clear();
	}
}

void WidgetVisitor::readAssignments()
{
	if (m_stk.isEmpty()) return;
	foreach (const State::Assignment& assignment, m_stk.top()->m_assignments)
	{
		QVariant value = property( assignment.second);
		if (!setProperty( assignment.first, value))
		{
			ERROR( "assigment failed", assignment.first);
		}
	}
}

void WidgetVisitor::writeAssignments()
{
	if (m_stk.isEmpty()) return;
	foreach (const State::Assignment& assignment, m_stk.top()->m_assignments)
	{
		QVariant value = property( assignment.first);
		if (!setProperty( assignment.second, value))
		{
			ERROR( "assigment failed", assignment.second);
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

QVariant WidgetVisitor::property( const QByteArray& name, int level)
{
	if (m_stk.empty()) return QVariant()/*invalid*/;
	// [A] check if a synonym is referenced and redirect to evaluate synonym value instead if yes
	QByteArray synonym = m_stk.top()->getSynonym( name);
	if (!synonym.isEmpty())
	{
		return property( synonym, level);
	}

	// [B] check if an internal property of the widget is referenced and return its value if yes
	QVariant rt;
	if ((rt = m_stk.top()->property( name)).isValid())
	{
		return resolve( rt);
	}

	// [C] check if an multipart property is referenced and try to step into the substructure to get the property if yes
	bool subelem = false;
	QByteArray prefix;
	QByteArray rest;
	int followidx = name.indexOf( '.');
	if (followidx >= 0)
	{
		prefix = name.mid( 0, followidx);
		rest = name.mid( followidx+1, name.size()-followidx-1);
		if (enter( prefix, false, level))
		{
			level += 1;
			subelem = true;
		}
	}
	else
	{
		if (enter( name, false, level))
		{
			level += 1;
			subelem = true;
			prefix = name;
			rest.clear();
		}
	}
	if (subelem)
	{
		rt = property( rest, level);
		leave( false);
		if (m_stk.top()->isRepeatingDataElement( prefix))
		{
			// ... handle array
			QList<QVariant> rtlist;
			rtlist.push_back( rt);

			while (enter( prefix, false, level))
			{
				rt = property( rest, level);
				leave( false);
				rtlist.push_back( rt);
			}
			return QVariant( rtlist);
		}
		return rt;
	}
	// [D] check if a dynamic property is referenced and return its value if yes
	if (m_stk.top()->m_internal_entercnt == 0)
	{
		rt = m_stk.top()->dynamicProperty( name);
		if (rt.isValid())
		{
			return resolve( rt);
		}
	}
	return rt;
}


QByteArray WidgetVisitor::objectName() const
{
	if (m_stk.isEmpty()) return QByteArray();
	return m_stk.top()->m_widget->objectName().toAscii();
}

QByteArray WidgetVisitor::className() const
{
	if (m_stk.isEmpty()) return QByteArray();
	return m_stk.top()->m_widget->metaObject()->className();
}

QByteArray WidgetVisitor::widgetid() const
{
	if (m_stk.isEmpty()) return QByteArray();
	QVariant ruid = m_stk.top()->m_widget->property( "widgetid");
	if (ruid.type() != QVariant::ByteArray)
	{
		ERROR( "property 'widgetid' missing in state");
		return objectName();
	}
	return ruid.toByteArray();
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

	// [A] check if a synonym is referenced and redirect set the synonym value instead if yes
	QByteArray synonym = m_stk.top()->getSynonym( name);
	if (!synonym.isEmpty())
	{
		return setProperty( synonym, value, level);
	}

	// [B] check if an internal property of the widget is referenced and set its value if yes
	if (m_stk.top()->setProperty( name, value)) return true;

	// [C] check if an multipart property is referenced and try to step into the substructures to set the property (must a single value and must not have any repeating elements) if yes
	bool subelem = false;
	QByteArray prefix;
	QByteArray rest;
	int followidx = name.indexOf( '.');
	if (followidx >= 0)
	{
		prefix = name.mid( 0, followidx);
		if (m_stk.top()->isRepeatingDataElement( prefix))
		{
			ERROR( "cannot set property addressing a set of properties", prefix);
		}
		rest = name.mid( followidx+1, name.size()-followidx-1);
		if (enter( prefix, false, level))
		{
			subelem = true;
		}
	}
	else
	{
		if (m_stk.top()->isRepeatingDataElement( name))
		{
			ERROR( "cannot set property addressing a set of properties", prefix);
		}
		if (enter( name, true, level))
		{
			subelem = true;
			prefix = name;
			rest.clear();
		}
	}
	if (subelem)
	{
		bool rt = setProperty( rest, value, level+1);
		leave( true);
		return rt;
	}

	// [D] check if a dynamic property is referenced and set its value if yes
	if (m_stk.top()->m_internal_entercnt == 0)
	{
		if (m_stk.top()->setDynamicProperty( name, value)) return true;
	}
	return false;
}

QList<QWidget*> WidgetVisitor::findSubNodes( NodeProperty prop, const QByteArray& cond) const
{
	QList<QWidget*> rt;
	if (m_stk.isEmpty()) return rt;
	QVector<QWidget*> ar;

	if (prop( m_stk.top()->m_widget, cond)) rt.push_back( m_stk.top()->m_widget);
	ar.push_back( m_stk.top()->m_widget);

	int endidx = ar.size(), idx = 0;
	do
	{
		endidx = ar.size();
		while (idx < endidx)
		{
			foreach( QWidget* ww, ar[idx]->findChildren<QWidget*>())
			{
				if (prop( ww, cond)) rt.push_back( ww);
				ar.push_back( ww);
			}
			++idx;
		}
	} while (endidx < ar.size());
	return rt;
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
	// skip synonym/link/assign/datasignal/dataslot declarations:
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
		if (key == "rootelement" || key == "form" || key == "widgetid")
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
	int level;

	WidgetVisitorStackElement()
		:nof_attributes(0)
		,dataelementidx(0)
		,isContent(false)
		,hasSelectedDataelements(false)
		,level(0)
	{}
	WidgetVisitorStackElement( const WidgetVisitor::StateR& state, const QList<QByteArray>* selectedDataElements, int level_=0)
		:nof_attributes(0)
		,dataelementidx(0)
		,isContent(false)
		,hasSelectedDataelements(selectedDataElements?true:false)
		,level(level_)
	{
		if (!selectedDataElements)
		{
			foreach (const QByteArray& prop, state->m_widget->dynamicPropertyNames())
			{
				if (!isReservedProperty( prop))
				{
					dataelements.push_back( prop);
				}
			}
			dataelements.append( state->dataelements());
			foreach (const QWidget* child, state->datachildren())
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

			/* [1] Handling explicit attribute selection: */
			if (elemstk.top().hasSelectedDataelements && !elemstk.top().isContent && elemstk.top().dataelementidx < elemstk.top().nof_attributes)
			{
				//... special handling of dataelement explicitely marked as attribute '@':
				//    we allow digging in substructures for this one single element if it exists
				QVariant val = property( dataelem, elemstk.top().level);
				if (val.isValid() && !dataelem.isEmpty() && val.type() != QVariant::List)
				{
					rt.push_back( Element( Element::Attribute, dataelem));
					rt.push_back( Element( Element::Value, val));
					++elemstk.top().dataelementidx;
					continue;
				}
			}
			/* [2] Handling grouping of multiple selections with same prefix together (e.g. person.name,person.id): */
			int pntidx = dataelem.indexOf('.');
			if (pntidx >= 0)
			{
				QByteArray prefix = dataelem.mid( 0, pntidx);
				if (enter( prefix, false, elemstk.top().level))
				{
					QList<QByteArray> selected = getSuffixDataElements( elemstk.top().dataelements, prefix);
					elemstk.top().isContent = true;
					int di=elemstk.top().dataelementidx,de=elemstk.top().dataelements.size();
					for (++di; di<de; ++di)
					{
						if (elemstk.top().dataelements.at(di).startsWith( prefix)
						&&  elemstk.top().dataelements.at(di).at( prefix.size()) == '.')
						{
							elemstk.top().dataelements.removeAt( di);
						}
					}
					if (!m_stk.top()->isRepeatingDataElement( prefix))
					{
						++elemstk.top().dataelementidx;
					}
					elemstk.push_back( WidgetVisitorStackElement( m_stk.top(), &selected, elemstk.top().level+1));
					rt.push_back( Element( Element::OpenTag, prefix));
					continue;
				}
			}
			/* [3] Handling ordinary substructures: */
			else if (enter( dataelem, false, elemstk.top().level))
			{
				elemstk.top().isContent = true;
				if (!m_stk.top()->isRepeatingDataElement( dataelem))
				{
					++elemstk.top().dataelementidx;
				}
				elemstk.push_back( WidgetVisitorStackElement( m_stk.top(), 0));
				rt.push_back( Element( Element::OpenTag, dataelem));
				continue;
			}
			/* [4] Handling ordinary properties: */
			QVariant val = property( dataelem, elemstk.top().level);
			if (val.isValid())
			{
				// evaluate if is attribute or content element:
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
				// print the output property value with its tag context:
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
			else if (elemstk.top().hasSelectedDataelements && !m_stk.top()->isRepeatingDataElement( dataelem))
			{
				ERROR( "data element not found (elements defined with dataelement are mandatory)", dataelem);
			}
			++elemstk.top().dataelementidx;
		}
		else
		{
			// end of elements of this tree node, print close tag:
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

static bool nodeProperty_hasAssignment( const QWidget* widget, const QByteArray& )
{
	foreach (const QByteArray& prop, widget->dynamicPropertyNames())
	{
		if (prop.startsWith( "assign:")) return true;
	}
	return false;
}

void WidgetVisitor::do_initInititalizations()
{
	foreach (QWidget* wdg, findSubNodes( nodeProperty_hasAssignment))
	{
		WidgetVisitor chldvisitor( wdg);
		chldvisitor.readAssignments();
	}
}

void WidgetVisitor::do_closeInititalizations()
{
	foreach (QWidget* wdg, findSubNodes( nodeProperty_hasAssignment))
	{
		WidgetVisitor chldvisitor( wdg);
		chldvisitor.writeAssignments();
	}
}

void WidgetVisitor::ERROR( const char* msg, const QString& arg) const
{
	logError( widget(), msg, arg);
}

void WidgetVisitor::ERROR( const char* msg, const QByteArray& arg) const
{
	logError( widget(), msg, QString( arg));
}

static bool nodeProperty_hasDataSlot( WidgetVisitor::DataSignalType type, const QWidget* widget, const QByteArray& cond)
{
	static const char* ar[] = {"dataslot:onload","dataslot:domainchange"};
	QVariant dataslots = widget->property( ar[(int)type]);
	int idx;
	if ((idx=dataslots.toByteArray().indexOf( cond)) >= 0)
	{
		QByteArray dd = dataslots.toByteArray();
		return (dd.size() == idx || dd.at(idx) == ' ' || dd.at(idx) == ',');
	}
	return false;
}

static bool nodeProperty_hasDataSlot_onload( const QWidget* widget, const QByteArray& cond)
{
	return nodeProperty_hasDataSlot( WidgetVisitor::OnLoad, widget, cond);
}
static bool nodeProperty_hasDataSlot_domainchange( const QWidget* widget, const QByteArray& cond)
{
	return nodeProperty_hasDataSlot( WidgetVisitor::DomainChange, widget, cond);
}

QList<QWidget*> WidgetVisitor::get_datasignal_receivers( DataSignalType type)
{
	QList<QWidget*> rt;
	if (m_stk.isEmpty()) return rt;

	foreach (const QByteArray& receiverprop, m_stk.top()->m_datasignals.id[(int)type])
	{
		QByteArray receiverid = resolve( receiverprop).toByteArray();
		WidgetVisitor mainvisitor( uirootwidget());

		if (is_widgetid( receiverid))
		{
			rt.append( mainvisitor.findSubNodes( nodeProperty_hasWidgetId, receiverid));
		}
		else
		{
			switch (type)
			{
				case OnLoad:
					rt.append( mainvisitor.findSubNodes( nodeProperty_hasDataSlot_onload, receiverid));
					break;
				case DomainChange:
					rt.append( mainvisitor.findSubNodes( nodeProperty_hasDataSlot_domainchange, receiverid));
					break;
			}
		}
	}
	return rt;
}



