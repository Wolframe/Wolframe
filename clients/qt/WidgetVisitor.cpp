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
#include "FormWidget.hpp"

#include <QDebug>
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
#include <QLayout>

#undef WOLFRAME_LOWLEVEL_DEBUG
#ifdef WOLFRAME_LOWLEVEL_DEBUG
#define TRACE_STATUS( TITLE, CLASS, OBJ, NAME)		qDebug() << "widget visit state" << (TITLE) << (CLASS) << (OBJ) << (NAME);
#define TRACE_FETCH( TITLE, OBJ, NAME, VALUE)		qDebug() << "widget visit get" << (TITLE) << (OBJ) << (NAME) << "=" << (VALUE);
#define TRACE_ASSIGNMENT( TITLE, OBJ, NAME, VALUE)	qDebug() << "widget visit set" << (TITLE) << (OBJ) << (NAME) << "=" << (VALUE);
#define TRACE_ENTER( TITLE, CLASS, OBJ, NAME)		qDebug() << "widget visit enter" << (TITLE) << (CLASS) << (OBJ) << "into" << (NAME);
#define TRACE_LEAVE( TITLE)				qDebug() << "widget visit leave" << (TITLE);
#else
#define TRACE_STATUS( TITLE, CLASS, OBJ, NAME)
#define TRACE_FETCH( TITLE, OBJ, NAME, VALUE)
#define TRACE_ASSIGNMENT( TITLE, OBJ, NAME, VALUE)
#define TRACE_ENTER( TITLE, CLASS, OBJ, NAME)
#define TRACE_LEAVE( TITLE)
#endif

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

static void getWidgetChildren_( QList<QWidget*>& rt, QObject* wdg)
{
	static const QString str_QWidget("QWidget");

	foreach (QObject* cld, wdg->children())
	{
		if (qobject_cast<QLayout*>( cld))
		{
			getWidgetChildren_( rt, cld);
		}
		else
		{
			QWidget* we = qobject_cast<QWidget*>( cld);
			if (we)
			{
				if (we->layout() && cld->metaObject()->className() == str_QWidget)
				{
					getWidgetChildren_( rt, cld);
				}
				else
				{
					rt.push_back( we);
				}
			}
		}
	}
}

static QList<QWidget*> getWidgetChildren( QWidget* wdg)
{
	QList<QWidget*> rt;
	getWidgetChildren_( rt, wdg);
	return rt;
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
	foreach (const QString& prop, m_widget->dynamicPropertyNames())
	{
		if (prop.indexOf(':') >= 0)
		{
			if (prop.startsWith( "synonym:"))
			{
				QVariant synonym = m_widget->property( prop.toAscii());
				m_synonyms.insert( prop.mid( 8, prop.size()-8), synonym.toString());
			}
			else if (prop.startsWith( "link:"))
			{
				QVariant link = m_widget->property( prop.toAscii());
				m_links.push_back( LinkDef( prop.mid( 5, prop.size()-5), link.toString()));
			}
			else if (prop.startsWith( "assign:"))
			{
				QVariant value = m_widget->property( prop.toAscii());
				m_assignments.push_back( Assignment( prop.mid( 7, prop.size()-7), value.toString()));
			}
			else if (prop.startsWith( "global:"))
			{
				QVariant value = m_widget->property( prop.toAscii());
				m_globals.push_back( Assignment( prop.mid( 7, prop.size()-7), value.toString()));
			}
			else if (prop.startsWith( "datasignal:"))
			{
				QList<QString> values;
				foreach (const QString& vv, m_widget->property( prop.toAscii()).toString().trimmed().split(','))
				{
					values.push_back( vv.trimmed());
				}
				if (prop == "datasignal:onload")
				{
					m_datasignals.id[(int)WidgetVisitor::OnLoad] = values;
				}
				else if (prop == "datasignal:onchange")
				{
					m_datasignals.id[(int)WidgetVisitor::OnChange] = values;
				}
				else if (prop == "datasignal:domainload")
				{
					m_datasignals.id[(int)WidgetVisitor::DomainChange] = values;
				}
				else
				{
					qCritical() << "error widget visitor state" << widget_->metaObject()->className() << widget_->objectName() << ": defined unknown data signal name" << prop;
				}
			}
			else if (prop.startsWith( "dataslot:"))
			{
				QList<QString> values;
				foreach (const QString& vv, m_widget->property( prop.toAscii()).toString().trimmed().split(','))
				{
					values.push_back( vv.trimmed());
				}
				if (prop == "dataslot:onload")
				{
					m_dataslots.id[(int)WidgetVisitor::OnLoad] = values;
				}
				else if (prop == "dataslot:onchange")
				{
					m_dataslots.id[(int)WidgetVisitor::OnChange] = values;
				}
				else if (prop == "dataslot:domainload")
				{
					m_dataslots.id[(int)WidgetVisitor::DomainChange] = values;
				}
				else
				{
					qCritical() << "error widget visitor state" << widget_->metaObject()->className() << widget_->objectName() << ": defined unknown data slot name" << prop;
				}
			}
		}
		if (!prop.startsWith( "_w_") && !prop.startsWith( "_q_"))
		{
			m_dynamicProperties.insert( prop, m_widget->property( prop.toAscii()));
		}
	}
	static qint64 g_cnt = 0;
	QVariant ruid = m_widget->property( "widgetid");
	if (!ruid.isValid())
	{
		QString rt =  m_widget->objectName();
		rt.append( ":");
		rt.append( QVariant( ++g_cnt).toString());
		m_widget->setProperty( "widgetid", QVariant(rt));
	}
}

QList<QWidget*> WidgetVisitor::State::datachildren() const
{
	return getWidgetChildren( m_widget);
}

bool WidgetVisitor::is_widgetid( const QString& id)
{
	return id.indexOf(':') >= 0;
}

QWidget* WidgetVisitor::get_widget_reference( const QString& id)
{
	if (enter( id, true) && m_stk.top()->m_internal_entercnt == 0)
	{
		QWidget* rt = widget();
		leave( true);
		return rt;
	}
	return 0;
}

QVariant WidgetVisitor::State::dynamicProperty( const QString& name) const
{
	QHash<QString,QVariant>::const_iterator di = m_dynamicProperties.find( name);
	if (di == m_dynamicProperties.end()) return QVariant();
	return di.value();
}

bool WidgetVisitor::State::setDynamicProperty( const QString& name, const QVariant& value)
{
	m_dynamicProperties.insert( name, value);
	m_widget->setProperty( name.toAscii(), value);
	return true;
}

const QString& WidgetVisitor::State::getSynonym( const QString& name) const
{
	static const QString empty;
	QHash<QString,QString>::const_iterator syi = m_synonyms.find( name);
	if (syi == m_synonyms.end()) return empty;
	return syi.value();
}

QString WidgetVisitor::State::getLink( const QString& name) const
{
	int ii = 0, nn = m_links.size();
	for (; ii<nn; ++ii)
	{
		if (m_links.at( ii).first == name)
		{
			return m_links.at( ii).second;
		}
	}
	return QString();
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
	return enter( name, writemode, 0);
}

bool WidgetVisitor::enter_root( const QString& name)
{
	if (m_stk.empty()) return false;
	QWidget* ww = predecessor( name);
	if (ww)
	{
		if (ww != m_stk.top()->m_widget)
		{
			m_stk.push_back( createWidgetVisitorState( ww));
			return true;
		}
	}
	return false;
}

QList<QWidget*> WidgetVisitor::children( const QString& name) const
{
	QList<QWidget*> rt;
	if (!m_stk.empty() && !m_stk.top()->m_internal_entercnt)
	{
		foreach( QWidget* ww, getWidgetChildren( m_stk.top()->m_widget))
		{
			if (name.isEmpty() || ww->objectName() == name)
			{
				rt.push_back( ww);
			}
		}
	}
	return rt;
}

bool WidgetVisitor::enter( const QString& name, bool writemode, int level)
{
	TRACE_STATUS( "try enter", className(), objectName(), name)
	if (m_stk.empty()) return false;

	// [A] check if name is a synonym and follow it if yes:
	QString synonym = m_stk.top()->getSynonym( name);
	if (!synonym.isEmpty())
	{
		return enter( synonym, writemode, level);
	}
	// [A.1] check if name is a multipart reference and follow it if yes:
	int followidx = name.indexOf( '.');
	if (followidx >= 0)
	{
		// ... the referenced item is a multipart reference so it gets complicated
		int entercnt = 0;
		QString prefix( name.mid( 0, followidx));
		QString rest( name.mid( followidx+1, name.size()-followidx-1));
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
		return true;
	}

	TRACE_STATUS( "try enter internal", className(), objectName(), name)
	// [B] check if name refers to a widget internal item and follow it if yes:
	if (m_stk.top()->enter( name, writemode))
	{
		TRACE_ENTER( "internal", className(), objectName(), name);
		++m_stk.top()->m_internal_entercnt;
		return true;
	}

	if (m_stk.top()->m_internal_entercnt == 0)
	{
		// [C] check if name refers to a symbolic link and follow the link if yes:
		TRACE_STATUS( "try enter link", className(), objectName(), name)
		QString lnk = m_stk.top()->getLink( name);
		if (!lnk.isEmpty())
		{
			QWidget* lnkwdg = resolveLink( lnk);
			if (!lnkwdg)
			{
				ERROR( "failed to resolve symbolic link to widget");
				return false;
			}
			m_stk.push_back( createWidgetVisitorState( lnkwdg));
			TRACE_ENTER( "link", className(), objectName(), name);
			return true;
		}

		// [D] on top level check if name refers to an ancessor or an ancessor child and follow it if yes:
		TRACE_STATUS( "try enter root", className(), objectName(), name)
		if (level == 0 && !name.isEmpty() && enter_root( name))
		{
			TRACE_ENTER( "root", className(), objectName(), name);
			return true;
		}

		// [E] check if name refers to a child and follow it if yes:
		if (!name.isEmpty())
		{
			TRACE_STATUS( "try enter children", className(), objectName(), name)
			QList<QWidget*> cn = children( name);
			if (cn.size() > 1)
			{
				ERROR( "ambiguus widget reference", name);
				return false;
			}
			if (cn.isEmpty()) return false;
			m_stk.push( createWidgetVisitorState( cn[0]));
			TRACE_ENTER( "child", className(), objectName(), name);
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
			TRACE_LEAVE( "internal")
			if (!m_stk.top()->leave( writemode))
			{
				ERROR( "illegal state: internal state leave failed");
			}
			--m_stk.top()->m_internal_entercnt;
		}
		else
		{
			TRACE_LEAVE( "")
			m_stk.pop();
		}
	}
}

static void append_escaped_( QString& dest, const QString& value)
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

static QVariant expand_variable_references( WidgetVisitor& visitor, const QString& value)
{
	int startidx = 0;
	int substidx = value.indexOf( '{');
	int endidx = value.indexOf( '}', substidx);
	QString rt;
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
	QString valuestr = value.toString();
	if (valuestr.indexOf( '{') >= 0)
	{
		return expand_variable_references( *this, valuestr);
	}
	return value;
}

FormWidget* WidgetVisitor::formwidget() const
{
	if (m_stk.isEmpty()) return 0;
	QObject* prn = m_stk.top()->m_widget;
	for (; prn != 0; prn = prn->parent())
	{
		FormWidget* fw = qobject_cast<FormWidget*>( prn);
		if (fw) return fw;
	}
	return 0;
}

QWidget* WidgetVisitor::predecessor( const QString& name) const
{
	if (m_stk.isEmpty()) return 0;
	QObject* prn = m_stk.top()->m_widget;
	for (; prn != 0; prn = prn->parent())
	{
		QWidget* wdg = qobject_cast<QWidget*>( prn);
		if (wdg)
		{
			if (wdg->objectName() == name) return wdg;
			if (wdg) foreach (QWidget* cld, getWidgetChildren( wdg))
			{
				if (cld->objectName() == name)
				{
					return cld;
				}
			}
		}
		if (qobject_cast<FormWidget*>( wdg)) break;
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

static bool nodeProperty_hasWidgetId( const QWidget* widget, const QVariant& cond)
{
	QVariant requestid = widget->property( "widgetid");
	return (requestid.isValid() && requestid == cond);
}

QWidget* WidgetVisitor::resolveLink( const QString& link)
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

void WidgetVisitor::endofDataFeed()
{
	if (!m_stk.isEmpty()) m_stk.top()->endofDataFeed();
}

void WidgetVisitor::clear()
{
	if (!m_stk.isEmpty())
	{
		m_stk.top()->clear();
	}
}

QVariant WidgetVisitor::property( const QString& name)
{
	return property( name, 0);
}

QVariant WidgetVisitor::property( const char* name)
{
	return property( QString( name), 0);
}

QVariant WidgetVisitor::property( const QString& name, int level)
{
	if (m_stk.empty()) return QVariant()/*invalid*/;
	// [A] check if a synonym is referenced and redirect to evaluate synonym value instead if yes
	QString synonym = m_stk.top()->getSynonym( name);
	if (!synonym.isEmpty())
	{
		return property( synonym, level);
	}

	// [C] check if an multipart property is referenced and try to step into the substructure to get the property if yes
	bool subelem = false;
	QString prefix;
	QString rest;
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
		QVariant rt = property( rest, level);
		bool isArray = m_stk.top()->m_internal_entercnt != 0 && m_stk.top()->isRepeatingDataElement( prefix);
		leave( false);
		if (isArray)
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
	if (followidx < 0)
	{
		// [B] check if an internal property of the widget is referenced and return its value if yes
		QVariant rt;
		if ((rt = m_stk.top()->property( name)).isValid())
		{
			TRACE_FETCH( "internal property", objectName(), name, rt)
			return resolve( rt);
		}

		// [D] check if a dynamic property is referenced and return its value if yes
		if (m_stk.top()->m_internal_entercnt == 0)
		{
			rt = m_stk.top()->dynamicProperty( name);
			if (rt.isValid())
			{
				TRACE_FETCH( "dynamic property", objectName(), name, rt)
				return resolve( rt);
			}
		}
	}
	return QVariant();
}


QString WidgetVisitor::objectName() const
{
	if (m_stk.isEmpty()) return QString();
	return m_stk.top()->m_widget->objectName();
}

QString WidgetVisitor::className() const
{
	if (m_stk.isEmpty()) return QString();
	return m_stk.top()->m_widget->metaObject()->className();
}

QString WidgetVisitor::widgetid() const
{
	if (m_stk.isEmpty()) return QString();
	QVariant ruid = m_stk.top()->m_widget->property( "widgetid");
	if (ruid.type() != QVariant::String)
	{
		ERROR( "property 'widgetid' missing in state");
		return objectName();
	}
	return ruid.toString();
}

bool WidgetVisitor::setProperty( const QString& name, const QVariant& value)
{
	return setProperty( name, value, 0);
}

bool WidgetVisitor::setProperty( const char* name, const QVariant& value)
{
	return setProperty( QString(name), value, 0);
}

bool WidgetVisitor::setProperty( const QString& name, const QVariant& value, int level)
{
	if (m_stk.empty()) return false;

	// [A] check if a synonym is referenced and redirect set the synonym value instead if yes
	QString synonym = m_stk.top()->getSynonym( name);
	if (!synonym.isEmpty())
	{
		TRACE_STATUS( "found synonym", synonym, name, value)
		return setProperty( synonym, value, level);
	}

	// [C] check if an multipart property is referenced and try to step into the substructures to set the property (must a single value and must not have any repeating elements) if yes
	bool subelem = false;
	QString prefix;
	QString rest;
	int followidx = name.indexOf( '.');
	if (followidx >= 0)
	{
		prefix = name.mid( 0, followidx);
		rest = name.mid( followidx+1, name.size()-followidx-1);
		TRACE_STATUS( "structured property", name, prefix, rest)
		if (enter( prefix, false, level))
		{
			subelem = true;
		}
		bool isArray = m_stk.top()->m_internal_entercnt != 0 && m_stk.top()->isRepeatingDataElement( prefix);
		if (isArray)
		{
			ERROR( "cannot set property addressing a set of properties", prefix);
		}
	}
	else
	{
		TRACE_STATUS( "identifier property", className(), objectName(), name)
		if (enter( name, true, level))
		{
			subelem = true;
			prefix = name;
			rest.clear();
		}
		bool isArray = m_stk.top()->m_internal_entercnt != 0 && m_stk.top()->isRepeatingDataElement( name);
		if (isArray)
		{
			ERROR( "cannot set property addressing a set of properties", prefix);
		}
	}
	if (subelem)
	{
		bool rt = setProperty( rest, value, level+1);
		leave( true);
		return rt;
	}
	if (followidx < 0)
	{
		TRACE_STATUS( "try to set internal property", className(), objectName(), name)
		// [B] check if an internal property of the widget is referenced and set its value if yes
		if (m_stk.top()->setProperty( name, value))
		{
			TRACE_ASSIGNMENT( "internal property", objectName(), name, value)
			return true;
		}

		TRACE_STATUS( "try to set dynamic property", className(), objectName(), name)
		// [D] check if a dynamic property is referenced and set its value if yes
		if (m_stk.top()->m_internal_entercnt == 0)
		{
			TRACE_ASSIGNMENT( "dynamic property", objectName(), name, value)
			if (m_stk.top()->setDynamicProperty( name, value)) return true;
		}
	}
	return false;
}

QList<QWidget*> WidgetVisitor::findSubNodes( NodeProperty prop, const QVariant& cond) const
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
			foreach( QWidget* ww, getWidgetChildren( ar[idx]))
			{
				if (prop( ww, cond)) rt.push_back( ww);
				ar.push_back( ww);
			}
			++idx;
		}
	} while (endidx < ar.size());
	return rt;
}

static bool isReservedProperty( const QString& key)
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
	QList<QString> dataelements;
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
	WidgetVisitorStackElement( const WidgetVisitor::StateR& state, const QList<QString>* selectedDataElements, int level_=0)
		:nof_attributes(0)
		,dataelementidx(0)
		,isContent(false)
		,hasSelectedDataelements(selectedDataElements?true:false)
		,level(level_)
	{
		if (!selectedDataElements)
		{
			level += 1; //... disallow root element reference with implicit references
			foreach (const QString& prop, state->m_widget->dynamicPropertyNames())
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
				QString objname = child->objectName();
				if (!objname.isEmpty() && !objname.startsWith("qt_"))
				{
					dataelements << objname;
				}
			}
			TRACE_ASSIGNMENT( "implicit dataelements", state->m_widget->metaObject()->className(), state->m_widget->objectName(), dataelements);
		}
		else
		{
			foreach (const QString& elem, *selectedDataElements)
			{
				if (elem.size() > 0 && elem[0] == '@')
				{
					dataelements.push_back( elem.mid( 1, elem.size()-1));
				}
			}
			nof_attributes = dataelements.size();
			foreach (const QString& elem, *selectedDataElements)
			{
				if (elem.size() == 0 || elem[0] != '@')
				{
					dataelements.push_back( elem);
				}
			}
			TRACE_ASSIGNMENT( "selected attribute elements", state->m_widget->metaObject()->className(), state->m_widget->objectName(), dataelements.mid( 0, nof_attributes));
			TRACE_ASSIGNMENT( "selected content elements", state->m_widget->metaObject()->className(), state->m_widget->objectName(), dataelements.mid( nof_attributes, dataelements.size()));
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

QList<WidgetVisitor::Element> WidgetVisitor::elements( const QList<QString>& selectedDataElements)
{
	return elements( &selectedDataElements);
}

QList<WidgetVisitor::Element> WidgetVisitor::elements()
{
	return elements( 0);
}

static QList<QString> getSuffixDataElements( const QList<QString>& ba, const QString& prefix)
{
	QList<QString> rt;
	foreach (const QString& de, ba)
	{
		if (de.startsWith( prefix) && de.size() > prefix.size() && de.at( prefix.size()) == '.')
		{
			rt.push_back( de.mid( prefix.size(), de.size()- prefix.size()));
		}
	}
	return rt;
}

QList<WidgetVisitor::Element> WidgetVisitor::elements( const QList<QString>* selectedDataElements)
{
	int loopcnt = 0;
	if (m_stk.isEmpty()) return QList<WidgetVisitor::Element>();

	QStack<WidgetVisitorStackElement> elemstk;
	elemstk.push_back( WidgetVisitorStackElement( m_stk.top(), selectedDataElements));

	QList<WidgetVisitor::Element> rt;
	while (!elemstk.isEmpty())
	{
		if (elemstk.top().dataelements.size() > elemstk.top().dataelementidx)
		{
			const QString& dataelem = elemstk.top().dataelements.at( elemstk.top().dataelementidx);

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
				QString prefix = dataelem.mid( 0, pntidx);
				if (enter( prefix, false, elemstk.top().level))
				{
					QList<QString> selected = getSuffixDataElements( elemstk.top().dataelements, prefix);
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
					bool isArray = m_stk.top()->m_internal_entercnt != 0 && m_stk.top()->isRepeatingDataElement( prefix);
					if (!isArray)
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
				bool isArray = m_stk.top()->m_internal_entercnt != 0 && m_stk.top()->isRepeatingDataElement( dataelem);
				if (!isArray)
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

static bool nodeProperty_hasGlobal( const QWidget* widget, const QVariant& )
{
	foreach (const QString& prop, widget->dynamicPropertyNames())
	{
		if (prop.startsWith( "global:")) return true;
	}
	return false;
}

void WidgetVisitor::readGlobals( const QHash<QString,QVariant>& globals)
{
	if (m_stk.isEmpty()) return;
	foreach (const State::Assignment& assignment, m_stk.top()->m_globals)
	{
		QHash<QString,QVariant>::const_iterator gi = globals.find( assignment.first);
		if (gi != globals.end())
		{
			setProperty( assignment.second, gi.value());
		}
	}
}

void WidgetVisitor::writeGlobals( QHash<QString,QVariant>& globals)
{
	if (m_stk.isEmpty()) return;
	foreach (const State::Assignment& assignment, m_stk.top()->m_globals)
	{
		globals[ assignment.first] = property( assignment.second);
	}
}

void WidgetVisitor::do_readGlobals( const QHash<QString,QVariant>& globals)
{
	foreach (QWidget* wdg, findSubNodes( nodeProperty_hasGlobal))
	{
		WidgetVisitor chldvisitor( wdg);
		chldvisitor.readGlobals( globals);
	}
}

void WidgetVisitor::do_writeGlobals( QHash<QString,QVariant>& globals)
{
	foreach (QWidget* wdg, findSubNodes( nodeProperty_hasGlobal))
	{
		WidgetVisitor chldvisitor( wdg);
		chldvisitor.writeGlobals( globals);
	}
}

static bool nodeProperty_hasAssignment( const QWidget* widget, const QVariant& )
{
	foreach (const QString& prop, widget->dynamicPropertyNames())
	{
		if (prop.startsWith( "assign:")) return true;
	}
	return false;
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

void WidgetVisitor::do_readAssignments()
{
	foreach (QWidget* wdg, findSubNodes( nodeProperty_hasAssignment))
	{
		WidgetVisitor chldvisitor( wdg);
		chldvisitor.readAssignments();
	}
}

void WidgetVisitor::do_writeAssignments()
{
	foreach (QWidget* wdg, findSubNodes( nodeProperty_hasAssignment))
	{
		WidgetVisitor chldvisitor( wdg);
		chldvisitor.writeAssignments();
	}
}

void WidgetVisitor::connectOnChangeListener( WidgetListener& listener)
{
	if (!m_stk.isEmpty())
	{
		m_stk.top()->connectOnChangeSignals( listener);
	}
}

void WidgetVisitor::ERROR( const char* msg, const QString& arg) const
{
	logError( widget(), msg, QString( arg));
}

static bool nodeProperty_hasDataSlot( WidgetVisitor::DataSignalType type, const QWidget* widget, const QVariant& cond)
{
	static const char* ar[] = {"dataslot:onload","dataslot:onchange","dataslot:domainchange"};
	QVariant dataslots = widget->property( ar[(int)type]);
	int idx = 0;
	while ((idx=dataslots.toString().indexOf( cond.toString(), idx)) >= 0)
	{
		idx += cond.toString().length();
		QString dd = dataslots.toString();
		if (dd.size() == idx || dd.at(idx) == ' ' || dd.at(idx) == ',') return true;
	}
	return false;
}

static bool nodeProperty_hasDataSlot_onload( const QWidget* widget, const QVariant& cond)
{
	return nodeProperty_hasDataSlot( WidgetVisitor::OnLoad, widget, cond);
}
static bool nodeProperty_hasDataSlot_onchange( const QWidget* widget, const QVariant& cond)
{
	return nodeProperty_hasDataSlot( WidgetVisitor::OnChange, widget, cond);
}
static bool nodeProperty_hasDataSlot_domainchange( const QWidget* widget, const QVariant& cond)
{
	return nodeProperty_hasDataSlot( WidgetVisitor::DomainChange, widget, cond);
}

QList<QWidget*> WidgetVisitor::get_datasignal_receivers( DataSignalType type)
{
	QList<QWidget*> rt;
	if (m_stk.isEmpty()) return rt;

	foreach (const QString& receiverprop, m_stk.top()->m_datasignals.id[(int)type])
	{
		TRACE_STATUS( "find datasignal receiver", className(), objectName(), receiverprop);
		QVariant receiverid = resolve( receiverprop);
		QString receiveridstr = receiverid.toString();
		QWidget* rcvwidget;

		if (is_widgetid( receiveridstr))
		{
			WidgetVisitor mainvisitor( uirootwidget());
			rt.append( mainvisitor.findSubNodes( nodeProperty_hasWidgetId, receiverid));
			foreach (QWidget* rcvwidget, rt) TRACE_STATUS( "found widget by address", rcvwidget->metaObject()->className(), rcvwidget->objectName(), rcvwidget->widgetid());
		}
		else if ((rcvwidget = get_widget_reference( receiveridstr)) != 0)
		{
			TRACE_STATUS( "found widget reference", rcvwidget->metaObject()->className(), rcvwidget->objectName(), rcvwidget->widgetid());
			rt.append( rcvwidget);
		}
		else
		{
			WidgetVisitor mainvisitor( uirootwidget());
			switch (type)
			{
				case OnLoad:
					rt.append( mainvisitor.findSubNodes( nodeProperty_hasDataSlot_onload, receiverid));
					break;
				case OnChange:
					rt.append( mainvisitor.findSubNodes( nodeProperty_hasDataSlot_onchange, receiverid));
					break;
				case DomainChange:
					rt.append( mainvisitor.findSubNodes( nodeProperty_hasDataSlot_domainchange, receiverid));
					break;
			}
			foreach (QWidget* rcvwidget, rt) TRACE_STATUS( "found widget by data slot identifier", rcvwidget->metaObject()->className(), rcvwidget->objectName(), rcvwidget->widgetid());
		}
	}
	return rt;
}



